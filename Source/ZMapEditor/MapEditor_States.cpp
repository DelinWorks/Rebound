#include "MapEditor.h"

USING_NS_CC;
using namespace backend;

using namespace GameUtils;

ax::Rect MapEditor::createSelection(V2D start_pos, V2D end_pos, I32 _tileSize, SelectionBox::Box& box)
{
    start_pos.x -= _tileSize / 2;
    start_pos.y -= _tileSize / 2;
    end_pos.x -= _tileSize / 2;
    end_pos.y -= _tileSize / 2;
    start_pos = V2D(snap(start_pos.x, _tileSize), snap(start_pos.y, _tileSize));
    end_pos = V2D(snap(end_pos.x, _tileSize), snap(end_pos.y, _tileSize));
    start_pos.x += _tileSize / 2;
    start_pos.y += _tileSize / 2;
    end_pos.x += _tileSize / 2;
    end_pos.y += _tileSize / 2;
    if (end_pos.x > start_pos.x)
    {
        start_pos.x = start_pos.x - _tileSize / 2;
        end_pos.x = end_pos.x + _tileSize / 2;
    }
    if (end_pos.x <= start_pos.x)
    {
        start_pos.x = start_pos.x + _tileSize / 2;
        end_pos.x = end_pos.x - _tileSize / 2;
    }
    if (end_pos.y > start_pos.y)
    {
        start_pos.y = start_pos.y - _tileSize / 2;
        end_pos.y = end_pos.y + _tileSize / 2;
    }
    if (end_pos.y <= start_pos.y)
    {
        start_pos.y = start_pos.y + _tileSize / 2;
        end_pos.y = end_pos.y - _tileSize / 2;
    }
    start_pos.x = clamp(start_pos.x, -map->_mapSize.x * _tileSize, map->_mapSize.x * _tileSize);
    start_pos.y = clamp(start_pos.y, -map->_mapSize.y * _tileSize, map->_mapSize.y * _tileSize);
    end_pos.x = clamp(end_pos.x, -map->_mapSize.x * _tileSize, map->_mapSize.x * _tileSize);
    end_pos.y = clamp(end_pos.y, -map->_mapSize.y * _tileSize, map->_mapSize.y * _tileSize);
    if (start_pos.x > end_pos.x) {
        auto temp = end_pos.x;
        end_pos.x = start_pos.x;
        start_pos.x = temp;
    }
    if (start_pos.y > end_pos.y) {
        auto temp = end_pos.y;
        end_pos.y = start_pos.y;
        start_pos.y = temp;
    }
    box.left.begin = V2D(start_pos.x, start_pos.y);
    box.left.end = V2D(start_pos.x, end_pos.y);
    box.bottom.begin = V2D(start_pos.x, start_pos.y);
    box.bottom.end = V2D(end_pos.x, start_pos.y);
    box.right.begin = V2D(start_pos.x, end_pos.y);
    box.right.end = V2D(end_pos.x, end_pos.y);
    box.top.begin = V2D(end_pos.x, start_pos.y);
    box.top.end = V2D(end_pos.x, end_pos.y);
    box.first.p0 = V2D(start_pos.x, start_pos.y);
    box.first.p1 = V2D(start_pos.x, end_pos.y);
    box.first.p2 = V2D(end_pos.x, end_pos.y);
    box.second.p0 = V2D(end_pos.x, end_pos.y);
    box.second.p1 = V2D(end_pos.x, start_pos.y);
    box.second.p2 = V2D(start_pos.x, start_pos.y);
    return Rect(start_pos.x, start_pos.y, end_pos.x, end_pos.y);
}

Rect MapEditor::createEditToolSelectionBox(V2D start_pos, V2D end_pos, I32 _tileSize)
{
    if (!removeSelectionNode) {
        removeSelectionNode = DrawNode::create(1.0);
        gridNode->addChild(removeSelectionNode, 2);
    }
    SelectionBox::Box box;
    Rect rect = createSelection(start_pos, end_pos, _tileSize, box);

    rect.origin.x = rect.origin.x / map->_tileSize.x;
    rect.origin.y = rect.origin.y / map->_tileSize.y;
    rect.size.x = rect.size.x / map->_tileSize.x;
    rect.size.y = rect.size.y / map->_tileSize.y;

    auto color1 = SELECTION_SQUARE_ALLOWED;
    auto color2 = SELECTION_SQUARE_TRI_ALLOWED;

    if (TEditMode == TileMapEditMode::REMOVE) {
        color1 = SELECTION_SQUARE_DENIED;
        color2 = SELECTION_SQUARE_TRI_DENIED;
    }

    removeSelectionNode->clear();
    removeSelectionNode->drawLine(box.left.begin, box.left.end, color1);
    removeSelectionNode->drawLine(box.bottom.begin, box.bottom.end, color1);
    removeSelectionNode->drawLine(box.right.begin, box.right.end, color1);
    removeSelectionNode->drawLine(box.top.begin, box.top.end, color1);
    removeSelectionNode->drawTriangle(box.first.p0, box.first.p1, box.first.p2, color2);
    removeSelectionNode->drawTriangle(box.second.p0, box.second.p1, box.second.p2, color2);
    return rect;
}

void MapEditor::setCameraScaleUiText(F32 scale)
{
    char buff[8];
    snprintf(buff, sizeof(buff), "x%g", scale);
    std::string buffAsStdStr = buff;
    cameraScaleL->setString(buffAsStdStr);

    cameraScaleB->enableSelf();
    if (scale < 1.0)
        cameraScaleB->icon->setSpriteFrame("editor_zoomed_in");
    else if (scale > 1.0)
        cameraScaleB->icon->setSpriteFrame("editor_zoomed_out");
    else
    {
        cameraScaleB->icon->setSpriteFrame("editor_zoom_aligned");
        cameraScaleB->disableSelf();
    }
}

