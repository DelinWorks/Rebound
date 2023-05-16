#include "MapEditor.h"

USING_NS_CC;
using namespace backend;

using namespace GameUtils;

void MapEditor::updateDirectorToStatsCount(i32 tileCount, i32 chunkCount)
{
}

Scene* MapEditor::createScene()
{
    return MapEditor::create();
}

MapEditor::~MapEditor()
{
    RLOGE(true, "sqlite3_close result: {}", sqlite3_close(pdb));
    LOG_RELEASE;
}

#define GRID_COLOR Color4F::BLACK
#define LAYER_BACKGROUND_COLOR Color4B(40, 47, 54, 255)
#define LAYER_BACKGROUND_BOUND_COLOR Color4B(30, 37, 44, 255)
#define LINE_BACKGROUND_BOUND_COLOR Color4F(0.7, 0.7, 0.7, 1)
#define SELECTION_SQUARE_ALLOWED Color4F(0, 0.58f, 1.0f, 0.8f)
#define SELECTION_SQUARE_DENIED Color4F(1, 0.19f, 0.19f, 0.8f)
#define SELECTION_SQUARE_TRI_ALLOWED Color4F(0, 0.58f, 1.0f, 0.08f)
#define SELECTION_SQUARE_TRI_DENIED Color4F(1, 0.19f, 0.19f, 0.08f)

bool MapEditor::init()
{
    if (!Scene::init())
    {
        return false;
    }

    REGISTER_SCENE(MapEditor);

#ifdef WIN32
    PROCESS_MEMORY_COUNTERS_EX pmc;
    GetProcessMemoryInfo(GetCurrentProcess(), (PROCESS_MEMORY_COUNTERS*)&pmc, sizeof(pmc));
    prevWorkingSetSize = pmc.PrivateUsage;
#endif

    cameraLocation = Node::create();

    ax::Device::setKeepScreenOn(true);

    visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();

    uiNode = Node::create();
    rebuildableUiNodes = Node::create();
    uiNodeNonFollow = Node::create();
    uiNodeNonFollow->addComponent((new UiRescaleComponent(visibleSize))
        ->enableDesignScaleIgnoring());
    gridNode = Node::create();
    addChild(cameraLocation);
    addChild(uiNode, 17);
    uiNode->addChild(rebuildableUiNodes);
    SET_POSITION_HALF_SCREEN(uiNode);
    _worlds[0]->addChild(uiNodeNonFollow, 17);
    _worlds[0]->addChild(gridNode, 11);

    //uiNode->addComponent(new FollowNodeTransformComponent(getDefaultCamera()));

    cameraScale = 1;

    //tileSizePure = 16;
    //tileSize = tileSizePure * contentScale;

    //chunkSize = -1;
    //if (tileSizePure == 8)
    //    chunkSize = 8 * CHUNK_SIZE_MULTIPLIER * contentScale;
    //if (tileSizePure == 16)
    //    chunkSize = 16 * CHUNK_SIZE_MULTIPLIER * contentScale;
    //if (tileSizePure == 32)
    //    chunkSize = 32 * CHUNK_SIZE_MULTIPLIER * contentScale;
    //if (tileSizePure == 64)
    //    chunkSize = 64 * CHUNK_SIZE_MULTIPLIER * contentScale;

    //mapSizeX = 500;
    //mapSizeY = 200;

    //mapSizeX = snap(mapSizeX, chunkSize / tileSize);
    //mapSizeY = snap(mapSizeY, chunkSize / tileSize);

    //_defaultCamera->setBackgroundBrush(ax::CameraBackgroundBrush::createColorBrush(Color4F::RED, 0));

    VirtualWorldManager::resizeRenderTextures(this);

    TileSystem::tileMapVirtualCamera = _camera;
    map = TileSystem::Map::create(Vec2(16, 16), 1, Vec2(1000000, 1000000));
    _worlds[1]->addChild(map, 10);

    grid = Node::create();
    auto gridDN = DrawNode::create(1);
    for (i32 i = -(map->_gridSize / map->_tileSize.x) * 8; i <= +(map->_gridSize / map->_tileSize.x) * 8; i++)
    {
        gridDN->drawLine(Vec2(i * map->_tileSize.x, -map->_gridSize / 2 * 8), Vec2(i * map->_tileSize.x, +map->_gridSize / 2 * 8), GRID_COLOR);
    }
    for (i32 i = -(map->_gridSize / 2 / map->_tileSize.y) * 8; i <= +(map->_gridSize / 2 / map->_tileSize.y) * 8; i++)
    {
        gridDN->drawLine(Vec2(-map->_gridSize * 8, i * map->_tileSize.y), Vec2(+map->_gridSize * 8, i * map->_tileSize.y), GRID_COLOR);
    }
    gridDN->setPosition(0, 0);
    grid->addChild(gridDN);
    gridNode->addChild(grid, 1);

    TopMapSizeNode = Node::create();
    TopMapSizeNode->setPosition(Vec2(0, map->_mapSize.y * map->_tileSize.y));
    auto TopMapSize = ax::LayerColor::create(LAYER_BACKGROUND_BOUND_COLOR);
    TopMapSize->setPosition(Vec2(visibleSize.width / -2, 0));
    TopMapSizeNode->addChild(TopMapSize);
    gridNode->addChild(TopMapSizeNode, 2);

    BottomMapSizeNode = Node::create();
    BottomMapSizeNode->setPosition(Vec2(0, -map->_mapSize.y * map->_tileSize.y));
    auto BottomMapSize = ax::LayerColor::create(LAYER_BACKGROUND_BOUND_COLOR);
    BottomMapSize->setPosition(Vec2(visibleSize.width / -2, -visibleSize.height));
    BottomMapSizeNode->addChild(BottomMapSize);
    gridNode->addChild(BottomMapSizeNode, 2);

    RightMapSizeNode = Node::create();
    RightMapSizeNode->setPosition(Vec2(map->_mapSize.x * map->_tileSize.x, 0));
    auto RightMapSize = ax::LayerColor::create(LAYER_BACKGROUND_BOUND_COLOR);
    RightMapSize->setPosition(Vec2(0, visibleSize.height / -2));
    RightMapSizeNode->addChild(RightMapSize);
    gridNode->addChild(RightMapSizeNode, 2);

    LeftMapSizeNode = Node::create();
    LeftMapSizeNode->setPosition(Vec2(-map->_mapSize.x * map->_tileSize.x, 0));
    auto LeftMapSize = ax::LayerColor::create(LAYER_BACKGROUND_BOUND_COLOR);
    LeftMapSize->setPosition(Vec2(-visibleSize.width, visibleSize.height / -2));
    LeftMapSizeNode->addChild(LeftMapSize);
    gridNode->addChild(LeftMapSizeNode, 2);

    WorldBoundsLimit = DrawNode::create(1.0F);
    WorldBoundsLimit->drawLine(Vec2(-map->_mapSize.x * map->_tileSize.x, map->_mapSize.y * map->_tileSize.y), Vec2(map->_mapSize.x * map->_tileSize.x, map->_mapSize.y * map->_tileSize.y), LINE_BACKGROUND_BOUND_COLOR);
    WorldBoundsLimit->drawLine(Vec2(-map->_mapSize.x * map->_tileSize.x, map->_mapSize.y * map->_tileSize.y), Vec2(-map->_mapSize.x * map->_tileSize.x, -map->_mapSize.y * map->_tileSize.y), LINE_BACKGROUND_BOUND_COLOR);
    WorldBoundsLimit->drawLine(Vec2(-map->_mapSize.x * map->_tileSize.x, -map->_mapSize.y * map->_tileSize.y), Vec2(map->_mapSize.x * map->_tileSize.x, -map->_mapSize.y * map->_tileSize.y), LINE_BACKGROUND_BOUND_COLOR);
    WorldBoundsLimit->drawLine(Vec2(map->_mapSize.x * map->_tileSize.x, -map->_mapSize.y * map->_tileSize.y), Vec2(map->_mapSize.x * map->_tileSize.x, map->_mapSize.y * map->_tileSize.y), LINE_BACKGROUND_BOUND_COLOR);
    gridNode->addChild(WorldBoundsLimit, 2);

    deltaEditing = DrawNode::create(100);
    _worlds[0]->addChild(deltaEditing);

    selectionPlaceSquare = DrawNode::create(1);
    selectionPlaceSquare->drawTriangle(Vec2(0, 0), Vec2(0, map->_tileSize.y), Vec2(map->_tileSize.x, 0), SELECTION_SQUARE_TRI_ALLOWED);
    selectionPlaceSquare->drawTriangle(Vec2(map->_tileSize.x, map->_tileSize.y), Vec2(map->_tileSize.x, 0), Vec2(0, map->_tileSize.y), SELECTION_SQUARE_TRI_ALLOWED);
    selectionPlaceSquare->drawLine(Vec2(0, 0), Vec2(map->_tileSize.x, 0), SELECTION_SQUARE_ALLOWED);
    selectionPlaceSquare->drawLine(Vec2(0, 0), Vec2(0, map->_tileSize.y), SELECTION_SQUARE_ALLOWED);
    selectionPlaceSquare->drawLine(Vec2(map->_tileSize.x, 0), Vec2(map->_tileSize.x, map->_tileSize.y), SELECTION_SQUARE_ALLOWED);
    selectionPlaceSquare->drawLine(Vec2(map->_tileSize.x, map->_tileSize.y), Vec2(0, map->_tileSize.y), SELECTION_SQUARE_ALLOWED);
    selectionPlaceSquare->setAnchorPoint(Point(0.5, 0.5));
    gridNode->addChild(selectionPlaceSquare, 3);

    selectionPlaceSquareForbidden = DrawNode::create(1);
    selectionPlaceSquareForbidden->drawTriangle(Vec2(0, 0), Vec2(0, map->_tileSize.y), Vec2(map->_tileSize.x, 0), SELECTION_SQUARE_TRI_DENIED);
    selectionPlaceSquareForbidden->drawTriangle(Vec2(map->_tileSize.x, map->_tileSize.y), Vec2(map->_tileSize.x, 0), Vec2(0, map->_tileSize.y), SELECTION_SQUARE_TRI_DENIED);
    selectionPlaceSquareForbidden->drawLine(Vec2(0, 0), Vec2(map->_tileSize.x, 0), SELECTION_SQUARE_DENIED);
    selectionPlaceSquareForbidden->drawLine(Vec2(0, 0), Vec2(0, map->_tileSize.y), SELECTION_SQUARE_DENIED);
    selectionPlaceSquareForbidden->drawLine(Vec2(map->_tileSize.x, 0), Vec2(map->_tileSize.x, map->_tileSize.y), SELECTION_SQUARE_DENIED);
    selectionPlaceSquareForbidden->drawLine(Vec2(map->_tileSize.x, map->_tileSize.y), Vec2(0, map->_tileSize.y), SELECTION_SQUARE_DENIED);
    selectionPlaceSquareForbidden->setAnchorPoint(Point(0.5, 0.5));
    gridNode->addChild(selectionPlaceSquareForbidden, 3);

    worldCoordsLines = DrawNode::create(1);
    worldCoordsLines->drawLine(Vec2(0, -map->_mapSize.x * map->_tileSize.x), Vec2(0, 0), Color4F(0, 0.5, 0, 1));
    worldCoordsLines->drawLine(Vec2(0, 0), Vec2(0, map->_mapSize.x * map->_tileSize.x), Color4F(0, 1, 0, 1));
    worldCoordsLines->drawLine(Vec2(-map->_mapSize.x * map->_tileSize.x, 0), Vec2(0, 0), Color4F(0.5, 0, 0, 1));
    worldCoordsLines->drawLine(Vec2(0, 0), Vec2(map->_mapSize.x * map->_tileSize.x, 0), Color4F(1, 0, 0, 1));
    worldCoordsLines->setAnchorPoint(Point(0.5, 0.5));
    worldCoordsLines->setOpacity(100);
    gridNode->addChild(worldCoordsLines, 1);

    _input->_oldMouseLocation = Vec2(0, 0);
    _input->_newMouseLocation = Vec2(0, 0);

    //streak = MotionStreak::create(0.1, 1, 8, Color3B::WHITE, "streak.png");
    //uiNode->addChild(streak);

    return true;
}

