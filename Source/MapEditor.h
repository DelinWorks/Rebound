/****************************************************************************
* 
 Copyright (c) 2017-2018 Xiamen Yaji Software Co., Ltd.
 
 http://www.ax-x.org
 
 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:
 
 The above copyright notice and this permission notice shall be included in
 all copies or substantial portions of the Software.
 
 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.
 ****************************************************************************/
//#pragma once

#ifndef MAPEDITOR_H
#define MAPEDITOR_H

#include "shared_scenes/GameUtils.h"
#include "fmod/include_fmod.h"
#include "sqlite3enc/sqleet.h"
#include "math/FastRNG.h"

#ifdef WIN32
#include "windows.h"
#include "psapi.h"
#endif

#include "Helper/Random.h"
#include <iostream>
#include <AppDelegate.h>
#include "string_manipulation_lib/zlibString.hpp"
#include "string_manipulation_lib/base64.hpp"
#include "renderer/backend/Device.h"
#include "Helper/Logging.hpp"
#include <shared_scenes/GameSingleton.h>

#include "Helper/Math.h"
using namespace Math;

#include "ZEditorToolbox/EditorToolbox.h"
#include "Helper/SelectionBox.h"
#include "Helper/ChangeValue.h"
#include "Nodes/ui/include_ui.h"
#include "Components/Components.h"
#include "Nodes/VirtualWorld.h"
#include "Helper/HAFStack.hpp"
#include "ZEditorToolbox/SelectableObject.h"
#include "Nodes/TileMapSystem.h"
#include "ZEditorToolbox/LayerManager.h"

#define GRID_COLOR Color4F::BLACK
#define LAYER_BACKGROUND_COLOR Color4B(40, 47, 64, 255)
#define LAYER_BACKGROUND_BOUND_COLOR Color4B(30, 37, 44, 255)
#define LINE_BACKGROUND_BOUND_COLOR Color4F(0.7, 0.7, 0.7, 1)
#define SELECTION_SQUARE_ALLOWED Color4F(0, 0.58f, 1.0f, 0.8f)
#define SELECTION_SQUARE_DENIED Color4F(1, 0.19f, 0.19f, 0.8f)
#define SELECTION_SQUARE_TRI_ALLOWED Color4F(0, 0.58f, 1.0f, 0.08f)
#define SELECTION_SQUARE_TRI_DENIED Color4F(1, 0.19f, 0.19f, 0.08f)

using namespace TileSystem;

enum class TileMapEditMode {
    NONE = 0,
    PLACE = 1,
    REMOVE = 2,
    BUCKET = 3,
    SELECT = 4
};

