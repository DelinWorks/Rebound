#include "MapEditor.h"

USING_NS_CC;
using namespace backend;

using namespace GameUtils;

ax::Rect MapEditor::createSelection(ax::Vec2 start_pos, ax::Vec2 end_pos, i32 _tileSize, SelectionBox::Box& box)
{
    start_pos.x -= _tileSize / 2;
    start_pos.y -= _tileSize / 2;
    end_pos.x -= _tileSize / 2;
    end_pos.y -= _tileSize / 2;
    start_pos = Vec2(snap(start_pos.x, _tileSize), snap(start_pos.y, _tileSize));
    end_pos = Vec2(snap(end_pos.x, _tileSize), snap(end_pos.y, _tileSize));
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
    box.left.begin = Vec2(start_pos.x, start_pos.y);
    box.left.end = Vec2(start_pos.x, end_pos.y);
    box.bottom.begin = Vec2(start_pos.x, start_pos.y);
    box.bottom.end = Vec2(end_pos.x, start_pos.y);
    box.right.begin = Vec2(start_pos.x, end_pos.y);
    box.right.end = Vec2(end_pos.x, end_pos.y);
    box.top.begin = Vec2(end_pos.x, start_pos.y);
    box.top.end = Vec2(end_pos.x, end_pos.y);
    box.first.p0 = Vec2(start_pos.x, start_pos.y);
    box.first.p1 = Vec2(start_pos.x, end_pos.y);
    box.first.p2 = Vec2(end_pos.x, end_pos.y);
    box.second.p0 = Vec2(end_pos.x, end_pos.y);
    box.second.p1 = Vec2(end_pos.x, start_pos.y);
    box.second.p2 = Vec2(start_pos.x, start_pos.y);
    return Rect(start_pos.x, start_pos.y, end_pos.x, end_pos.y);
}

Rect MapEditor::createEditToolSelectionBox(Vec2 start_pos, Vec2 end_pos, i32 _tileSize)
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

    removeSelectionNode->clear();
    removeSelectionNode->drawLine(box.left.begin, box.left.end, SELECTION_SQUARE_DENIED);
    removeSelectionNode->drawLine(box.bottom.begin, box.bottom.end, SELECTION_SQUARE_DENIED);
    removeSelectionNode->drawLine(box.right.begin, box.right.end, SELECTION_SQUARE_DENIED);
    removeSelectionNode->drawLine(box.top.begin, box.top.end, SELECTION_SQUARE_DENIED);
    removeSelectionNode->drawTriangle(box.first.p0, box.first.p1, box.first.p2, SELECTION_SQUARE_TRI_DENIED);
    removeSelectionNode->drawTriangle(box.second.p0, box.second.p1, box.second.p2, SELECTION_SQUARE_TRI_DENIED);
    return rect;
}

void MapEditor::setCameraScaleUiText(f32 scale)
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
    _redo.reset();
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