void MapEditor::onInitDone(f32 dt)
{
    if (!isInitDone)
    {
#ifdef WIN32
        glfwRequestWindowAttention(Darkness::getInstance()->gameWindow.window);
#endif

        //i32 result = SQLITE_OK;
        //dbPath = ax::FileUtils::getInstance()->getWritablePath() + dbName;
        ////std::ifstream f(dbPath);
        //switch (loadStep) {
        ////case 0:
        ////    loadingString->setString("Finding file " + dbName);
        ////    if (!f) result = SQLITE_NOMEM;
        ////    else result = SQLITE_OK;
        ////    errorInCase = "File not found in " + ax::FileUtils::getInstance()->getWritablePath();
        ////    if (result == SQLITE_OK)
        ////        loadStep++;
        ////    else loadStep = 99;
        ////    return;
        //case 1:
        //{
        //    str statString = "Opening file " + dbName;
        //    if (loadingString->getString().compare(statString) != 0)
        //    {
        //        loadingString->setString(statString);
        //        errorInCase = "Failed to open sqlite object: " + dbPath;
        //        return;
        //    }
        //    pdb = NULL;
        //    result = result SQLITE_RESULT_CHECK sqlite3_open(dbPath.c_str(), &pdb) : result;
        //    str secret = zlibString::decompress_string(Strings::from_base64("eNpzys02dktxNsryLfUoKC4OMwkPMzEtDbasCKwwS/Qt8AIAr4sKtw=="));
        //    result = result SQLITE_RESULT_CHECK sqlite3_key(pdb, secret.c_str(), secret.length()) : result;

        //    if (result == SQLITE_OK)
        //        loadStep++;
        //    else loadStep = 99;
        //    return;
        //}
        //case 2:
        //{
        //    str statString = "Running VACUUM on " + dbName;
        //    if (loadingString->getString().compare(statString) != 0)
        //    {
        //        loadingString->setString(statString);
        //        errorInCase = "Failed to run VACUUM command on sqlite object: " + std::string(sqlite3_errmsg(pdb));
        //        return;
        //    }
        //    result = result SQLITE_RESULT_CHECK sqlite3_exec(pdb, "VACUUM;", NULL, NULL, NULL) : result;
        //    result = result SQLITE_RESULT_CHECK sqlite3_exec(pdb, "PRAGMA auto_vacuum=1;", NULL, NULL, NULL) : result;
        //    result = result SQLITE_RESULT_CHECK sqlite3_exec(pdb, "PRAGMA journal_mode=WAL;", NULL, NULL, NULL) : result;
        //    result = result SQLITE_RESULT_CHECK sqlite3_exec(pdb, "PRAGMA synchronous=NORMAL;", NULL, NULL, NULL) : result;
        //    if (result == SQLITE_OK)
        //        loadStep++;
        //    else loadStep = 99;
        //    return;
        //}
        //case 3:
        //{
        //    std::string statString = "Creating MapChunkDatas on " + dbName;
        //    if (loadingString->getString().compare(statString) != 0)
        //    {
        //        loadingString->setString(statString);
        //        errorInCase = "Failed to create table MapChunkDatas on sqlite object: " + std::string(sqlite3_errmsg(pdb));
        //        return;
        //    }
        //    result = result SQLITE_RESULT_CHECK sqlite3_exec(pdb, "CREATE TABLE IF NOT EXISTS MapChunkDatas (position_in_chunk_space TEXT UNIQUE, texture_atlas_id TINYi32 UNSIGNED, chunk_data_binary TEXT);", NULL, NULL, NULL) : result;
        //    result = result SQLITE_RESULT_CHECK sqlite3_exec(pdb, "CREATE UNIQUE INDEX IF NOT EXISTS idx_position_in_chunk_space ON MapChunkDatas(position_in_chunk_space);", NULL, NULL, NULL) : result;
        //    result = result SQLITE_RESULT_CHECK sqlite3_exec(pdb, "CREATE INDEX IF NOT EXISTS idx_texture_atlas_id        ON MapChunkDatas(texture_atlas_id);", NULL, NULL, NULL) : result;
        //    result = result SQLITE_RESULT_CHECK sqlite3_exec(pdb, "CREATE INDEX IF NOT EXISTS idx_chunk_data_binary       ON MapChunkDatas(chunk_data_binary);", NULL, NULL, NULL) : result;
        //    if (result == SQLITE_OK)
        //        loadStep++;
        //    else loadStep = 99;
        //    return;
        //}
        //case 4:
        //{
        //    str statString = "Creating EditorMetaData on " + dbName;
        //    if (loadingString->getString().compare(statString) != 0)
        //    {
        //        loadingString->setString(statString);
        //        errorInCase = "Failed to create table EditorMetaData on sqlite object: " + std::string(sqlite3_errmsg(pdb));
        //        return;
        //    }
        //    sqlite3_stmt* stmt;
        //    result = result SQLITE_RESULT_CHECK sqlite3_prepare_v2(pdb, "SELECT COUNT(name) FROM sqlite_master WHERE type='table' AND name='EditorMetaData';", -1, &stmt, NULL) : result;
        //    i32 isTableExists = 0;
        //    if (result == SQLITE_OK)
        //    {
        //        i32 stmt_result = 0;
        //        while (true)
        //        {
        //            stmt_result = sqlite3_step(stmt);
        //            if (stmt_result == SQLITE_ROW)
        //            {
        //                isTableExists = sqlite3_column_int(stmt, 0);
        //            }
        //            else break;
        //        }
        //        sqlite3_finalize(stmt);
        //    }
        //    result = result SQLITE_RESULT_CHECK sqlite3_exec(pdb, "CREATE TABLE IF NOT EXISTS EditorMetaData (store_key TEXT UNIQUE, store_value TEXT);", NULL, NULL, NULL) : result;
        //    if (isTableExists == 0)
        //    {
        //        result = result SQLITE_RESULT_CHECK sqlite3_exec(pdb, "INSERT INTO EditorMetaData VALUES ('editor_camera_position_vec2', '[0,0]');", NULL, NULL, NULL) : result;
        //        result = result SQLITE_RESULT_CHECK sqlite3_exec(pdb, "INSERT INTO EditorMetaData VALUES ('editor_camera_scale_index', '0');", NULL, NULL, NULL) : result;

        //        //for (size_t i = 0; i < 999; i++)
        //        //{
        //        //    char stmt1Buff[128];
        //        //    sprintf(stmt1Buff, "INSERT INTO EditorMetaData VALUES ('%d', '[0,0]');", i);
        //        //    result = result SQLITE_RESULT_CHECK sqlite3_exec(pdb, stmt1Buff, NULL, NULL, NULL) : result;
        //        //}
        //    }

        //    if (result == SQLITE_OK)
        //        loadStep++;
        //    else loadStep = 99;
        //    return;
        //}
        //case 5:
        //{
        //    str statString = "Loading " + dbName + " Chunks";
        //    if (loadingString->getString().compare(statString) != 0)
        //    {
        //        loadingString->setString(statString);
        //        errorInCase = "Failed to select from table MapChunkDatas: " + std::string(sqlite3_errmsg(pdb));
        //        return;
        //    }
        //    {
        //        sqlite3_stmt* stmt;
        //        result = sqlite3_prepare_v2(pdb, "SELECT * FROM MapChunkDatas", -1, &stmt, NULL);
        //        if (result == SQLITE_OK)
        //        {
        //            i32 result = 0;
        //            while (true)
        //            {
        //                result = sqlite3_step(stmt);
        //                if (result == SQLITE_ROW)
        //                {
        //                    //std::cout << sqlite3_column_text(stmt, 2) << "\n";
        //                }
        //                else break;
        //            }
        //            result = sqlite3_finalize(stmt);
        //        }
        //    }
        //    if (result == SQLITE_OK)
        //        loadStep++;
        //    else loadStep = 99;
        //    return;
        //}
        //case 6:
        //{
        //    str statString = "Reading value tileSize";
        //    if (loadingString->getString().compare(statString) != 0)
        //    {
        //        loadingString->setString(statString);
        //        errorInCase = "tileSize can only be: 8, 16, 32, 64";
        //        return;
        //    }
        //    if (map->_chunkSize == -1)
        //    {
        //        loadStep = 99;
        //        return;
        //    }
        //    loadStep++;
        //    return;
        //}
        //case 7:
        //{
        //    str statString = "Reading value contentScale";
        //    if (loadingString->getString().compare(statString) != 0)
        //    {
        //        loadingString->setString(statString);
        //        errorInCase = "contentScale can only be: 1, 2";
        //        return;
        //    }
        //    if (map->content_scale != 1 && map->content_scale != 2)
        //    {
        //        loadStep = 99;
        //        return;
        //    }
        //    loadStep++;
        //    return;
        //}
        //case 99:
        //    loadingString->setString(TEXT(loadingString->getString()) + " Failed!\nerror: " + errorInCase);
        //    loadingString->setTextColor(Color4B(237, 67, 78, 255));
        //    loadStep = 100;
        //    return;
        //case 100:
        //    return;
        //default:
        //    loadStep = -1;
        //}
        //buildEntireUi();
        //uiNode->removeChild(loadingLayout);

        //result = SQLITE_OK;
        //sqlite3_stmt* stmt;
        //result = result SQLITE_RESULT_CHECK sqlite3_prepare_v2(pdb, "SELECT store_value FROM EditorMetaData WHERE store_key='editor_camera_position_vec2'", -1, &stmt, NULL) : result;
        //if (result == SQLITE_OK)
        //{
        //    result = result SQLITE_RESULT_CHECK sqlite3_step(stmt) : result;
        //    if (result == SQLITE_ROW)
        //    {
        //        auto output = parseVector2D(std::string(RCAST<CCCP>(sqlite3_column_text(stmt, 0))));
        //        //cameraLocation->setPosition(output);
        //        result = result SQLITE_RESULT_CHECK sqlite3_finalize(stmt) : result;
        //    }
        //}
        //sqlite3_finalize(stmt);

        //cameraScaleIndex = 10;

        //result = SQLITE_OK;
        //result = result SQLITE_RESULT_CHECK sqlite3_prepare_v2(pdb, "SELECT store_value FROM EditorMetaData WHERE store_key='editor_camera_scale_index'", -1, &stmt, NULL) : result;
        //if (result == SQLITE_OK)
        //{
        //    result = result SQLITE_RESULT_CHECK sqlite3_step(stmt) : result;
        //    if (result == SQLITE_ROW)
        //    {
        //        auto output = std::stoi(std::string(RCAST<CCCP>(sqlite3_column_text(stmt, 0))));

        //        if (output != 0)
        //        {
        //            cameraScaleIndex = output;
        //            setCameraScaleIndex();
        //        }
        //        result = result SQLITE_RESULT_CHECK sqlite3_finalize(stmt) : result;
        //    }
        //}
        //sqlite3_finalize(stmt);

        //bg = ax::LayerColor::create(LAYER_BACKGROUND_COLOR);
        //bg->setAnchorPoint(Vec2(0.5F, 0.5F));
        ////bg->addComponent((new UiRescaleComponent(visibleSize))->enableLayerResizing());
        //_world->addChild(bg, -1);

        //std::cout << sqlite3_close(pdb) << std::endl;
        //pdb = nullptr;

        //set_cameraScaleUiText(std::numeric_limits<F32>::max());

        //std::vector<TileID> Rtiles = { 1, 1024 + 25 };

        //TileID*tiles = (TileID*)malloc(CHUNK_BUFFER_SIZE * sizeof(TileID));

        tilesetArr = TilesetArray::create({ 16, 16 });

        auto texture1 = Director::getInstance()->getTextureCache()->addImage("maps/level1/textures/atlas_002.png");
        auto texture2 = Director::getInstance()->getTextureCache()->addImage("maps/level1/textures/atlas_001.png");

        tilesetArr->addTileset(texture1);
        tilesetArr->addTileset(texture2);
        //auto i = new Image();
        //i->initWithImageFile("maps/level1/textures/atlas_002.png");

        map->setTilesetArray(tilesetArr);

        map->addLayer("background");
        map->addLayer("collision");
        map->addLayer("decoration");
        map->bindLayer(0);

        //tilesetArr->addTileset(texture1);
        //tilesetArr->addTileset(texture2);

        //tilesetArr->calculateBounds();

        //for (int i = 0; i < CHUNK_BUFFER_SIZE; i++) {
        //    tiles[i] = Rtiles[Random::maxInt(Rtiles.size() - 1)];

        //    if (Random::float01() > 0.5)
        //        tiles[i] |= TILE_FLAG_ROTATE;
        //    if (Random::float01() > 0.5)
        //        tiles[i] |= TILE_FLAG_FLIP_X;
        //    if (Random::float01() > 0.5)
        //        tiles[i] |= TILE_FLAG_FLIP_Y;
        //}

        //tarr = TileArray::create(tiles);

        //ChunkFactory::buildVertexCache(tarr, tilesetArr);

        //ChunkDescriptor d{};
        //d._tiles = tarr;
        //d._tilesetArr = tilesetArr;

        //for (int x = 0; x < 200; x++)
        //    for (int y = 0; y < 200; y++) {
        //        auto chunk = ChunkRenderer::create(d);
        //        addChild(chunk);
        //        chunk->setPositionInChunkSpace(x, y);
        //    }


        //for (int i = 0; i < CHUNK_BUFFER_SIZE; i++) {
        //    tiles[i] = Rtiles[Random::maxInt(Rtiles.size() - 1)];
        //}

        //auto texture = Director::getInstance()->getTextureCache()->addImage("maps/level1/textures/atlas_002.png");

        //BENCHMARK_SECTION_BEGIN("build tiled mesh");
        //for (int x = 0; x < 20; x++)
        //    for (int y = 0; y < 20; y++) {
        //        auto mesh = ChunkFactory::buildTiledMesh(tiles, vertices, { 16, 16 }, ax::Vec2(texture->getPixelsWide(), texture->getPixelsHigh()));
        //        renderer = ChunkFactory::createChunkMeshRenderer(texture, mesh);
        //        renderer->setPosition(mesh._sizeInPixels.x * x, mesh._sizeInPixels.y * y);
        //        addChild(renderer);
        //    }
        //BENCHMARK_SECTION_END();

        auto color = Color3B(0, 255, 65);
        auto hsv = HSV(color);
        hsv.h += 180;
        color = hsv.toColor3B();

        RLOG("hsv: {},{},{}", color.r, color.g, color.b);

        buildEntireUi();

        editorUndoRedoMax(100); // save 100 undo redo states maximum

        isInitDone = true;
    }
    else {
        if (updateSchedTime < 1.0f)
            updateSchedTime += dt;
        else
        {
            perSecondUpdate(dt);
            updateSchedTime = 0;
        }
    }
}