class MapEditor : public ax::Scene,
                  public SceneInputManager,
                  public VirtualWorldManager,
                  public GameUtils::SignalHandeler
{
public:
    static ax::Scene* createScene();
    CREATE_FUNC(MapEditor);

    struct Tileset {
        std::string tilesetName;
        V2D tileSize;
        std::string texture_id;
    };

    std::vector<MapEditor::Tileset> _tilesets;

    ~MapEditor();

    F32 global_dt;
    F32 fps_dt;
    void update(F32 dt) override;
    void tick(F32 dt);
    F32 updateSchedTime;
    F32 elapsedDt = 0;
    void perSecondUpdate(F32 dt);
    void lateUpdate(F32 dt);

    void tileMapEditUpdate(V2D prev, V2D next);
    void tileMapModifyRegion(F32 x, F32 y, F32 _width, F32 _height);

    virtual bool init();
    void onInitDone(F32 dt);

    void onKeyHold(ax::EventKeyboard::KeyCode keyCode, ax::Event* event);
    void onKeyPressed(ax::EventKeyboard::KeyCode keyCode, ax::Event* event);
    void onKeyReleased(ax::EventKeyboard::KeyCode keyCode, ax::Event* event);
    void onMouseDown(ax::Event* event);
    void onMouseUp(ax::Event* event);
    bool hasMouseMoved = false;
    void onMouseMove(ax::Event* event);
    void setCameraScaleIndex(I32 index = 0, bool shiftTransform = true);
    void onMouseScroll(ax::Event* event);
    bool onTouchBegan(ax::Touch* touch, ax::Event* event);
    void onTouchMoved(ax::Touch* touch, ax::Event* event);
    void onTouchEnded(ax::Touch* touch, ax::Event* event);
    void onTouchCancelled(ax::Touch* touch, ax::Event* event);
    void handleSignal(std::string signal);
    
    void visit(Renderer* renderer, const Mat4& parentTransform, uint32_t parentFlags) override;

    TilesetArray* tilesetArr;

    void updateDirectorToStatsCount(I32 tileCount, I32 chunkCount);
    void menuCloseCallback(ax::Ref* pSender);
    void buildEntireUi();

    FastRNG rng;

    sqlite3* pdb;

    V2D _mousePosTileHint;
    std::set<V2D> _editorPrevMoveTiles;
    TileSystem::Map* map;

    TileArray* tarr;

    ax::Size visibleSize;
    ax::Node* grid;
    ax::DrawNode* deltaEditing;
    ax::Node* cameraLocation;
    V2D oldSelectionPlace;
    V2D selectionPlace;
    V2D chunkSelectionPlace;
    F32 cameraScale;
    V2D cameraPos = V2D::ZERO;
    I32 cameraScaleIndex = 10;
    F32 possibleCameraScales[18] = { 0.05, 0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7, 0.8, 1.0, 1.5, 2.0, 3.0, 4.0, 8.0, 16.0, 32.0, 64.0 };
    CUI::Button* cameraScaleB;
    CUI::Label* cameraScaleL;

#define OBJECT_MODE 0
#define TILE_MAP_MODE 1

    CUI::DropDown* modeDropdown;

    ax::Node* selectionNode;
    ax::DrawNode* selectionPlaceSquare;
    ax::DrawNode* worldCoordsLines;
    ax::DrawNode* cameraCenterIndicator;

    EditorToolbox::ColorChannelManager channelMgr;

    ChangeValue<bool> gridHideValue;
    ChangeValue<float> gridOpacityValue;

    bool isInitDone = false;
    bool isTouchNew = false;

    bool hideEditorUIState = false;
    ax::Node* rebuildableUiNodes;
    void rebuildEntireUi();
    bool isEditorDragging = false;
    bool isEditorHideGrid = false;
    bool isLocationEditable = false;
    bool isPlacing = false;
    ax::Rect createSelection(V2D start_pos, V2D end_pos, I32 _tileSize, SelectionBox::Box& box);
    ax::Rect createEditToolSelectionBox(V2D start_pos, V2D end_pos, I32 _tileSize);
    V2D removeSelectionStartPos;
    ax::DrawNode* removeSelectionNode;
    V2D selectionPosition;
    bool isTileMapRect = false;

    bool isCtrlTMRemove = false;
    CUI::Button* placeB;
    CUI::Button* removeB;
    CUI::Button* bucketB;
    CUI::Button* selectB;
    void setTileMapEditMode(TileMapEditMode mode);
    void updateTileMapEditModeState();
    TileMapEditMode TEditMode;

    TileSystem::TileTexCoords editorTileCoords;

    std::string dbPath;
    std::string dbName = "map.daumap";
    std::string errorInCase = "";
    I32 loadStep = 1;

    ax::Node* uiNode;
    ax::Node* uiNodeNonFollow;

    ax::Node* gridNode;
    ax::Node* TopMapSizeNode;
    ax::Node* BottomMapSizeNode;
    ax::Node* LeftMapSizeNode;
    ax::Node* RightMapSizeNode;
    ax::DrawNode* WorldBoundsLimit;

    F32 _hoverToolTipTime = 0.0f;
    CUI::GUI* _hoverToolTipPointer;
    CUI::ToolTip* _editorToolTip;
    CUI::Label* _debugText;
    CUI::ImageView* _tilesetPicker;

    void setCameraScaleUiText(F32 scale);
    void setWorldBoundsLayerColorTransforms(VirtualCamera* cam);

    Rect editorWASDCamMoveRect;

    bool isCtrlPressed = false;
    bool isShiftPressed = false;
    void releaseUndoRedoStacks(bool keepUndo);
    HeapAllocatedFixedStack<EditorToolbox::UndoRedoState> _undo;
    HeapAllocatedFixedStack<EditorToolbox::UndoRedoState> _redo;

    CUI::HSVWheel* currentShownWheel;
    static void fillContainerColorGrid(EditorToolbox::ColorChannelManager* m, CUI::Container* c,
        int rows, int columns, int page, std::function<void(int i)> _onColorSelect);

    U16 channelId = 0;
    void editorPushUndoColorPalette();
    CUI::Container* createFledgedHSVPanel();

    CUI::Button* tileFlipH;
    CUI::Button* tileFlipV;
    CUI::Button* tileRot90;
    void editorTileFlipRotateUpdateState();

    void editorUndoRedoAlterStacks(std::function<void(EditorToolbox::UndoRedoState&)>);
    void editorUndoRedoMax(int m);
    void editorUndoRedoUpdateState();
    void editorUndo();
    void editorRedo();
    void editorPushUndoState();

    EditorToolbox::UndoRedoState& editorTopUndoStateOrDefault();
    EditorToolbox::UndoRedoState* editorTopUndoStateOrNull();
    CUI::Button* undoB;
    CUI::Button* redoB;

    CUI::Button* createLayerBtn;
    CUI::Button* moveLayerUpBtn;
    CUI::Button* moveLayerDownBtn;
    CUI::Button* renameLayerBtn;
    CUI::Button* removeLayerBtn;

    //U32 _boundLayer = UINT32_MAX;
    //CUI::List* _layersList;
    //std::vector<std::wstring> _layers;
    //void bindLayer(U32 index);
    //void addGeneralLayer(std::wstring name);
    //void renameGeneralLayer(U32 index, std::wstring newName);
    //void removeLayer(U32 index);
    //void moveLayer(U32 index, U32 newIndex);

    EditorToolbox::LayerManager _layerManager;

    void editorLayerControlsUpdateState();

    bool isSelectableHoveredLastFrame = false;
    bool isSelectableHovered = false;
    std::vector<Selectable*> _selectables;
};

#endif