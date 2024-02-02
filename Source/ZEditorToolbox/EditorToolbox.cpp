#include "EditorToolbox.h"

void EditorToolbox::UndoRedoState::setAction(UndoRedoCategory action)
{
    this->action = action;
}

EditorToolbox::UndoRedoCategory EditorToolbox::UndoRedoState::getAction()
{
    return action;
}

void EditorToolbox::UndoRedoState::applyUndoState()
{
    switch (action) {
    case UNDOREDO_COLOR_PALETTE:
        applyUndoStateColorPaletteEdit();
        break;
    case UNDOREDO_TILEMAP:
        applyUndoStateTilemapEdit();
        break;
    case UNDOREDO_LAYER_MODIFY:
        applyUndoStateLayerEdit();
        break;
    }
}

void EditorToolbox::UndoRedoState::applyRedoState()
{
    switch (action) {
    case UNDOREDO_COLOR_PALETTE:
        applyRedoStateColorPaletteEdit();
        break;
    case UNDOREDO_TILEMAP:
        applyRedoStateTilemapEdit();
        break;
    case UNDOREDO_LAYER_MODIFY:
        applyRedoStateLayerEdit();
        break;
    }
}

void EditorToolbox::UndoRedoState::releaseData()
{
    if (affectedLayers.action == UNDOREDO_LAYER_ADD ||
        affectedLayers.action == UNDOREDO_LAYER_DELETE)
        AX_SAFE_RELEASE(affectedLayers.layer.layer);
}

void EditorToolbox::UndoRedoState::applyUndoStateTilemapEdit()
{
    affectedTiles.map->bindLayer(affectedTiles.layer_idx);
    for (auto& _ : affectedTiles.prev_tiles)
        affectedTiles.map->setTileAt(Vec2(_.first.x, _.first.y), _.second);
}

void EditorToolbox::UndoRedoState::applyRedoStateTilemapEdit()
{
    affectedTiles.map->bindLayer(affectedTiles.layer_idx);
    for (auto& _ : affectedTiles.next_tiles)
        affectedTiles.map->setTileAt(Vec2(_.first.x, _.first.y), _.second);
}

void EditorToolbox::UndoRedoState::applyUndoStateLayerEdit()
{
    auto& m = affectedLayers;
    switch (m.action)
    {
    case UNDOREDO_LAYER_ADD:
        m.manager->removeLayer(m.prev_layer_idx);
        break;
    case UNDOREDO_LAYER_DELETE:
        m.manager->layers.push_back(m.layer.name);
        m.manager->tileMap->addLayer(m.layer.layer);
        m.manager->addListElement(m.layer.name);
        m.manager->moveLayer(m.manager->getLayerCount() - 1, m.prev_layer_idx);
        break;
    case UNDOREDO_LAYER_RENAME:
        m.manager->renameGeneralLayer(m.prev_layer_idx, m.layer.name);
        break;
    case UNDOREDO_LAYER_MOVE:
        m.manager->moveLayer(m.next_layer_idx, m.prev_layer_idx);
        break;
    }
}

void EditorToolbox::UndoRedoState::applyRedoStateLayerEdit()
{
    auto& m = affectedLayers;
    switch (m.action)
    {
    case UNDOREDO_LAYER_ADD:
        m.manager->layers.push_back(m.layer.name);
        m.manager->tileMap->addLayer(m.layer.layer);
        m.manager->addListElement(m.layer.name);
        m.manager->moveLayer(m.manager->getLayerCount() - 1, m.prev_layer_idx);
        break;
    case UNDOREDO_LAYER_DELETE:
        m.manager->removeLayer(m.prev_layer_idx);
        break;
    case UNDOREDO_LAYER_RENAME:
        m.manager->renameGeneralLayer(m.prev_layer_idx, m.new_name);
        break;
    case UNDOREDO_LAYER_MOVE:
        m.manager->moveLayer(m.prev_layer_idx, m.next_layer_idx);
        break;
    }
}

void EditorToolbox::UndoRedoState::applyUndoStateColorPaletteEdit()
{
    auto m = affectedColors;
    m.manager->setColor(m.color_idx, m.color_prev);
    if (m.wheel && m.wheel->currentChannel == m.color_idx)
        m.wheel->updateColorValues(m.color_prev.color);
}

void EditorToolbox::UndoRedoState::applyRedoStateColorPaletteEdit()
{
    auto m = affectedColors;
    m.manager->setColor(m.color_idx, m.color_next);
    if (m.wheel && m.wheel->currentChannel == m.color_idx)
        m.wheel->updateColorValues(m.color_next.color);
}

void EditorToolbox::UndoRedoAffectedTiles::allocateBuckets() {}

void EditorToolbox::UndoRedoAffectedTiles::addOrIgnoreTilePrev(ax::Vec2 pos, U32 gid)
{
    if (prev_tiles.find(pos) == prev_tiles.end())
        prev_tiles.emplace(pos, gid);
}

void EditorToolbox::UndoRedoAffectedTiles::addOrIgnoreTileNext(ax::Vec2 pos, U32 gid)
{
    if (next_tiles.find(pos) == next_tiles.end())
        next_tiles.emplace(pos, gid);
}

void EditorToolbox::UndoRedoAffectedColorPalette::setHSVWheelPointer(CUI::HSVWheel* wheel)
{
    this->wheel = wheel;
}

void EditorToolbox::UndoRedoAffectedColorPalette::setColorPrev(ColorChannel color)
{
    color_prev = color;
}

void EditorToolbox::UndoRedoAffectedColorPalette::setColorNext(ColorChannel color)
{
    color_next = color;
}