void MapEditor::perSecondUpdate(f32 dt)
{
    //std::vector<TileID> Rtiles = { 0 };

    //for (int i = 0; i < CHUNK_BUFFER_SIZE; i++) {
    //    if (Random::float01() > 0.9) {
    //        TileID newGid = Rtiles[Random::maxInt(Rtiles.size() - 1)];

    //        if (Random::float01() > 0.5)
    //            newGid |= TILE_FLAG_ROTATE;
    //        if (Random::float01() > 0.5)
    //            newGid |= TILE_FLAG_FLIP_X;
    //        if (Random::float01() > 0.5)
    //            newGid |= TILE_FLAG_FLIP_Y;

    //        ChunkFactory::setTile(tarr, i, newGid);
    //    }
    //}

    //coord.ccw();

    //for (i16 i = 0; i < 1024; i++)
    //{
    //    int startIdx = i;
    //    startIdx *= 36;

    //    //vertices[(3 + startIdx) + 9 * i] = 1.0f;
    //    //vertices[(4 + startIdx) + 9 * i] = 0.0f;
    //    //vertices[(5 + startIdx) + 9 * i] = 0.0f;
    //    //vertices[(6 + startIdx) + 9 * i] = 1.0f;

    //    vertices[(7 + startIdx) + 9 * 0] = coord.tl.U;
    //    vertices[(8 + startIdx) + 9 * 0] = coord.tl.V;

    //    vertices[(7 + startIdx) + 9 * 1] = coord.tr.U;
    //    vertices[(8 + startIdx) + 9 * 1] = coord.tr.V;

    //    vertices[(7 + startIdx) + 9 * 2] = coord.bl.U;
    //    vertices[(8 + startIdx) + 9 * 2] = coord.bl.V;

    //    vertices[(7 + startIdx) + 9 * 3] = coord.br.U;
    //    vertices[(8 + startIdx) + 9 * 3] = coord.br.V;
    //}

    //BENCHMARK_SECTION_BEGIN("update gpu buffer");
    //TileMeshCreator::updateMeshVertexData(vertices, renderer->getMesh());
    //BENCHMARK_SECTION_END();
}

