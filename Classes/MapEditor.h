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
#include "ui/CocosGUI.h"
#include "math/FastRNG.h"

#include "shared_scenes/custom_nodes/ui/include_ui.h"

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

class MapEditor : public ax::Scene
{
public:
    static ax::Scene* createScene();

    ~MapEditor();

#if CC_TARGET_PLATFORM == CC_PLATFORM_WIN32
    SIZE_T prevWorkingSetSize;
#endif

    f32 global_dt;
    f32 fps_dt;
    void update(f32 dt);
    f32 updateSchedTime;
    void perSecondUpdate(f32 dt);
    void lateUpdate(f32 dt);
    void editUpdate(ax::Vec2 old, ax::Vec2 place, ax::Size placeStampSize, ax::Size removeStampSize);
    void editUpdate_place(f32 x, f32 y, f32 _width, f32 _height);
    void editUpdate_remove(f32 x, f32 y, f32 _width, f32 _height);

    virtual bool init();
    void onInitDone(f32 dt);
    void draw(ax::Renderer* renderer, const ax::Mat4& transform, uint32_t flags);

    std::vector<ax::ui::Widget*> uiHitEvents;
    void updateUiHitEvent(ax::Vec2 point);

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
    
    TilesetArray* tilesetArr;

    //ax::Vec2 parseVector2D(std::string position);
    //ax::backend::ProgramState* createGPUProgram(std::string resources_frag_shader_path, std::string resources_vertex_shader_path);
    //ax::Vec2 convertFromScreenToSpace(ax::Vec2 LocationInView, ax::Size& visibleSize, bool reverseY);
    //ax::Vec2 convertFromSpaceToTileSpace(ax::Vec2 LocationInSpace);
    //ax::Vec2 convertFromTileSpaceToSpace(ax::Vec2 LocationInTileSpace);
    //ax::Vec2 convertFromSpaceToChunkSpace(ax::Vec2 LocationInSpace);
    //ax::Vec2 convertFromChunkSpaceToSpace(ax::Vec2 LocationInChunkSpace);
    //void resetEditorChunkCache();
    //bool addChunkIfNotExists(ax::Vec2 LocationInChunkSpace, i32 tileGID);
    //bool addTileIfNotExists(ax::Vec2 LocationInTileSpace, i32 tileGID = 0, i32 rot = 0, std::string hex = "FFFFFFFF");
    //bool removeTileIfNotExists(ax::Vec2 LocationInTileSpace);
    //std::string createCSVFromChunk(Chunk* chunk);
    void updateDirectorToStatsCount(i32 tileCount, i32 chunkCount);
    //bool unloadChunk(Chunk* ref);
    //void reorderChunks();
    void menuCloseCallback(ax::Ref* pSender);
    void buildEntireUi();
    CREATE_FUNC(MapEditor);

    void setUiTextDefaultShade(ax::ui::Text* text_node, bool use_shadow = true);

    FastRNG rng;

    sqlite3* pdb;

    std::vector<float> vertices;
    ax::MeshRenderer* renderer;
    TileTexCoords coord{ { 0,0 }, { 1,0 }, { 0,1 }, { 1,1 } };

    GameUtils::TileSystem::Map* map;

    TileArray* tarr;

    ax::Sprite*                map_sprite_stamp;
    ax::Texture2D*             map_default_tex;
    ax::backend::ProgramState* map_default_tile_shader;
    ax::BlendFunc*             map_default_tile_shader_blend;

    ax::Size visibleSize;
    //ZOrder* orderSystem;
    ax::LayerColor* bg;
    ax::Node* grid;
    //ax::DrawNode* chunkGrid;
    ax::DrawNode* deltaEditing;
    ax::Vec2 mouseLocation;
    ax::Vec2 oldMouseLocation;
    ax::Vec2 newMouseLocation;
    ax::Vec2 oldMouseLocationOnUpdate;
    ax::Vec2 newMouseLocationOnUpdate;
    ax::Vec2 mouseLocationDelta;
    ax::Vec2 mouseLocationInView;
    ax::Node* cameraLocation;
    ax::Vec2 oldSelectionPlace;
    ax::Vec2 selectionPlace;
    ax::Vec2 chunkSelectionPlace;
    f32 cameraScale;
    i32 cameraScaleIndex = 5;
    //f32 possibleCameraScales[29] = { 0.05F, 0.1F, 0.2F, 0.3F, 0.4F, 0.6F, 0.8F, 1, 1.5F, 2, 3, 4, 5, 6, 7, 8, 9, 10, 12, 14, 18, 22, 26, 32, 48, 64, 80, 96, 100};
    f32 possibleCameraScales[16] = { 34, 36, 38, 40, 42, 44, 46, 48, 50, 52, 54, 1, 58, 60, 70, 85 };

    ax::DrawNode* selectionPlaceSquare;
    ax::DrawNode* selectionPlaceSquareForbidden;
    ax::DrawNode* worldCoordsLines;
    ax::DrawNode* cameraCenterIndicator;

    ChangeValueBool* gridHideValue;
    ChangeValueFloat* gridOpacityValue;

    bool isInitDone = false;
    bool isTouchNew = false;

    ax::Node* rebuildableUiNodes;
    void rebuildEntireUi();
    bool isUiObstructing = false;
    bool isEditorDragging = false;
    bool isEditorHideGrid = false;
    bool isLocationEditable = false;
    bool isPlacing = false;
    ax::Rect createSelection(ax::Vec2 start_pos, ax::Vec2 end_pos, i32 _tileSize, SelectionBox::Box& box);
    ax::Rect createRemoveToolTileSelectionBox(ax::Vec2 start_pos, ax::Vec2 end_pos, i32 _tileSize);
    ax::Vec2 removeSelectionStartPos;
    ax::DrawNode* removeSelectionNode;
    bool isRemoving = false;

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

    ax::Node* chunkNode;
    ax::Node* worldNode;

    ax::MotionStreak* streak;

    ax::Node* statsParentNode;
    ax::ui::Text* FPSUiText;
    ax::ui::Text* VertsUiText;
    ax::ui::Text* BatchesUiText;
    ax::ui::Text* ChunkUiText;

    void set_cameraScaleUiText(f32 scale);
    void setWorldBoundsLayerColorTransforms(Camera* cam);
    ax::Node* cameraScaleUi;
    ax::Node* cameraScaleUiAlphaSpriteCascade;
    ax::ui::Text* cameraScaleUiText;
    ax::Sprite* cameraScaleUiSpNormal;
    ax::Sprite* cameraScaleUiSpSmall;
    ax::Sprite* cameraScaleUiSpBig;
    ax::ui::Button* cameraScaleResetButton;

    // Custom Tile Spawn Code
    i32 cur = 0;
};

#endif