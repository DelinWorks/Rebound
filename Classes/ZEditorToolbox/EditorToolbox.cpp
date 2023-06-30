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
    }
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

void EditorToolbox::UndoRedoAffectedTiles::addOrIgnoreTilePrev(ax::Vec2 pos, u32 gid)
{
    if (prev_tiles.find(pos) == prev_tiles.end())
        prev_tiles.emplace(pos, gid);
}

void EditorToolbox::UndoRedoAffectedTiles::addOrIgnoreTileNext(ax::Vec2 pos, u32 gid)
{
    if (next_tiles.find(pos) == next_tiles.end())
        next_tiles.emplace(pos, gid);
}

void EditorToolbox::UndoRedoAffectedColorPalette::setHSVWheelPointer(CUI::HSVWheel* wheel)
{
    this->wheel = wheel;
}

EditorToolbox::UndoRedoAffectedColorPalette::~UndoRedoAffectedColorPalette()
{
}

void EditorToolbox::UndoRedoAffectedColorPalette::setColorPrev(ColorChannel color)
{
    color_prev = color;
}

void EditorToolbox::UndoRedoAffectedColorPalette::setColorNext(ColorChannel color)
{
    color_next = color;
}