void MapEditor::update(f32 dt)
{
    updateDirectorToStatsCount(map->_tileCount, 0);
    if (getContainer()) getContainer()->hover(_input->_mouseLocationInViewNoScene, _defaultCamera);
}

void MapEditor::tick(f32 dt)
{
    REBUILD_UI;

    elapsedDt += dt;

    //TileSystem::zPositionMultiplier = 1.0 + sin(elapsedDt) * 0.1;

    //SET_UNIFORM(_rts[1]->getSprite()->getProgramState(), "u_time", float(elapsedDt * 0.1));
    //_rt->getSprite()->getTexture()->setAliasTexParameters();
    //_rts[1]->getSprite()->setSkewX(sin(elapsedDt * 6) * 10);
    //_rts[1]->getSprite()->setSkewY(cos(elapsedDt * 10) * 10);
    //_rt->getSprite()->setScale(1.25);

    //if (getContainer()) getContainer()->updateLayoutManagers(true);

    //ps->addParticles(1, -1, -1);
    //ps->addParticles(1, -1, 0);

    onInitDone(dt);
    if (!isInitDone)
        return;

    auto* focusState = &Darkness::getInstance()->gameWindow.focusState;
    if (*focusState) {
        map->reload();
        *focusState = false;
    }

    //float angle = MATH_RAD_TO_DEG(Vec2::angle(Vec2(1, 0), Vec2(1, 1)));

    //streak->setPosition(Vec2(_input->_mouseLocation.x - (visibleSize.x / 2), (_input->_mouseLocation.y + (visibleSize.y / -2)) * -1));

    //for (auto& i : findNodesByTag(this, 91))
    //    ((ParticleSystemQuad*)i)->setEmissionShape(0, ParticleSystem::createCircleShape({ 0,0 }, pos.x * 2));

    //ps->setPosition(convertFromScreenToSpace(mouseLocation, visibleSize, getDefaultCamera(), true));

    global_dt = dt;

    //auto sound = FMODAudioEngine::getInstance()->getSoundChannel("sound1");
    //if (sound != nullptr)
    //{
    //    unsigned i32 pos;
    //    sound->getPosition(&pos, FMOD_TIMEUNIT_MS);
    //    std::cout << pos << std::endl;
    //}

    //std::cout << this->getChildrenCount() << "\n";

    setWorldBoundsLayerColorTransforms(_camera);

    cameraLocation->setPosition(Vec2(
        clamp(cameraLocation->getPositionX(), (f32)(map->_mapSize.x * map->_tileSize.x) * -1, (f32)map->_mapSize.x * map->_tileSize.x),
        clamp(cameraLocation->getPositionY(), (f32)(map->_mapSize.y * map->_tileSize.x) * -1, (f32)map->_mapSize.y * map->_tileSize.x)));

    _camera->setPosition(cameraLocation->getPosition());

    Vec2 pos = convertFromScreenToSpace(_input->_mouseLocationInView, _camera);

    _input->_oldMouseLocationOnUpdate = _input->_newMouseLocationOnUpdate;
    _input->_newMouseLocationOnUpdate = _input->_mouseLocation;
    for (const auto i : uiNodeNonFollow->getChildren())
        i->setScale(_camera->getScale());
    Vec2 clampedChunkSelectionPlaceToCamera = Vec2(snap(cameraLocation->getPositionX() - map->_chunkSize / 2, map->_chunkSize * 10), snap(cameraLocation->getPositionY() - map->_chunkSize / 2, map->_chunkSize * 10));
    grid->setPosition(clampedChunkSelectionPlaceToCamera.x, clampedChunkSelectionPlaceToCamera.y);
    //std::cout << convertFromSpaceToChunkSpace(selectionPlace).x << ", " << convertFromSpaceToChunkSpace(selectionPlace).y << "\n";
    selectionPlaceSquare->setPosition(selectionPlace);
    selectionPlaceSquareForbidden->setPosition(selectionPlace);
    if (selectionPlace.x + map->_tileSize.x / 2 < -map->_mapSize.x * map->_tileSize.x || selectionPlace.x + map->_tileSize.x / 2 > map->_mapSize.x * map->_tileSize.x ||
        selectionPlace.y + map->_tileSize.y / 2 < -map->_mapSize.y * map->_tileSize.y || selectionPlace.y + map->_tileSize.y / 2 > map->_mapSize.y * map->_tileSize.y)
    {
        isLocationEditable = false;
        selectionPlaceSquare->setVisible(false);
        selectionPlaceSquareForbidden->setVisible(true);
    }
    else
    {
        isLocationEditable = true;
        selectionPlaceSquare->setVisible(true);
        selectionPlaceSquareForbidden->setVisible(false);
    }

    if (isRemoving)
    {
        selectionPlaceSquare->setVisible(false);
        selectionPlaceSquareForbidden->setVisible(false);
    }

    oldSelectionPlace = selectionPlace;
    selectionPlace = Vec2(snap(pos.x - map->_tileSize.x * 1.5f, map->_tileSize.x) + map->_tileSize.x, snap(pos.y - map->_tileSize.y * 1.5f, map->_tileSize.y) + map->_tileSize.y);
    chunkSelectionPlace = Vec2(snap(pos.x - map->_chunkSize / 2, map->_chunkSize), snap(pos.y - map->_chunkSize / 2, map->_chunkSize));

    lateUpdate(dt);
}

