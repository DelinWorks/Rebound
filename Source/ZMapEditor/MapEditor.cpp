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
    //RLOGE(true, "sqlite3_close result: {}", sqlite3_close(pdb));
    LOG_RELEASE;
}

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
    _worlds[1]->addChild(gridNode, 11);

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
    map = TileSystem::Map::create(Vec2(16, 16), 2, Vec2(1024 * 16 * 4, 1024 * 16 * 4));
    _worlds[0]->addChild(map, 10);

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

    selectionNode = Node::create();
    gridNode->addChild(selectionNode, 3);

    selectionPlaceSquare = DrawNode::create(1);
    selectionPlaceSquare->drawTriangle(Vec2(0, 0), Vec2(0, map->_tileSize.y), Vec2(map->_tileSize.x, 0), SELECTION_SQUARE_TRI_ALLOWED);
    selectionPlaceSquare->drawTriangle(Vec2(map->_tileSize.x, map->_tileSize.y), Vec2(map->_tileSize.x, 0), Vec2(0, map->_tileSize.y), SELECTION_SQUARE_TRI_ALLOWED);
    selectionPlaceSquare->drawLine(Vec2(0, 0), Vec2(map->_tileSize.x, 0), SELECTION_SQUARE_ALLOWED);
    selectionPlaceSquare->drawLine(Vec2(0, 0), Vec2(0, map->_tileSize.y), SELECTION_SQUARE_ALLOWED);
    selectionPlaceSquare->drawLine(Vec2(map->_tileSize.x, 0), Vec2(map->_tileSize.x, map->_tileSize.y), SELECTION_SQUARE_ALLOWED);
    selectionPlaceSquare->drawLine(Vec2(map->_tileSize.x, map->_tileSize.y), Vec2(0, map->_tileSize.y), SELECTION_SQUARE_ALLOWED);
    selectionPlaceSquare->setAnchorPoint(Point(0.5, 0.5));
    selectionNode->addChild(selectionPlaceSquare);

    selectionPlaceSquareForbidden = DrawNode::create(1);
    selectionPlaceSquareForbidden->drawTriangle(Vec2(0, 0), Vec2(0, map->_tileSize.y), Vec2(map->_tileSize.x, 0), SELECTION_SQUARE_TRI_DENIED);
    selectionPlaceSquareForbidden->drawTriangle(Vec2(map->_tileSize.x, map->_tileSize.y), Vec2(map->_tileSize.x, 0), Vec2(0, map->_tileSize.y), SELECTION_SQUARE_TRI_DENIED);
    selectionPlaceSquareForbidden->drawLine(Vec2(0, 0), Vec2(map->_tileSize.x, 0), SELECTION_SQUARE_DENIED);
    selectionPlaceSquareForbidden->drawLine(Vec2(0, 0), Vec2(0, map->_tileSize.y), SELECTION_SQUARE_DENIED);
    selectionPlaceSquareForbidden->drawLine(Vec2(map->_tileSize.x, 0), Vec2(map->_tileSize.x, map->_tileSize.y), SELECTION_SQUARE_DENIED);
    selectionPlaceSquareForbidden->drawLine(Vec2(map->_tileSize.x, map->_tileSize.y), Vec2(0, map->_tileSize.y), SELECTION_SQUARE_DENIED);
    selectionPlaceSquareForbidden->setAnchorPoint(Point(0.5, 0.5));
    selectionNode->addChild(selectionPlaceSquareForbidden);

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

    channelMgr.getColor(0).color = Color4F(LAYER_BACKGROUND_COLOR);

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
        map->_layers[2]->setBlendFunc(BlendFunc::ALPHA_PREMULTIPLIED);
        map->bindLayer(0);

        //BENCHMARK_SECTION_BEGIN("Chunk Serialize Speed");
        //std::string data = FileUtils::getInstance()->getStringFromFile("C:/Users/turky/Desktop/new 1.txt"sv);
        //auto base64 = Strings::to_base64(zlibString::compress_string(data));
        //BENCHMARK_SECTION_END();

        //auto img = new Image();
        //img->initWithImageFile("C:/Users/turky/Pictures/Untitled1(Photo)(noise_scale)(Level1)(x6.000000).png");
        //int len = compressBound(img->getDataLen());
        //uint8_t* compressed = new uint8_t[len];
        //zlibString::compress_array(img->getData(), img->getDataLen(), compressed, &len);
        //std::string encoded = Strings::to_base64(compressed, len);

        //uint8_t* data = Strings::from_base64_arr(encoded, &len);
        //auto s = img->getHeight() * img->getWidth() * 4;
        //uint8_t* uncompressed = new uint8_t[s];
        //zlibString::decompress_array(data, len, uncompressed, s);
        //RLOG("");

        //delete[] compressed;
        //delete[] data;
        //delete[] uncompressed;
        //img->release();
        
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

        buildEntireUi();

        editorUndoRedoMax(0); // save 100 undo redo states maximum

        setCameraScaleIndex();

        //for (int i = 0; i < 8; i++) {
        //    testLb = SpriteBatchNode::create("textures/w.png");

        //    for (int i = 0; i < 8000; i++) {
        //        auto s = Sprite::create("textures/w.png");
        //        s->setPosition(Random::rangeFloat(-600, 600), Random::rangeFloat(-600, 600));
        //        s->retain();
        //        s->setUserData(testLb);
        //        s->setScaleX(.1);
        //        _sprites.push_back(s);
        //        testLb->addChild(s);
        //        _objectCount++;
        //    }

        //    _worlds[1]->addChild(testLb);
        //}

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
    if (getContainer()) {
        bool cond = getContainer()->hover(_input->_mouseLocationInViewNoScene, _defaultCamera);
        selectionNode->setVisible(!cond && !isEditorDragging && !isSelectableHovered);
    }
    isSelectableHoveredLastFrame = false;
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

    if (CUI::_pCurrentHoveredTooltipItem &&
        _hoverToolTipPointer != CUI::_pCurrentHoveredTooltipItem &&
        CUI::_pCurrentHoveredTooltipItem->hoverTooltip.length() > 0 &&
        _hoverToolTipTime > 1.0f) {
        auto& s = CUI::_pCurrentHoveredTooltipItem->hoverTooltip;
        _editorToolTip->showToolTip(s, UINT32_MAX);
        _hoverToolTipPointer = CUI::_pCurrentHoveredTooltipItem;
    }
    else if (!CUI::_pCurrentHoveredTooltipItem) {
        if (_hoverToolTipTime > 0.0f)
            _hoverToolTipTime -= dt;
        if (_hoverToolTipPointer != CUI::_pCurrentHoveredTooltipItem) {
            _editorToolTip->hideToolTip();
            _hoverToolTipPointer = CUI::_pCurrentHoveredTooltipItem;
        }
    }
    else if (CUI::_pCurrentHoveredTooltipItem && _hoverToolTipTime < 1.0f) _hoverToolTipTime += dt;

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

    auto WASDVec = Vec2(((editorWASDCamMoveRect.origin.x + editorWASDCamMoveRect.size.x) * _camera->getScale() * 500 * dt),
        ((editorWASDCamMoveRect.origin.y + editorWASDCamMoveRect.size.y) * _camera->getScale() * 500 * dt));

    cameraLocation->setPosition(cameraLocation->getPosition() + WASDVec);

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
    BENCHMARK_SECTION_BEGIN("LOOK UP BENCHMARK");
    if (selectionPlace == selectionPosition) return;
    std::vector v = { _tilesetPicker->selectedIndex + 1 };
    auto& undoCmd = editorTopUndoStateOrDefault();
    undoCmd.setAction(EditorToolbox::UNDOREDO_TILEMAP);
    undoCmd.affectedTiles.map = map;
    undoCmd.affectedTiles.layer_idx = map->_layerIdx;
    //undoCmd.affected.allocateBuckets();
    for (int x = _x; x < _width; x++)
        for (int y = _y; y < _height; y++) {
            TileID gid = v[Random::maxInt(v.size() - 1)] | editorTileCoords.state();
            undoCmd.affectedTiles.addOrIgnoreTilePrev({ float(x), float(y) }, map->getTileAt({ float(x), float(y) }));
            map->setTileAt({ float(x), float(y) }, gid);
            undoCmd.affectedTiles.addOrIgnoreTileNext({ float(x), float(y) }, gid);
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
        if (selectionPlace != selectionPosition)
            selectionPosition = selectionPlace;
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


void MapEditor::setCameraScaleIndex(i32 dir, bool shiftTransform) {
    cameraScaleIndex += dir;
    i32 n = (sizeof(possibleCameraScales) / sizeof(possibleCameraScales[0])) - 1;
    cameraScaleIndex = (int)clamp(cameraScaleIndex, 0, n);
    f32 preCamScl = cameraScale;
    cameraScale = possibleCameraScales[cameraScaleIndex];
    //if (cameraScale != 1.0F) {
    //    cameraScale = cameraScale);
    //    //cameraScale *= 100;
    //}
    ////if (cameraScale < 1)
    ////    cameraScale = snap_interval(cameraScale, 1, 500);
    ////else if (cameraScale < 3)
    ////    cameraScale = snap_interval(cameraScale, 1, 100);
    ////else if (cameraScale < 10)
    ////    cameraScale = snap_interval(cameraScale, 1, 10);
    ////else if (cameraScale < 50)
    ////    cameraScale = snap_interval(cameraScale, 1, 5);
    ////else
    ////    cameraScale = snap_interval(cameraScale, 1, 1);
    //f32 postCamScl = cameraScale;

    Vec2 targetPos = convertFromScreenToSpace(_input->_mouseLocationInView, _camera);
    Vec2 pos = cameraLocation->getPosition();
    Vec2 newPos = pos.lerp(targetPos, 1.0F - (cameraScale / preCamScl));
    if (shiftTransform)
        cameraLocation->setPosition(newPos.x, newPos.y);
    _camera->setZoom(cameraScale / map->_contentScale);
    setWorldBoundsLayerColorTransforms(_camera);
    setCameraScaleUiText(cameraScale);
}

void MapEditor::visit(Renderer* renderer, const Mat4& parentTransform, uint32_t parentFlags)
{
    // we are updating in the visit function because axmol
    // calls visit before update which makes the game have
    // a one frame delay which can be frustration.
    Node::update(0);

    //auto& pc = _camera->getPosition();
    //Rect view(visibleSize / -2 * _camera->getScale() + pc, visibleSize / 2 * _camera->getScale() + pc);

    //auto lastTime = std::chrono::high_resolution_clock::now();

    //auto dt = Director::getInstance()->getDeltaTime();
    //for (auto&& _ : _sprites) {
    //    auto p = _->getPosition();
    //    p = p.rotateByAngle(Vec2(0, 0), AX_DEGREES_TO_RADIANS(dt * 10));
    //    _->setPosition(p);

    //    //_->setRotation(_->getRotation() - dt * 10);

    //    auto& s = _->getContentSize();
    //    Rect range(s / -2 * MAX(_->getScaleX(), _->getScaleY()) + p, s / 2 * MAX(_->getScaleX(), _->getScaleY()) + p);

    //    bool visible = Math::rectIntersectsRectOffOrigin(view, range);

    //    if (_->getParent() && !visible) {
    //        auto now = std::chrono::high_resolution_clock::now();
    //        auto millis = std::chrono::duration_cast<std::chrono::milliseconds>(now - lastTime).count();

    //        if (millis < 15) {
    //            _->removeFromParent();
    //            _objectCount--;
    //        }
    //    }
    //    else if (!_->getParent() && visible) {
    //        auto batch = (SpriteBatchNode*)_->getUserData();
    //        batch->addChild(_);
    //        _objectCount++;
    //    }
    //}

    //RLOG("CURRENT OBJECT COUNT: {}", 0);

    tick(_director->getDeltaTime());
    VirtualWorldManager::renderAllPasses(this, channelMgr.getColor(0).color);

    Scene::visit(renderer, parentTransform, parentFlags);

    //Mat4 p = _camera->getUnscaledWorldSpaceMatrix().getInversed();
    //testLb->draw(renderer, p * trans, parentFlags);
}

void MapEditor::menuCloseCallback(Ref* pSender)
{
    //Close the ax-x game scene and quit the application
    Director::getInstance()->end();

    /*To navigate back to native iOS screen(if present) without quitting the application  ,do not use Director::getInstance()->end() as given above,instead trigger a custom event created in RootViewController.mm as below*/

    //EventCustom customEndEvent("game_scene_close_event");
    //_eventDispatcher->dispatchEvent(&customEndEvent);


}

void MapEditor::rebuildEntireUi()
{
    visibleSize = Director::getInstance()->getVisibleSize();
    getContainer()->setContentSize(visibleSize / 1, false);
    SCENE_BUILD_UI;
}