void MapEditor::setWorldBoundsLayerColorTransforms(VirtualCamera* cam)
{
    BottomMapSizeNode->setScaleX(cameraScale);
    BottomMapSizeNode->setScaleY(cameraScale);
    BottomMapSizeNode->setPositionX(cam->getPositionX());

    TopMapSizeNode->setScaleX(cameraScale);
    TopMapSizeNode->setScaleY(cameraScale);
    TopMapSizeNode->setPositionX(cam->getPositionX());

    RightMapSizeNode->setScaleX(cameraScale);
    RightMapSizeNode->setScaleY(cameraScale);
    RightMapSizeNode->setPositionY(cam->getPositionY());

    LeftMapSizeNode->setScaleX(cameraScale);
    LeftMapSizeNode->setScaleY(cameraScale);
    LeftMapSizeNode->setPositionY(cam->getPositionY());
}

void MapEditor::editorTileFlipRotateUpdateState()
{
    if (editorTileCoords.isH) tileFlipH->enableIconHighlight(); else tileFlipH->disableIconHighlight();
    if (editorTileCoords.isV) tileFlipV->enableIconHighlight(); else tileFlipV->disableIconHighlight();
    if (editorTileCoords.is90) tileRot90->enableIconHighlight(); else tileRot90->disableIconHighlight();
    _mousePosTileHint = INVALID_LOCATION;
}

#define TM_EDIT_BUTTON_HIGHLIGH1 Color3B(131, 255, 146)
#define TM_EDIT_BUTTON_HIGHLIGH2 Color3B(255, 201, 71)

void MapEditor::updateTileMapEditModeState()
{
    placeB->disableIconHighlight();
    removeB->disableIconHighlight();
    bucketB->disableIconHighlight();
    selectB->disableIconHighlight();

    switch (TEditMode) {
    case TileMapEditMode::PLACE:
        placeB->enableIconHighlight(TM_EDIT_BUTTON_HIGHLIGH1);
        break;
    case TileMapEditMode::REMOVE:
        removeB->enableIconHighlight(TM_EDIT_BUTTON_HIGHLIGH2);
        break;
    case TileMapEditMode::BUCKET:
        bucketB->enableIconHighlight(TM_EDIT_BUTTON_HIGHLIGH1);
        break;
    case TileMapEditMode::SELECT:
        selectB->enableIconHighlight();
        break;
    }
}

void MapEditor::editorUndoRedoAlterStacks(std::function<void(EditorToolbox::UndoRedoState&)> func)
{
    for (auto& _ : _undo.pool)
        func(_);
    for (auto& _ : _redo.pool)
        func(_);
}

void MapEditor::editorUndoRedoMax(int m)
{
    _undo.set_capacity(m);
    _redo.set_capacity(m);
}

void MapEditor::editorUndoRedoUpdateState()
{
    if (_undo.size() == 0)
        undoB->disable();
    else
        undoB->enable();

    if (_redo.size() == 0)
        redoB->disable();
    else
        redoB->enable();
}

void MapEditor::editorUndo()
{
    if (_undo.size() > 0) {
        auto& state = _undo.top();
        _redo.push(state);
        state.applyUndoState();
        _undo.pop();
    }
    editorUndoRedoUpdateState();
}

void MapEditor::editorRedo()
{
    if (_redo.size() > 0) {
        auto& state = _redo.top();
        _undo.push(_redo.top());
        _redo.top().applyRedoState();
        _redo.pop();
    }
    editorUndoRedoUpdateState();
}

void MapEditor::editorPushUndoState() {
    _undo.push(EditorToolbox::UndoRedoState());
    releaseUndoRedoStacks(true);
    editorUndoRedoUpdateState();
}

EditorToolbox::UndoRedoState& MapEditor::editorTopUndoStateOrDefault()
{
    if (_undo.size() == 0)
        editorPushUndoState();
    editorUndoRedoUpdateState();
    return _undo.top();
}

EditorToolbox::UndoRedoState* MapEditor::editorTopUndoStateOrNull()
{
    return _undo.size() > 0 ? (&_undo.top()) : nullptr;
}

void MapEditor::editorLayerControlsUpdateState()
{
    moveLayerUpBtn->disableSelf();
    moveLayerDownBtn->disableSelf();

    auto& l = _layerManager;

    if (l.getLayerIndex() >= l.getLayerCount())
    {
        renameLayerBtn->disableSelf();
        removeLayerBtn->disableSelf();
    }
    else
    {
        if (l.getLayerCount() > 1)
        {
            if (l.getLayerIndex() != 0)
                moveLayerUpBtn->enableSelf();
            if (l.getLayerIndex() != l.getLayerCount() - 1)
                moveLayerDownBtn->enableSelf();
        }

        renameLayerBtn->enableSelf();
        removeLayerBtn->enableSelf();
    }
}

void MapEditor::editorPushUndoColorPalette()
{
    editorPushUndoState();
    auto& state = _undo.top();
    state.setAction(EditorToolbox::UNDOREDO_COLOR_PALETTE);
    state.affectedColors.setHSVWheelPointer(currentShownWheel);
    state.affectedColors.manager = &channelMgr;
    state.affectedColors.color_idx = channelId;
    state.affectedColors.setColorPrev(channelMgr.getColor(channelId));
}

void MapEditor::releaseUndoRedoStacks(bool keepUndo)
{
    if (!keepUndo)
        while (_undo.size() != 0)
        {
            auto& state = _undo.top();
            state.releaseData();
            _undo.pop();
        }

    while (_redo.size() != 0)
    {
        auto& state = _redo.top();
        state.releaseData();
        _redo.pop();
    }
}