void MapEditor::lateUpdate(f32 dt)
{
    for (auto& i : grid->getChildren()) {
        if (cameraScale < 5)
        {
            i->setOpacity(60);
            i->setScale(1);
            worldCoordsLines->setOpacity(100);
        }
        else if (cameraScale >= 5 && cameraScale < 13)
        {
            i->setOpacity(20);
            i->setScale(10);
            worldCoordsLines->setOpacity(60);
        }
    }
    //deltaEditing->clear();
    //deltaEditing->setLineWidth(1);
    //deltaEditing->drawLine(convertFromScreenToSpace(oldMouseLocationOnUpdate, visibleSize, true), convertFromScreenToSpace(newMouseLocationOnUpdate, visibleSize, true), Color4F::GREEN);
    //cameraCenterIndicator->setPosition(_defaultCamera->getPosition());
    //cameraCenterIndicator->setScaleX(_defaultCamera->getScaleX());
    //cameraCenterIndicator->setScaleY(_defaultCamera->getScaleY());
    Size place    = Size();
    place.width   = 10;
    place.height  = 10;
    Size remove   = Size();
    remove.width  = 1;
    remove.height = 1;
    editUpdate(oldSelectionPlace, selectionPlace, place, remove);
    Vec2 loc = convertFromScreenToSpace(visibleSize, _camera);
    Vec2 loc0 = convertFromScreenToSpace(Vec2::ZERO, _camera);
}

// DON'T CALL THIS MANUALLY
void MapEditor::editUpdate_place(f32 _x, f32 _y, f32 _width, f32 _height) {
    std::vector v = { 1 };
    BENCHMARK_SECTION_BEGIN("Tile placement test");
    auto& undoCmd = editorUndoTopOrDummy();
    undoCmd.action = Editor::UNDOREDO_TILEMAP;
    undoCmd.affected.map = map;
    for (int x = _x; x < _width; x++)
        for (int y = _y; y < _height; y++) {
            TileID gid = v[Random::maxInt(v.size() - 1)];
            if (Random::float01() > 0.5)
                gid |= TILE_FLAG_ROTATE;
            if (Random::float01() > 0.5)
                gid |= TILE_FLAG_FLIP_X;
            if (Random::float01() > 0.5)
                gid |= TILE_FLAG_FLIP_Y;
            undoCmd.affected.addOrIgnoreTilePrev({ float(x), float(y) }, map->getTileAt({ float(x), float(y) }));
            map->setTileAt({ float(x), float(y) }, gid);
            undoCmd.affected.addOrIgnoreTileNext({ float(x), float(y) }, gid);
        }
    BENCHMARK_SECTION_END();
}

// DON'T CALL THIS MANUALLY
void MapEditor::editUpdate_remove(f32 _x, f32 _y, f32 _width, f32 _height) {
    _x = _x / map->_tileSize.x;
    _y = _y / map->_tileSize.y;
    _width = _width / map->_tileSize.x;
    _height = _height / map->_tileSize.y;

    for (int x = _x; x < _width; x++)
        for (int y = _y; y < _height; y++)
            map->setTileAt(Vec2(x, y), 0);
}

void MapEditor::editUpdate(Vec2& old, Vec2& place, Size& placeStampSize, Size& removeStampSize)
{
    if (isRemoving)
        createEditToolSelectionBox(removeSelectionStartPos, convertFromScreenToSpace(_input->_mouseLocationInView, _camera, false), map->_tileSize.x);

    if (isPlacing || isRemoving)
    {
        f32 _oldX = old.x, _newX = place.x;
        f32 _oldY = old.y, _newY = place.y;
        f32 vX, vY, evalX, evalY, finalEval;
        evalX = _oldX - _newX;
        evalY = _oldY - _newY;
        f32 _width = 0, height = 0;
        finalEval = abs((abs(evalX) - _width) / map->_tileSize.x > (abs(evalY) - height) / map->_tileSize.y ? (abs(evalX) - _width) / map->_tileSize.x : (abs(evalY) - height) / map->_tileSize.y);
        for (f32 i = 0l; i < 1.0l; i += 1.0l / finalEval)
        {
            vX = _oldX - _newX;
            vX = vX * i;
            vX += _newX;
            vY = _oldY - _newY;
            vY = vY * i;
            vY += _newY;
            if (isPlacing)
            {
                vX = round(vX / map->_tileSize.x);
                vY = round(vY / map->_tileSize.y);
                //auto rect = createEditToolSelectionBox(Vec2(vX, vY), Vec2(placeStampSize.width, placeStampSize.height), map->_tileSize.x);
                editUpdate_place(vX, vY, vX + 1, vY + 1);
                // break the loop cuz we dont want the stamp to be lined with frame lag
                // cuz that shit steals a TON of frames just skip it if its over 10 units on any axis
                if (placeStampSize.width > 10 || placeStampSize.height > 10)
                    break;
                continue;
            }
            //if (isRemoving)
            //{
            //    editUpdate_remove(snap(vX, tileSize), snap(vY, tileSize), removeStampSize.width, removeStampSize.height);
            //    // Same Here but for removing Tiles
            //    if (removeStampSize.width > 10 || removeStampSize.height > 10)
            //        break;
            //    continue;
            //}
        }
    }


    //if (isPlacing) {
    //    for (i32 x = -tileSize * 10; x < tileSize * 10; x += tileSize)
    //        for (i32 y = -tileSize * 10; y < tileSize * 10; y += tileSize)
    //            addTileIfNotExists(convertFromSpaceToTileSpace(Vec2(place.x + x, place.y + y)));
    //}

    //if (isRemoving) {
    //    for (i32 x = -tileSize * 10; x < tileSize * 10; x += tileSize)
    //        for (i32 y = -tileSize * 10; y < tileSize * 10; y += tileSize)
    //            removeTileIfNotExists(convertFromSpaceToTileSpace(Vec2(place.x + x, place.y + y)));
    //}
}

void MapEditor::onKeyHold(ax::EventKeyboard::KeyCode keyCode, ax::Event* event)
{
}

void MapEditor::onKeyPressed(EventKeyboard::KeyCode keyCode, Event* event)
{
    if (keyCode == EventKeyboard::KeyCode::KEY_1) map->bindLayer(0);
    if (keyCode == EventKeyboard::KeyCode::KEY_2) map->bindLayer(1);
    if (keyCode == EventKeyboard::KeyCode::KEY_3) map->bindLayer(2);

    if (keyCode == EventKeyboard::KeyCode::KEY_LEFT_CTRL) { isCtrlPressed = true; }

    if (keyCode == EventKeyboard::KeyCode::KEY_Z) editorUndo();

    if (keyCode == EventKeyboard::KeyCode::KEY_Y) editorRedo();

    if (keyCode == EventKeyboard::KeyCode::KEY_LEFT_ALT)
    {
        isEditorDragging = true;
    }

    if (keyCode == EventKeyboard::KeyCode::KEY_H)
        grid->setVisible(!grid->isVisible());

    if (keyCode == EventKeyboard::KeyCode::KEY_G) {
        auto action = ActionFloat::create(0.25, zPositionMultiplier, zPositionMultiplier < 0.5 ? 1 : 0,
            [=](float value) { zPositionMultiplier = tweenfunc::backEaseInOut(value); });
        runAction(action);
    }

    if (keyCode == EventKeyboard::KeyCode::KEY_S)
    {
        for (int i = 0; i < 1; i++) {
            auto panel = CustomUi::DiscardPanel::create();
            panel->init(L"WARNING", L"User Interface Test !!!", CustomUi::DiscardButtons::YES_NO, CustomUi::DiscardType::INPUT);
            getContainer()->pushModal(panel);
        }

        //std::terminate();
        //sqlite3_exec(pdb, std::string("DELETE FROM MapChunkDatas").c_str(), NULL, NULL, NULL);
        //for (const auto& x : *map->layer_groups)
        //    for (const auto& y : *x->layers)
        //        for (const auto& i : *y->chunks_map) {
        //            std::string dat = Strings::to_base64(zlibString::compress_string(map->createCSVFromChunk(i.second)));
        //            std::string pos = std::string("[") + std::to_string(i.second->x) + "," + std::to_string(i.second->y) + std::string("]");
        //            std::string gid = std::to_string(i.second->textureGID);
        //            i32 result = 0;
        //            result = result SQLITE_RESULT_CHECK sqlite3_exec(pdb, std::string("INSERT OR IGNORE INTO MapChunkDatas VALUES ('" + pos + "'," + gid + ",\"\");").c_str(), NULL, NULL, NULL) : result;
        //            result = result SQLITE_RESULT_CHECK sqlite3_exec(pdb, std::string("UPDATE MapChunkDatas SET chunk_data_binary='" + dat + "' WHERE position_in_chunk_space='" + pos + "'").c_str(), NULL, NULL, NULL) : result;
        //            SQLITE_CRASH_CHECK(result);
        //        }
        //sqlite3_wal_checkpoint(pdb, NULL);
    }

#ifdef WIN32
    if (keyCode == EventKeyboard::KeyCode::KEY_T)
    {
        auto s = getSingleFileDialog(glfwGetWin32Window(Darkness::getInstance()->gameWindow.window));
        RLOG("Texture Path: {}", Strings::narrow(s));
    }
#endif
}

void MapEditor::onKeyReleased(EventKeyboard::KeyCode keyCode, Event* event)
{
    if (keyCode == EventKeyboard::KeyCode::KEY_LEFT_CTRL) isCtrlPressed = false;

    if (keyCode == EventKeyboard::KeyCode::KEY_LEFT_ALT) isEditorDragging = false;
}

