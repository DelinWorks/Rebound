/****************************************************************************
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

#include <string> 
#include <sstream> 
#include <iomanip> 
#include <fstream>
#include <limits>

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

#include "Helper/SelectionBox.h"
#include "Helper/ChangeValue.h"
#include "Helper/Rebound/TileMapSystem.hpp"
#include "Nodes/ui/include_ui.h"
#include "Components/Components.h"

#include "Nodes/VirtualWorld.h"

#include "Helper/DataStructures/HAFStack.hpp"

using namespace TileSystem;

class MapEditor : public ax::Scene, public SceneInputManager, public VirtualWorldManager
{
public:
    static ax::Scene* createScene();
    CREATE_FUNC(MapEditor);

    ~MapEditor();

#if CC_TARGET_PLATFORM == CC_PLATFORM_WIN32
    SIZE_T prevWorkingSetSize;
#endif

    f32 global_dt;
    f32 fps_dt;
    void update(f32 dt) override;
    void tick(f32 dt);
    f32 updateSchedTime;
    f32 elapsedDt = 0;
    void perSecondUpdate(f32 dt);
    void lateUpdate(f32 dt);
    void editUpdate(Vec2& old, Vec2& place, Size& placeStampSize, Size& removeStampSize);
    void editUpdate_place(f32 x, f32 y, f32 _width, f32 _height);
    void editUpdate_remove(f32 x, f32 y, f32 _width, f32 _height);

    virtual bool init();
    void onInitDone(f32 dt);

    void onKeyHold(ax::EventKeyboard::KeyCode keyCode, ax::Event* event);
    void onKeyPressed(ax::EventKeyboard::KeyCode keyCode, ax::Event* event);
    void onKeyReleased(ax::EventKeyboard::KeyCode keyCode, ax::Event* event);
    void onMouseDown(ax::Event* event);
    void onMouseUp(ax::Event* event);
    bool hasMouseMoved = false;
    void onMouseMove(ax::Event* event);
    void setCameraScaleIndex(i32 index = 0);
    void onMouseScroll(ax::Event* event);
    bool onTouchBegan(ax::Touch* touch, ax::Event* event);
    void onTouchMoved(ax::Touch* touch, ax::Event* event);
    void onTouchEnded(ax::Touch* touch, ax::Event* event);
    void onTouchCancelled(ax::Touch* touch, ax::Event* event);
    
    void visit(Renderer* renderer, const Mat4& parentTransform, uint32_t parentFlags) override;

    TilesetArray* tilesetArr;

    void updateDirectorToStatsCount(i32 tileCount, i32 chunkCount);
    void menuCloseCallback(ax::Ref* pSender);
    void buildEntireUi();

    FastRNG rng;

    sqlite3* pdb;

    TileSystem::Map* map;

    TileArray* tarr;

    ax::Sprite*                map_sprite_stamp;
    ax::Texture2D*             map_default_tex;
    ax::backend::ProgramState* map_default_tile_shader;
    ax::BlendFunc*             map_default_tile_shader_blend;

    ax::Size visibleSize;
    ax::Node* grid;
    ax::DrawNode* deltaEditing;
    ax::Node* cameraLocation;
    ax::Vec2 oldSelectionPlace;
    ax::Vec2 selectionPlace;
    ax::Vec2 chunkSelectionPlace;
    f32 cameraScale;
    i32 cameraScaleIndex = 5;
    f32 possibleCameraScales[16] = { 34, 36, 38, 40, 42, 44, 46, 48, 50, 52, 54, 1, 58, 60, 66, 75 };
    CustomUi::Button* cameraScaleB;
    CustomUi::Label* cameraScaleL;

    ax::DrawNode* selectionPlaceSquare;
    ax::DrawNode* selectionPlaceSquareForbidden;
    ax::DrawNode* worldCoordsLines;
    ax::DrawNode* cameraCenterIndicator;

    ChangeValue<bool> gridHideValue;
    ChangeValue<float> gridOpacityValue;

    bool isInitDone = false;
    bool isTouchNew = false;

    ax::Node* rebuildableUiNodes;
    void rebuildEntireUi();
    bool isEditorDragging = false;
    bool isEditorHideGrid = false;
    bool isLocationEditable = false;
    bool isPlacing = false;
    ax::Rect createSelection(ax::Vec2 start_pos, ax::Vec2 end_pos, i32 _tileSize, SelectionBox::Box& box);
    ax::Rect createEditToolSelectionBox(ax::Vec2 start_pos, ax::Vec2 end_pos, i32 _tileSize);
    ax::Vec2 removeSelectionStartPos;
    ax::DrawNode* removeSelectionNode;
    ax::Vec2 selectionPosition;
    bool isRemoving = false;

    TileSystem::TileTexCoords editorTileCoords;

    std::string dbPath;
    std::string dbName = "map.daumap";
    std::string errorInCase = "";
    i32 loadStep = 1;

    ax::Node* uiNode;
    ax::Node* uiNodeNonFollow;

    ax::Node* gridNode;
    ax::Node* TopMapSizeNode;
    ax::Node* BottomMapSizeNode;
    ax::Node* LeftMapSizeNode;
    ax::Node* RightMapSizeNode;
    ax::DrawNode* WorldBoundsLimit;

    CustomUi::Label* _debugText;

    CustomUi::ImageView* _tilesetPicker;

    void setCameraScaleUiText(f32 scale);
    void setWorldBoundsLayerColorTransforms(VirtualCamera* cam);

    bool isCtrlPressed = false;
    HeapAllocatedFixedStack<GameUtils::Editor::UndoRedoState> _undo;
    HeapAllocatedFixedStack<GameUtils::Editor::UndoRedoState> _redo;

    void editorUndoRedoMax(int m);
    void editorUndo();
    void editorRedo();
    void editorPushUndoState();
    GameUtils::Editor::UndoRedoState& editorUndoTopOrDummy();
};

#endif