#pragma once

#include "Helper/short_types.h"
#include "Helper/Math.h"
#include "Nodes/TileMapSystem.h"
#include <Types.h>
#include "ColorChannelManager.h"
#include "Nodes/ui/uiHSVWheel.h"
#include "LayerManager.h"

USING_NS_AX;

namespace EditorToolbox {

    enum UndoRedoCategory {
        UNDOREDO_NONE = 0,
        UNDOREDO_COLOR_PALETTE = 1,
        UNDOREDO_TILEMAP = 2,
        UNDOREDO_LAYER_MODIFY = 3,
    };

    class UndoRedoAffectedColorPalette {
    public:
        ColorChannelManager* manager;
        U16 color_idx;
        ColorChannel color_prev;
        ColorChannel color_next;
        CUI::HSVWheel* wheel = nullptr;

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

    enum UndoRedoLayerModifyCategory {
        UNDOREDO_LAYER_ADD = 0,
        UNDOREDO_LAYER_MOVE = 1,
        UNDOREDO_LAYER_RENAME = 2,
        UNDOREDO_LAYER_DELETE = 3,
    };

    class UndoRedoAffectedLayer {
    public:
        LayerManager* manager = nullptr;
        UndoRedoLayerModifyCategory action;
        HeterogeneousLayer layer;
        U16 prev_layer_idx = 0;
        U16 next_layer_idx = 0;
        std::wstring new_name;
    };

    class UndoRedoState {
    public:

        UndoRedoAffectedColorPalette affectedColors;
        UndoRedoAffectedTiles affectedTiles;
        UndoRedoAffectedLayer affectedLayers;

        void setAction(UndoRedoCategory action);
        UndoRedoCategory getAction();

        void applyUndoState();
        void applyRedoState();

        void releaseData();

    private:
        UndoRedoCategory action = UndoRedoCategory::UNDOREDO_NONE;

        void applyUndoStateColorPaletteEdit();
        void applyRedoStateColorPaletteEdit();

        void applyUndoStateTilemapEdit();
        void applyRedoStateTilemapEdit();

        void applyUndoStateLayerEdit();
        void applyRedoStateLayerEdit();
    };
}