void MapEditor::onMouseDown(ax::Event* event)
{
    EventMouse* e = (EventMouse*)event;

    if (e->getMouseButton() == EventMouse::MouseButton::BUTTON_4)
    {
        isEditorDragging = true;
    }

    if (e->getMouseButton() == EventMouse::MouseButton::BUTTON_LEFT)
    {
        editorPushUndoState();
        isPlacing = true;
        auto mouseClick = DrawNode::create(1);
        mouseClick->setPosition(Vec2(_input->_mouseLocation.x - (visibleSize.x / 2), (_input->_mouseLocation.y + (visibleSize.y / -2)) * -1));
        mouseClick->addComponent(new DrawNodeCircleExpandComponent(.5, 80, 16));
        DESTROY(mouseClick, .5);
        uiNode->addChild(mouseClick, 999);
    }
    if (e->getMouseButton() == EventMouse::MouseButton::BUTTON_RIGHT)
    {
        editorPushUndoState();
        isRemoving = true;
        removeSelectionStartPos = convertFromScreenToSpace(_input->_mouseLocation, _camera, true);
    }
}

void MapEditor::onMouseUp(ax::Event* event)
{
    EventMouse* e = (EventMouse*)event;

    if (e->getMouseButton() == EventMouse::MouseButton::BUTTON_4)
    {
        isEditorDragging = false;
    }
    if (e->getMouseButton() == EventMouse::MouseButton::BUTTON_LEFT)
    {
        if (isPlacing) {
            isPlacing = false;
        }
    }
    if (e->getMouseButton() == EventMouse::MouseButton::BUTTON_RIGHT)
    {
        if (!isRemoving) return;
        isRemoving = false;
        Rect rect = createEditToolSelectionBox(removeSelectionStartPos, convertFromScreenToSpace(_input->_mouseLocation, _camera, true), map->_tileSize.x);
        editUpdate_place(rect.origin.x, rect.origin.y, rect.size.width, rect.size.height);
        RLOG("remove_selection_tool: begin: {},{} end: {},{}", rect.origin.x, rect.origin.y, rect.size.width, rect.size.height);
        removeSelectionNode->clear();
    }
}

void MapEditor::onMouseMove(ax::Event* event)
{
    if (!hasMouseMoved) { hasMouseMoved = true; return; }
    if (isEditorDragging)
    {
        cameraLocation->setPositionX(cameraLocation->getPositionX() + (_input->_mouseLocationDelta.x * _camera->getScale()));
        cameraLocation->setPositionY(cameraLocation->getPositionY() + (_input->_mouseLocationDelta.y * -1 * _camera->getScale()));
    }
    //CCLOG("%f,%f", cameraLocation->getPositionX(), cameraLocation->getPositionY());
}

void MapEditor::setCameraScaleIndex(i32 dir) {
    cameraScaleIndex += dir;
    i32 n = (sizeof(possibleCameraScales) / sizeof(possibleCameraScales[0])) - 1;
    cameraScaleIndex = (int)clamp(cameraScaleIndex, 0, n);
    f32 preCamScl = cameraScale;
    cameraScale = possibleCameraScales[cameraScaleIndex];
    if (cameraScale != 1.0F) {
        cameraScale = tweenfunc::quadraticIn(tweenfunc::quadraticIn(tweenfunc::quadraticIn(cameraScale / 100)));
        cameraScale *= 100;
    }
    if (cameraScale < 1)
        cameraScale = snap_interval(cameraScale, 1, 500);
    else if (cameraScale < 3)
        cameraScale = snap_interval(cameraScale, 1, 100);
    else if (cameraScale < 10)
        cameraScale = snap_interval(cameraScale, 1, 10);
    else if (cameraScale < 50)
        cameraScale = snap_interval(cameraScale, 1, 5);
    else
        cameraScale = snap_interval(cameraScale, 1, 1);
    f32 postCamScl = cameraScale;

    Vec2 targetPos = convertFromScreenToSpace(_input->_mouseLocationInView, _camera);
    Vec2 pos = cameraLocation->getPosition();
    Vec2 newPos = pos.lerp(targetPos, 1.0F - (postCamScl / preCamScl));
    cameraLocation->runAction(Sequence::create(MoveTo::create(0, Vec2(newPos.x, newPos.y)), NULL));
    _camera->setZoom(cameraScale);
    setWorldBoundsLayerColorTransforms(_camera);
    setCameraScaleUiText(cameraScale);
}

void MapEditor::onMouseScroll(ax::Event* event)
{
    EventMouse* e = (EventMouse*)event;

    setCameraScaleIndex(e->getScrollY());

    if (pdb != nullptr)
    {
        i32 result = 0;
        result = result SQLITE_RESULT_CHECK sqlite3_exec(pdb, std::string("UPDATE EditorMetaData SET store_value='" + std::to_string(cameraScaleIndex) + "' WHERE store_key='editor_camera_scale_index';").c_str(), NULL, NULL, NULL) : result;
        SQLITE_CRASH_CHECK(result);
    }
}

bool MapEditor::onTouchBegan(ax::Touch* touch, ax::Event* event)
{
    isEditorDragging = true;
    isTouchNew = true;
    return true;
}

void MapEditor::onTouchMoved(ax::Touch* touch, ax::Event* event)
{
    //mouseLocation.x = touch->getLocation().x;
    //mouseLocation.y = touch->getLocation().y * -1;
    //oldMouseLocation = newMouseLocation;
    //newMouseLocation = mouseLocation;
    //if (isTouchNew)
    //{
    //    oldMouseLocation = newMouseLocation;
    //    isTouchNew = false;
    //}
    //mouseLocationDelta = oldMouseLocation - newMouseLocation;
    //if (isEditorDragging)
    //{
    //    cameraLocation->setPositionX(cameraLocation->getPositionX() + (mouseLocationDelta.x * _camera->getScale()));
    //    cameraLocation->setPositionX(cameraLocation->getPositionY() + (mouseLocationDelta.y * -1 * _camera->getScale()));
    //}
    //_input->_mouseLocationInView = touch->getLocation();
}

void MapEditor::onTouchEnded(ax::Touch* touch, ax::Event* event)
{
    isEditorDragging = true;
    cameraLocation->setPosition(_camera->getPosition());
}

void MapEditor::onTouchCancelled(ax::Touch* touch, ax::Event* event)
{
}

void MapEditor::visit(Renderer* renderer, const Mat4& parentTransform, uint32_t parentFlags)
{
    // we are updating in the visit function because axmol
    // calls visit before update which makes the game have
    // a one frame delay which can be frustration.
    tick(_director->getDeltaTime());
    VirtualWorldManager::renderAllPasses(this, Color4F(LAYER_BACKGROUND_COLOR));
    Scene::visit(renderer, parentTransform, parentFlags);
}

void MapEditor::menuCloseCallback(Ref* pSender)
{
    //Close the ax-x game scene and quit the application
    Director::getInstance()->end();

    /*To navigate back to native iOS screen(if present) without quitting the application  ,do not use Director::getInstance()->end() as given above,instead trigger a custom event created in RootViewController.mm as below*/

    //EventCustom customEndEvent("game_scene_close_event");
    //_eventDispatcher->dispatchEvent(&customEndEvent);


}

