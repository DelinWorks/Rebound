#pragma once

#include "Helper/short_types.h"
#include "Helper/Math.h"
#include "Nodes/TileMapSystem.h"
#include <Types.h>
#include "ColorChannelManager.h"
#include "Nodes/ui/uiHSVWheel.h"

USING_NS_AX;

namespace EditorToolbox {

    enum UndoRedoCategory {
        UNDOREDO_NONE = 0,
        UNDOREDO_COLOR_PALETTE = 1,
        UNDOREDO_TILEMAP = 2,
    };

    class UndoRedoAffectedColorPalette {
    public:
        ColorChannelManager* manager;
        U16 color_idx;
        ColorChannel color_prev;
        ColorChannel color_next;
        CUI::HSVWheel* wheel = nullptr;

        ~UndoRedoAffectedColorPalette();

        void setHSVWheelPointer(CUI::HSVWheel* wheel);
        void setColorPrev(ColorChannel color);
        void setColorNext(ColorChannel color);
    };

    class UndoRedoAffectedTiles {
    public:
        TileSystem::Map* map;
        U16 layer_idx;
        std::unordered_map<Vec2Hashable, U32> prev_tiles;
        std::unordered_map<Vec2Hashable, U32> next_tiles;

        void allocateBuckets();
        void addOrIgnoreTilePrev(ax::Vec2 pos, U32 gid);
        void addOrIgnoreTileNext(ax::Vec2 pos, U32 gid);
    };

    class UndoRedoState {
    public:

        UndoRedoAffectedColorPalette affectedColors;
        UndoRedoAffectedTiles affectedTiles;

        void setAction(UndoRedoCategory action);
        UndoRedoCategory getAction();

        void applyUndoState();
        void applyRedoState();

    private:
        UndoRedoCategory action = UndoRedoCategory::UNDOREDO_NONE;

        void applyUndoStateTilemapEdit();
        void applyRedoStateTilemapEdit();

        void applyUndoStateColorPaletteEdit();
        void applyRedoStateColorPaletteEdit();
    };
}