void MapEditor::buildEntireUi()
{
    CustomUi::callbackAccess.clear();

    auto container = _input->_uiContainer = CustomUi::Container::create();
    container->setStatic();
    container->setContentSize(visibleSize);
    container->setBorderLayoutAnchor();
    uiNode->addChild(container);
    CustomUi::callbackAccess.emplace("main", container);

    auto test = CustomUi::Slider::create();
    test->init({ 128, 16 });
    container->addChild(test);

    auto topRightContainer = CustomUi::Container::create();
    topRightContainer->setBorderLayout(BorderLayout::TOP_LEFT, BorderContext::PARENT);
    topRightContainer->setLayout(CustomUi::FlowLayout(CustomUi::SORT_VERTICAL, CustomUi::STACK_CENTER, 0, 0, true));
    topRightContainer->setBorderLayoutAnchor();
    topRightContainer->setBackgroundSpriteCramped(ax::Vec2::ZERO, { -1, -1 });
    container->addChild(topRightContainer);

    auto menuContainer = CustomUi::Container::create();
    menuContainer->setLayout(CustomUi::FlowLayout(CustomUi::SORT_HORIZONTAL, CustomUi::STACK_CENTER, 0, 0, false));
    //menuContainer->setBorderLayoutAnchor(BorderLayout::RIGHT);
    menuContainer->setTag(CONTAINER_FLOW_TAG);
    topRightContainer->addChild(menuContainer);

    auto padding = Size(2, 10);
    auto hpadding = Size(3, 20);

    auto fileB = CustomUi::Button::create();
    fileB->init(L"File", 16, ax::Vec2::ZERO, hpadding);
    fileB->setUiPadding(padding);
    menuContainer->addChild(fileB);

    auto editB = CustomUi::Button::create();
    editB->init(L"Edit", 16, ax::Vec2::ZERO, hpadding);
    editB->setUiPadding(padding);
    menuContainer->addChild(editB);

    editB->_callback = [=](CustomUi::Button* target) {
        auto fcontainer = CustomUi::Container::create();
        auto vis = Director::getInstance()->getVisibleSize();
        fcontainer->setBorderLayoutAnchor(TOP_LEFT);
        fcontainer->setConstraint(CustomUi::DependencyConstraint(target, TOP_RIGHT, { 0, 0.1 }, true, vis / -2));
        fcontainer->setBackgroundSprite();

        auto lb = CustomUi::Label::create();
        lb->init(L"Edit Menu Created!", 16);
        lb->setUiPadding({ 20, 10 });
        fcontainer->addChild(lb);

        fcontainer->runAction(Sequence::create(
            DelayTime::create(1),
            FadeOut::create(1),
            CallFunc::create([=]() { fcontainer->removeFromParent(); }),
            nullptr
        ));

        container->addChild(fcontainer);
    };

    auto settingsB = CustomUi::Button::create();
    settingsB->init(L"Menu", 16, ax::Vec2::ZERO, hpadding);
    settingsB->setUiPadding(padding);
    menuContainer->addChild(settingsB);

    settingsB->_callback = [=](CustomUi::Button* target) {
        auto fcontainer = CustomUi::Container::create();
        auto vis = Director::getInstance()->getVisibleSize();
        fcontainer->setBorderLayoutAnchor(TOP_LEFT);
        fcontainer->setConstraint(CustomUi::DependencyConstraint(target, TOP_RIGHT, { 0, 0.1 }, true, vis / -2));
        fcontainer->setBackgroundSprite();

        auto lb = CustomUi::Label::create();
        lb->init(L"Settings Menu Created!", 16);
        lb->setUiPadding({ 20, 10 });
        fcontainer->addChild(lb);

        fcontainer->runAction(Sequence::create(
            DelayTime::create(1),
            FadeOut::create(1),
            CallFunc::create([=]() { fcontainer->removeFromParent(); }),
            nullptr
        ));

        container->addChild(fcontainer);
    };

    auto editContainer = CustomUi::Container::create();
    //editContainer->setBorderLayoutAnchor(BorderLayout::RIGHT);
    editContainer->setLayout(CustomUi::FlowLayout(CustomUi::SORT_HORIZONTAL, CustomUi::STACK_CENTER, 0, 0, false));
    editContainer->setTag(CONTAINER_FLOW_TAG);
    editContainer->setMargin({ 0, 1 });
    CustomUi::callbackAccess.emplace("edit_container", editContainer);

    padding = { 52, 5 };

    auto undoB = CustomUi::Button::create();
    undoB->initIcon("editor_undo");
    undoB->setUiPadding(padding);
    editContainer->addChild(undoB);

    auto extContainer = CustomUi::Container::create();

    CustomUi::callbackAccess.emplace("ext_edit_container", extContainer);

    fileB->_callback = [=](CustomUi::Button* target) {
        auto fcontainer = CustomUi::Container::create();
        auto vis = Director::getInstance()->getVisibleSize();
        fcontainer->setBorderLayoutAnchor(TOP_LEFT);
        fcontainer->setConstraint(CustomUi::DependencyConstraint(CustomUi::callbackAccess["ext_edit_container"],
            BOTTOM_LEFT, { -0.55, 0.55 }, true, vis / -2));
        fcontainer->setLayout(CustomUi::FlowLayout(CustomUi::SORT_VERTICAL, CustomUi::STACK_CENTER));
        fcontainer->setMargin({ 0, 10 });
        fcontainer->setBackgroundSprite();
        fcontainer->setBlocking();
        fcontainer->setDismissible();

        auto lb = CustomUi::Button::create();
        lb->init(L"-- Resources ------", 16);
        lb->disable();
        lb->setUiPadding({ 10, 5 });
        fcontainer->addChild(lb);

        lb = CustomUi::Button::create();
        lb->init(L"New Map", 16);
        lb->setUiPadding({ 10, 5 });
        fcontainer->addChild(lb);

        lb = CustomUi::Button::create();
        lb->init(L"Open Map", 16);
        lb->setUiPadding({ 10, 5 });
        fcontainer->addChild(lb);

        lb = CustomUi::Button::create();
        lb->init(L"Import Texture", 16);
        lb->setUiPadding({ 10, 5 });
        fcontainer->addChild(lb);

        lb = CustomUi::Button::create();
        lb->init(L"Reload Textures", 16);
        lb->setUiPadding({ 10, 5 });
        fcontainer->addChild(lb);

        lb = CustomUi::Button::create();
        lb->init(L"-- Changes --------", 16);
        lb->disable();
        lb->setUiPadding({ 10, 5 });
        fcontainer->addChild(lb);

        lb = CustomUi::Button::create();
        lb->init(L"Save", 16);
        lb->setUiPadding({ 10, 5 });
        fcontainer->addChild(lb);

        lb = CustomUi::Button::create();
        lb->init(L"Save and Exit", 16);
        lb->setUiPadding({ 10, 5 });
        fcontainer->addChild(lb);

        lb = CustomUi::Button::create();
        lb->init(L"Exit without Saving", 16);
        lb->setUiPadding({ 10, 5 });
        fcontainer->addChild(lb);

        lb = CustomUi::Button::create();
        lb->init(L"-- Other ----------", 16);
        lb->disable();
        lb->setUiPadding({ 10, 5 });
        fcontainer->addChild(lb);

        lb = CustomUi::Button::create();
        lb->init(L"More Options...", 16);
        lb->setUiPadding({ 10, 5 });
        fcontainer->addChild(lb);

        lb->_callback = [=](CustomUi::Button* target) {
            fcontainer->removeFromParent();
            auto fcontainer1 = CustomUi::Container::create();
            auto vis = Director::getInstance()->getVisibleSize();
            fcontainer1->setBorderLayoutAnchor(TOP_LEFT);
            fcontainer1->setConstraint(CustomUi::DependencyConstraint(CustomUi::callbackAccess["ext_edit_container"],
                BOTTOM_LEFT, { -0.55, 0.55 }, true, vis / -2));
            fcontainer1->setLayout(CustomUi::FlowLayout(CustomUi::SORT_VERTICAL, CustomUi::STACK_CENTER));
            fcontainer1->setMargin({ 0, 10 });
            fcontainer1->setBackgroundSprite();
            fcontainer1->setBlocking();
            fcontainer1->setDismissible();

            auto lb = CustomUi::Button::create();
            lb->init(L"-- Resources --------------------------", 16);
            lb->disable();
            lb->setUiPadding({ 10, 5 });
            fcontainer1->addChild(lb);

            lb = CustomUi::Button::create();
            lb->init(L"Import .TTF/.OTF Font File", 16);
            lb->setUiPadding({ 10, 5 });
            fcontainer1->addChild(lb);

            lb = CustomUi::Button::create();
            lb->init(L"-- Passes -----------------------------", 16);
            lb->disable();
            lb->setUiPadding({ 10, 5 });
            fcontainer1->addChild(lb);

            lb = CustomUi::Button::create();
            lb->init(L"Open Render Pass Editor", 16);
            lb->setUiPadding({ 10, 5 });
            fcontainer1->addChild(lb);

            lb = CustomUi::Button::create();
            lb->init(L"-- TileMaps ---------------------------", 16);
            lb->disable();
            lb->setUiPadding({ 10, 5 });
            fcontainer1->addChild(lb);

            lb = CustomUi::Button::create();
            lb->init(L"Switch to Bilinear Rendering", 16);
            lb->setUiPadding({ 10, 5 });
            fcontainer1->addChild(lb);

            lb = CustomUi::Button::create();
            lb->init(L"-- Shaders ----------------------------", 16);
            lb->disable();
            lb->setUiPadding({ 10, 5 });
            fcontainer1->addChild(lb);

            lb = CustomUi::Button::create();
            lb->init(L"Import GLSL Shader", 16);
            lb->setUiPadding({ 10, 5 });
            fcontainer1->addChild(lb);

            lb = CustomUi::Button::create();
            lb->init(L"Compile GLSL v3.0 Compliant Shader Code", 16);
            lb->setUiPadding({ 10, 5 });
            fcontainer1->addChild(lb);

            lb = CustomUi::Button::create();
            lb->init(L"-- Native -----------------------------", 16);
            lb->disable();
            lb->setUiPadding({ 10, 5 });
            fcontainer1->addChild(lb);

            lb = CustomUi::Button::create();
            lb->init(L"SQLite Vacuum File", 16);
            lb->setUiPadding({ 10, 5 });
            fcontainer1->addChild(lb);

            CustomUi::Functions::menuContentFitButtons(fcontainer1);

            CustomUi::callbackAccess["main"]->addChild(fcontainer1);
        };

        CustomUi::Functions::menuContentFitButtons(fcontainer);

        CustomUi::callbackAccess["main"]->addChild(fcontainer);
    };

    auto redoB = CustomUi::Button::create();
    redoB->initIcon("editor_redo");
    redoB->setUiPadding(padding);
    editContainer->addChild(redoB);

    auto moveB = CustomUi::Button::create();
    moveB->initIcon("editor_move");
    moveB->setUiPadding(padding);
    editContainer->addChild(moveB);

    CONTAINER_MAKE_MINIMIZABLE(topRightContainer);

    auto vis = Director::getInstance()->getVisibleSize();
    extContainer->setBorderLayoutAnchor(TOP_LEFT);
    extContainer->setConstraint(CustomUi::DependencyConstraint(CustomUi::callbackAccess["edit_container"],
        BOTTOM_LEFT, { -0.02, 0 }));
    extContainer->setLayout(CustomUi::FlowLayout(CustomUi::SORT_VERTICAL, CustomUi::STACK_CENTER, 30));
    extContainer->setMargin({ 20, 10 });
    extContainer->setBackgroundSpriteCramped(ax::Vec2::ZERO, { -1, -1 });
    extContainer->setTag(GUI_ELEMENT_EXCLUDE);

    padding = Vec2(20, 8);

    auto placeB = CustomUi::Button::create();
    placeB->initIcon("editor_place", padding);
    extContainer->addChild(placeB);

    auto bucketB = CustomUi::Button::create();
    bucketB->initIcon("editor_bucket_fill", padding);
    extContainer->addChild(bucketB);

    auto removeB = CustomUi::Button::create();
    removeB->initIcon("editor_remove", padding);
    extContainer->addChild(removeB);

    auto selectB = CustomUi::Button::create();
    selectB->initIcon("editor_select", padding);
    extContainer->addChild(selectB);

    auto rectFillB = CustomUi::Button::create();
    rectFillB->initIcon("editor_rectangle_fill", padding);
    extContainer->addChild(rectFillB);

    editContainer->addChild(extContainer);

    topRightContainer->addChild(editContainer);

    auto cameraScaleContainer = CustomUi::Container::create();
    cameraScaleContainer->setBorderLayout(BorderLayout::TOP, BorderContext::PARENT);
    cameraScaleContainer->setLayout(CustomUi::FlowLayout(CustomUi::SORT_HORIZONTAL, CustomUi::STACK_RIGHT, 10, 0, false));
    cameraScaleContainer->setBorderLayoutAnchor();
    cameraScaleContainer->setMargin({ 0, 1 });
    container->addChild(cameraScaleContainer);

    cameraScaleB = CustomUi::Button::create();
    cameraScaleB->initIcon("editor_zoom_aligned");
    cameraScaleB->_callback = [&](CustomUi::Button* target) {
        if (cameraScale != 1)
        {
            i32 i = 0;
            i32 n = (sizeof(possibleCameraScales) / sizeof(possibleCameraScales[0])) - 1;
            while (i < n)
            {
                if (possibleCameraScales[i] == 1.0F)
                    break;
                i++;
            }
            cameraScaleIndex = i;
            cameraScale = possibleCameraScales[cameraScaleIndex];
            _camera->setZoom(cameraScale);
            setCameraScaleUiText(cameraScale);
        }
    };
    cameraScaleContainer->addChild(cameraScaleB);

    cameraScaleL = CustomUi::Label::create();
    cameraScaleL->init(L"", 16);
    cameraScaleL->enableOutline();
    cameraScaleContainer->addChild(cameraScaleL);

    rebuildableUiNodes->removeAllChildren();
    _debugText = CustomUi::Label::create();
    _debugText->init(L"", 16);
    _debugText->enableOutline();
    getContainer()->addChild(_debugText);
    ((UiRescaleComponent*)_debugText->getComponent("UiRescaleComponent"))->setVisibleSizeHints(-2, 5, -2);
    _debugText->setAnchorPoint(Vec2(-0.5, -0.5));
    /* FPS COUNTER CODE BODY */ {
        _debugText->stopAllActions();
        auto update_fps_action = CallFunc::create([&]() {
            wchar_t buff[14];
            wchar_t buffDt[14];
            fps_dt += (_director->getInstance()->getDeltaTime() - fps_dt) * 0.25f;
            _snwprintf(buff, sizeof(buff), L"%.1lf", 1.0F / fps_dt);
            _snwprintf(buffDt, sizeof(buffDt), L"%.1lf", fps_dt * 1000);
            std::wstring buffAsStdStr = buff;
            std::wstring buffAsStdStrDt = buffDt;
            i32 verts = (i32)Director::getInstance()->getRenderer()->getDrawnVertices();
            i32 batches = (i32)Director::getInstance()->getRenderer()->getDrawnBatches();
            std::wstring text = WFMT(L"T: %d | C: %d\n", 0, 0) + L"D3D11: " + buffAsStdStr + L" / " + buffAsStdStrDt + L"ms\n" +
                WFMT(L"%s: %d / ", L"Draw Calls", batches) + WFMT(L"%s: %d", L"Vertices Drawn", verts);
            _debugText->setString(text);
        });
        auto wait_fps_action = DelayTime::create(0.5f);
        auto make_seq = Sequence::create(update_fps_action, wait_fps_action, nullptr);
        auto seq_repeat_forever = RepeatForever::create(make_seq);
        _debugText->runAction(seq_repeat_forever);
    }

    rebuildEntireUi();
}

void MapEditor::rebuildEntireUi()
{
    SCENE_BUILD_UI;
}

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
    box.left.begin =   Vec2(start_pos.x, start_pos.y);
    box.left.end =     Vec2(start_pos.x, end_pos.y);
    box.bottom.begin = Vec2(start_pos.x, start_pos.y);
    box.bottom.end =   Vec2(end_pos.x, start_pos.y);
    box.right.begin =  Vec2(start_pos.x, end_pos.y);
    box.right.end =    Vec2(end_pos.x, end_pos.y);
    box.top.begin =    Vec2(end_pos.x, start_pos.y);
    box.top.end =      Vec2(end_pos.x, end_pos.y);
    box.first.p0 =     Vec2(start_pos.x, start_pos.y);
    box.first.p1 =     Vec2(start_pos.x, end_pos.y);
    box.first.p2 =     Vec2(end_pos.x, end_pos.y);
    box.second.p0 =    Vec2(end_pos.x, end_pos.y);
    box.second.p1 =    Vec2(end_pos.x, start_pos.y);
    box.second.p2 =    Vec2(start_pos.x, start_pos.y);
    return Rect(start_pos.x, start_pos.y, end_pos.x, end_pos.y);
}

Rect MapEditor::createEditToolSelectionBox(Vec2 start_pos, Vec2 end_pos, i32 _tileSize)
{
    if (removeSelectionNode == nullptr) {
        removeSelectionNode = DrawNode::create();
        gridNode->addChild(removeSelectionNode, 2);
    }
    SelectionBox::Box box = SelectionBox::Box();
    Rect rect = createSelection(start_pos, end_pos, _tileSize, box);

    rect.origin.x = rect.origin.x / map->_tileSize.x;
    rect.origin.y = rect.origin.y / map->_tileSize.y;
    rect.size.x = rect.size.x / map->_tileSize.x;
    rect.size.y = rect.size.y / map->_tileSize.y;

    removeSelectionNode->clear();
    removeSelectionNode->setLineWidth(1);
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

    cameraScaleB->enable();
    if (scale < 1.0)
        cameraScaleB->icon->setSpriteFrame("editor_zoomed_in");
    else if (scale > 1.0)
        cameraScaleB->icon->setSpriteFrame("editor_zoomed_out");
    else
    {
        cameraScaleB->icon->setSpriteFrame("editor_zoom_aligned");
        cameraScaleB->disable();
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

void MapEditor::editorUndoRedoMax(int m)
{
    _undo.set_capacity(m);
    _redo.set_capacity(m);
}

void MapEditor::editorUndo()
{
    if (_undo.size() > 0) {
        _redo.push(_undo.top());
        _undo.top().applyUndoState();
        _undo.pop();
    }
}

void MapEditor::editorRedo()
{
    if (_redo.size() > 0) {
        _undo.push(_redo.top());
        _redo.top().applyRedoState();
        _redo.pop();
    }
}

void MapEditor::editorPushUndoState() {
    _undo.push(Editor::UndoRedoState());
    _redo.reset();
}

GameUtils::Editor::UndoRedoState& MapEditor::editorUndoTopOrDummy()
{
    if (_undo.size() == 0)
        editorPushUndoState();
    return _undo.top();
}
