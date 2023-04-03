#include "MapEditor.h"

using namespace GameUtils::Parser;
using namespace GameUtils::CocosExt;
using namespace GameUtils::CocosExt::CustomComponents;

USING_NS_CC;
using namespace backend;

void MapEditor::updateDirectorToStatsCount(i32 tileCount, i32 chunkCount)
{
    i32 verts = (i32)Director::getInstance()->getRenderer()->getDrawnVertices();
    i32 batches = (i32)Director::getInstance()->getRenderer()->getDrawnBatches();
    BatchesUiText ->setString(FMT("Draw Calls: %d", batches));
    VertsUiText   ->setString(FMT("Verts Drawn: %d", verts));
    ChunkUiText   ->setString(FMT("T: %d | C: %d", tileCount, chunkCount));
}

Scene* MapEditor::createScene()
{
    return MapEditor::create();
}

MapEditor::~MapEditor()
{
    std::cout << sqlite3_close(pdb) << std::endl;
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

    scheduleUpdate();
    ax::Device::setKeepScreenOn(true);

    visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();

    uiNode = Node::create();
    rebuildableUiNodes = Node::create();
    uiNodeNonFollow = Node::create();
    uiNodeNonFollow->addComponent((new CustomComponents::UiRescaleComponent(visibleSize))
        ->enableDesignScaleIgnoring());
    gridNode = Node::create();
    chunkNode = Node::create();
    worldNode = Node::create();
    addChild(cameraLocation);
    addChild(uiNode, 17);
    uiNode->addChild(rebuildableUiNodes);
    addChild(uiNodeNonFollow, 17);
    addChild(gridNode, 9);
    addChild(chunkNode, 7);
    addChild(worldNode, 8);

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

    map = TileSystem::Map::create(Vec2(16, 16), 1, Vec2(1000, 1000));
    addChild(map);

    grid = Node::create();
    auto gridDN = DrawNode::create(1);
    for (i32 i = -(map->_gridSize / map->_tileSize.x) * 7; i <= +(map->_gridSize / map->_tileSize.x) * 8; i++)
    {
        gridDN->drawLine(Vec2(i * map->_tileSize.x, -map->_gridSize / 2 * 7), Vec2(i * map->_tileSize.x, +map->_gridSize / 2 * 8), GRID_COLOR);
    }
    for (i32 i = -(map->_gridSize / 2 / map->_tileSize.y) * 7; i <= +(map->_gridSize / 2 / map->_tileSize.y) * 8; i++)
    {
        gridDN->drawLine(Vec2(-map->_gridSize * 7, i * map->_tileSize.y), Vec2(+map->_gridSize * 8, i * map->_tileSize.y), GRID_COLOR);
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
    this->addChild(deltaEditing);

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

    auto parent = Node::create();
    parent->setPositionX(visibleSize.width);

    uiNodeNonFollow->addChild(parent);

    streak = MotionStreak::create(0.1, 1, 8, Color3B::WHITE, "streak.png");
    uiNode->addChild(streak);

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

        bg = ax::LayerColor::create(LAYER_BACKGROUND_COLOR);
        bg->setAnchorPoint(Vec2(0.5F, 0.5F));
        bg->addComponent((new CustomComponents::UiRescaleComponent(visibleSize))->enableLayerResizing());
        addChild(bg, -1);

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

        auto container = _input->_uiContainer = CustomUi::Container::create();
        container->addComponent((new CustomComponents::UiRescaleComponent(visibleSize))
            ->enableDesignScaleIgnoring()->setBorderLayout(BorderLayout::TOP));
        uiNode->addChild(container);

        auto textField = CustomUi::TextField::create();
        textField->init("layer name", 18, {100, 20});
        container->addChild(textField);

        container->enable();

        buildEntireUi();

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
    updateDirectorToStatsCount(0, 0);
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
    REBUILD_UI;

    if (_input->_uiContainer) {
        _input->_uiContainer->update(dt);
        _input->_uiContainer->hover(_input->_mouseLocationInViewNoScene, _defaultCamera);
    }

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

    auto cam = _defaultCamera;

    //float angle = MATH_RAD_TO_DEG(Vec2::angle(Vec2(1, 0), Vec2(1, 1)));

    streak->setPosition(Vec2(_input->_mouseLocation.x - (visibleSize.x / 2), (_input->_mouseLocation.y + (visibleSize.y / -2)) * -1));

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

    setWorldBoundsLayerColorTransforms(_defaultCamera);

    cameraLocation->setPosition(Vec2(
        clamp(cameraLocation->getPositionX(), (f32)(map->_mapSize.x * map->_tileSize.x) * -1, (f32)map->_mapSize.x * map->_tileSize.x),
        clamp(cameraLocation->getPositionY(), (f32)(map->_mapSize.y * map->_tileSize.x) * -1, (f32)map->_mapSize.y * map->_tileSize.x)));

    _defaultCamera->setPosition(cameraLocation->getPosition());

    Vec2 pos = convertFromScreenToSpace(_input->_mouseLocationInView, visibleSize, cam);
    //updateDirectorToStatsCount(map->totalTiles(), map->totalChunks());

    uiNode->setPosition(cameraLocation->getPosition());
    uiNode->setScale(cam->getZoom());
    uiNode->setRotation(cam->getRotation());

    _input->_oldMouseLocationOnUpdate = _input->_newMouseLocationOnUpdate;
    _input->_newMouseLocationOnUpdate = _input->_mouseLocation;
    for (const auto i : uiNodeNonFollow->getChildren())
        i->setScale(cam->getZoom());
    Vec2 clampedChunkSelectionPlaceToCamera = Vec2(snap(cameraLocation->getPositionX() - map->_chunkSize / 2, map->_chunkSize), snap(cameraLocation->getPositionY() - map->_chunkSize / 2, map->_chunkSize));
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
    auto cam = _defaultCamera;

    bg->setPositionX(cameraLocation->getPositionX() - visibleSize.width / 2);
    bg->setPositionY(cameraLocation->getPositionY() - visibleSize.height / 2);
    bg->setScale(cam->getZoom());
    bg->setRotation(cam->getRotation());

    for (auto& i : grid->getChildren()) {
        if (cameraScale < 5)
        {
            i->setOpacity(60);
            worldCoordsLines->setOpacity(100);
        }
        else if (cameraScale >= 5 && cameraScale < 13)
        {
            i->setOpacity(20);
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
    place.width   = 100;
    place.height  = 100;
    Size remove   = Size();
    remove.width  = 1;
    remove.height = 1;
    editUpdate(oldSelectionPlace, selectionPlace, place, remove);
    Vec2 loc = convertFromScreenToSpace(Vec2(visibleSize.width, visibleSize.height), visibleSize, cam);
    Vec2 loc0 = convertFromScreenToSpace(Vec2(0, 0), visibleSize, cam);
}

//void MapEditor::chunkUpdate(F32 dt)
//{
//    // i can't i just can't i feel like i wasnt built for this cancer
//    // im starting to hate this shit and hate my life too
//    // everything goes right until on day it stops and throws
//    // a punch at my face i hate this illness
//    // and more importantly i hate you c++ you gave me cancer and youll still do
//    // this code works perfectly fine BUT it will fuck up some day soon
//
//    Vec2 loc = convertFromScreenToSpace(Vec2(visibleSize.width, visibleSize.height), visibleSize);
//    Vec2 loc0 = convertFromScreenToSpace(Vec2(0, 0), visibleSize);
//    for (const auto& i : chunks) {
//        Vec2 l = convertFromChunkSpaceToSpace(Vec2(i->x, i->y));
//        if (l.x + chunkSize < loc0.x || l.y + chunkSize < loc0.y || l.x > loc.x || l.y > loc.y) {
//            if (!i->rebuild) {
//                i->rebuild = true;
//                chunkNode->removeChild(i->batch);
//                i->hasTransparentTile = false;
//                CCLOG("UPDATE: frustum culling unloaded chunk model at %d,%d GID: %d", i->x, i->y, i->tileGID);
//            }
//        }
//        else {
//            if (i->rebuild)
//                chunksToRebuild.push_back(i);/*
//            if (!i.rebuild)
//                i.batch->visit(renderer, this->getWorldToNodeTransform(), 0);*/
//        }
//    }
//    bool removeEmptyChunks = false;
//    for (const auto& i : chunksToRebuild) {
//        if (i->rebuild)
//        {
//            sprite = Sprite::createWithTexture(tex);
//            sprite->retain();
//            sprite->setScaleX(contentScale);
//            sprite->setScaleY(contentScale);
//            sprite->setAnchorPoint(Vec2(0, 0));
//            if (i->isEdited)
//            {
//                chunkNode->removeChild(i->batch);
//                i->hasTransparentTile = false;
//                //CCLOG("UPDATE: frustum culling unloaded chunk model at %d,%d", i->x, i->y);
//                i->isEdited = false;
//            }
//            if (i->tiles.size() == 0) {
//                removeEmptyChunks = true;
//                continue;
//            }
//            i->index = -1;
//            i->batch = SpriteBatchNode::createWithTexture(tex, i->tiles.size());
//            for (CTile* t : i->tiles)
//            {
//                i->index++;
//                Vec2 pos = convertFromTileSpaceToSpace(Vec2(t->x, t->y));
//                pos.x = pos.x + tileSize / 2;
//                pos.y = pos.y + tileSize / 2;
//                sprite->setPosition(pos);
//                sprite->setTextureRect(Rect(Vec2(tileSizePure * 3, tileSizePure * 0), Size(tileSizePure, tileSizePure)));
//                auto col = ColorConversion::hex2rgba(t->hex);
//                if (col.r < 255 || col.g < 255 || col.b < 255)
//                    sprite->setColor(Color3B(col.r, col.g, col.b));
//                if (col.a < 255)
//                {
//                    sprite->setOpacity(col.a);
//                    i->hasTransparentTile = true;
//                }
//                i32 rot = 90 * t->rot;
//                if (rot != 0)
//                    sprite->setRotation(rot);
//                sprite->setAnchorPoint(Vec2(0.5f, 0.5f));
//                i->batch->insertQuadFromSprite(sprite, i->index);
//            }
//            i->rebuild = false;
//            chunkNode->addChild(i->batch, 0);
//            i->batch->setProgramState(defaultTileProgramState);
//            if (!i->hasTransparentTile)
//                i->batch->setBlendFunc(defaultTileBlend);
//        }
//    }
//    if (chunksToRebuild.size() > 0)
//    {
//        chunksToRebuild.clear();
//        if (removeEmptyChunks)
//            reorderChunks();
//    }
//}

// DON'T CALL THIS MANUALLY
void MapEditor::editUpdate_place(f32 _x, f32 _y, f32 _width, f32 _height) {
    _x = round(_x / map->_tileSize.x);
    _y = round(_y / map->_tileSize.y);
    std::vector v = { 1,2,3,1035 };
    TileID gid = v[Random::maxInt(v.size() - 1)];
        if (Random::float01() > 0.5)
            gid |= TILE_FLAG_ROTATE;
        if (Random::float01() > 0.5)
            gid |= TILE_FLAG_FLIP_X;
        if (Random::float01() > 0.5)
            gid |= TILE_FLAG_FLIP_Y;
    map->setTileAt({ _x, _y }, gid);
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
        createRemoveToolTileSelectionBox(removeSelectionStartPos, convertFromScreenToSpace(_input->_mouseLocation, visibleSize, _defaultCamera, true), map->_tileSize.x);

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
                editUpdate_place(vX, vY, placeStampSize.width, placeStampSize.height);
                // break the loop cuz we dont want the stamp to be lined with frame lag
                // cuz that shit steals a TON of frames just skip it if its over 10 units on any axis
                //if (placeStampSize.width > 10 || placeStampSize.height > 10)
                //    break;
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

    if (keyCode == EventKeyboard::KeyCode::KEY_LEFT_ALT)
    {
        isEditorDragging = true;
    }

    if (keyCode == EventKeyboard::KeyCode::KEY_H)
        grid->setVisible(!grid->isVisible());

    if (keyCode == EventKeyboard::KeyCode::KEY_G)
        isEditorHideGrid = true;

    if (keyCode == EventKeyboard::KeyCode::KEY_P)
    {
        auto proj = Director::getInstance()->getProjection();

        if (proj == Director::Projection::_3D)
            Director::getInstance()->setProjection(Director::Projection::_2D);
        else
            Director::getInstance()->setProjection(Director::Projection::_3D);
    }

    if (keyCode == EventKeyboard::KeyCode::KEY_S)
    {
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

    if (keyCode == EventKeyboard::KeyCode::KEY_W)
    {
        _defaultCamera->runAction(
            RepeatForever::create(
                Sequence::create(
                    RotateBy::create(3, 360),
                    _NOTHING
                )
            )
        ); 
    }
}

void MapEditor::onKeyReleased(EventKeyboard::KeyCode keyCode, Event* event)
{
    if (keyCode == EventKeyboard::KeyCode::KEY_LEFT_ALT)
    {
        isEditorDragging = false;
    }
}

void MapEditor::onMouseDown(ax::Event* event)
{
    EventMouse* e = (EventMouse*)event;

    if (e->getMouseButton() == EventMouse::MouseButton::BUTTON_4)
    {
        isEditorDragging = true;
    }

    if (e->getMouseButton() == EventMouse::MouseButton::BUTTON_5)
    {
        for (auto n : findNodesByTag(this, 91))
        {
            auto p = (ParticleSystemQuad*)n;

            //if (p->isUpdatePaused())
            //    p->resumeUpdate();
            //else
            //    p->pauseUpdate();
        }
    }
    if (e->getMouseButton() == EventMouse::MouseButton::BUTTON_LEFT)
    {
        isPlacing = true;

        printf("%d\n", ++cur);
        auto mouseClick = DrawNode::create(1);
        mouseClick->setPosition(Vec2(_input->_mouseLocation.x - (visibleSize.x / 2), (_input->_mouseLocation.y + (visibleSize.y / -2)) * -1));
        mouseClick->addComponent(new DrawNodeCircleExpandComponent(.5, 80, 32));
        DESTROY(mouseClick, .5);
        uiNode->addChild(mouseClick, 999);
    }
    if (e->getMouseButton() == EventMouse::MouseButton::BUTTON_RIGHT)
    {
        isRemoving = true;
        removeSelectionStartPos = convertFromScreenToSpace(_input->_mouseLocation, visibleSize, _defaultCamera, true);
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
        isPlacing = false;
    }
    if (e->getMouseButton() == EventMouse::MouseButton::BUTTON_RIGHT)
    {
        isRemoving = false;
        Rect rect = createRemoveToolTileSelectionBox(removeSelectionStartPos, convertFromScreenToSpace(_input->_mouseLocation, visibleSize, _defaultCamera, true), map->_tileSize.x);
        editUpdate_remove(rect.origin.x, rect.origin.y, rect.size.width, rect.size.height);
        std::cout << "remove_selection_tool: begin:" << rect.origin.x << "," << rect.origin.y << " end:" << rect.size.width << "," << rect.size.height << "\n";
        removeSelectionNode->clear();
    }
}

void MapEditor::onMouseMove(ax::Event* event)
{
    if (!hasMouseMoved) { hasMouseMoved = true; return; }
    if (isEditorDragging)
    {
        cameraLocation->setPositionX(cameraLocation->getPositionX() + (_input->_mouseLocationDelta.x * _defaultCamera->getZoom()));
        cameraLocation->setPositionY(cameraLocation->getPositionY() + (_input->_mouseLocationDelta.y * -1 * _defaultCamera->getZoom()));
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

    Vec2 targetPos = convertFromScreenToSpace(_input->_mouseLocationInView, visibleSize, getDefaultCamera());
    Vec2 pos = cameraLocation->getPosition();
    Vec2 newPos = pos.lerp(targetPos, 1.0F - (postCamScl / preCamScl));
    cameraLocation->runAction(Sequence::create(MoveTo::create(0, Vec2(newPos.x, newPos.y)), NULL));
    _defaultCamera->setZoom(cameraScale);
    setWorldBoundsLayerColorTransforms(_defaultCamera);
    set_cameraScaleUiText(cameraScale);
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

    //_defaultCamera->setScaleX(clamp(_defaultCamera->getScaleX(), 1, 30));
    //_defaultCamera->setScaleY(clamp(_defaultCamera->getScaleY(), 1, 30));


}

bool MapEditor::onTouchBegan(ax::Touch* touch, ax::Event* event)
{
    isEditorDragging = true;
    //cameraLocation = _defaultCamera->getPosition();
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
    //    cameraLocation->setPositionX(cameraLocation->getPositionX() + (mouseLocationDelta.x * _defaultCamera->getZoom()));
    //    cameraLocation->setPositionX(cameraLocation->getPositionY() + (mouseLocationDelta.y * -1 * _defaultCamera->getZoom()));
    //}
    //_input->_mouseLocationInView = touch->getLocation();
}

void MapEditor::onTouchEnded(ax::Touch* touch, ax::Event* event)
{
    isEditorDragging = true;
    cameraLocation->setPosition(_defaultCamera->getPosition());
}

void MapEditor::onTouchCancelled(ax::Touch* touch, ax::Event* event)
{
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
    rebuildableUiNodes->removeAllChildren();
    statsParentNode = Node::create();
    statsParentNode->setPosition(Vec2(visibleSize.width / -2 + 5, visibleSize.height / -2));
    setNodeIgnoreDesignScale(statsParentNode);
    statsParentNode->addComponent((new CustomComponents::UiRescaleComponent(visibleSize))
        ->enableDesignScaleIgnoring()->setVisibleSizeHints(-2, 5, -2));
    f32 fontSize = 20;
    std::string fontName = "fonts/arial.ttf";
    FPSUiText = ui::Text::create("FPS_UI_TEXT", fontName, fontSize);
    VertsUiText = ui::Text::create("VERTS_UI_TEXT", fontName, fontSize);
    BatchesUiText = ui::Text::create("BATCHES_UI_TEXT", fontName, fontSize);
    ChunkUiText = ui::Text::create("CHUNK_UI_TEXT", fontName, fontSize);
    setUiTextDefaultShade(FPSUiText, false);
    setUiTextDefaultShade(VertsUiText, false);
    setUiTextDefaultShade(BatchesUiText, false);
    setUiTextDefaultShade(ChunkUiText, false);
    statsParentNode->addChild(FPSUiText);
    statsParentNode->addChild(VertsUiText);
    statsParentNode->addChild(BatchesUiText);
    statsParentNode->addChild(ChunkUiText);
    rebuildableUiNodes->addChild(statsParentNode);
    FPSUiText->setAnchorPoint(Vec2(0, 0));
    /* FPS COUNTER CODE BODY */ {
        FPSUiText->stopAllActions();
        auto update_fps_action = CallFunc::create([&]() {
            char buff[14];
            char buffDt[14];
            fps_dt += (_director->getInstance()->getDeltaTime() - fps_dt) * 0.25f;
            snprintf(buff, sizeof(buff), "%.1lf", 1.0F / fps_dt);
            snprintf(buffDt, sizeof(buffDt), "%.1lf", fps_dt * 1000);
            std::string buffAsStdStr = buff;
            std::string buffAsStdStrDt = buffDt;
            FPSUiText->setString("D3D11: " + buffAsStdStr + " | " + buffAsStdStrDt + "ms");
        });
        auto wait_fps_action = DelayTime::create(0.5f);
        auto make_seq = Sequence::create(update_fps_action, wait_fps_action, nullptr);
        auto seq_repeat_forever = RepeatForever::create(make_seq);
        FPSUiText->runAction(seq_repeat_forever);
    }
    VertsUiText->setAnchorPoint(Vec2(0, 0));
    BatchesUiText->setAnchorPoint(Vec2(0, 0));
    ChunkUiText->setAnchorPoint(Vec2(0, 0));
    f32 spaceY = FPSUiText->getContentSize().height;
    VertsUiText->setPositionY(spaceY);
    spaceY += spaceY / 1;
    BatchesUiText->setPositionY(spaceY);
    spaceY += spaceY / 2;
    ChunkUiText->setPositionY(spaceY);

    // Camera Ui Scale Text And Sprites
    //{
    //    cameraScaleUi = Node::create();
    //    cameraScaleUi->setCascadeOpacityEnabled(true);
    //    setNodeIgnoreDesignScale(cameraScaleUi);
    //    f32 posY = visibleSize.height / 2 - 10;
    //    cameraScaleUi->addComponent((new CustomComponents::UiRescaleComponent(visibleSize))
    //        ->enableDesignScaleIgnoring()->setVisibleSizeHints(0, 0, 2, -10));
    //    cameraScaleUi->setPositionY(posY);
    //    cameraScaleUiText = ui::Text::create("x1.0", "fonts/SourceCodePro-Regular.ttf", 24);
    //    //cameraScaleUiText->setTextHorizontalAlignment(ax::TextHAlignment::RIGHT);
    //    cameraScaleUiText->setPositionX(5);
    //    cameraScaleUiText->setAnchorPoint(Vec2(0.5, 1));
    //    setUiTextDefaultShade(cameraScaleUiText);
    //    cameraScaleUi->addChild(cameraScaleUiText);
    //    cameraScaleUiAlphaSpriteCascade = Node::create();
    //    cameraScaleUiAlphaSpriteCascade->setCascadeOpacityEnabled(true);
    //    cameraScaleUi->addChild(cameraScaleUiAlphaSpriteCascade);
    //    cameraScaleUiSpNormal = Sprite::createWithSpriteFrameName("editor_camera_zoom_x1.png");
    //    cameraScaleUiSpSmall = Sprite::createWithSpriteFrameName("editor_camera_zoom_small.png");
    //    cameraScaleUiSpBig = Sprite::createWithSpriteFrameName("editor_camera_zoom_big.png");
    //    cameraScaleUiSpNormal->setAnchorPoint(Vec2(1, 1));
    //    cameraScaleUiSpSmall->setAnchorPoint(Vec2(1, 1));
    //    cameraScaleUiSpBig->setAnchorPoint(Vec2(1, 1));
    //    cameraScaleUiSpNormal->setPositionX(-40);
    //    cameraScaleUiSpSmall->setPositionX(-40);
    //    cameraScaleUiSpBig->setPositionX(-40);
    //    cameraScaleUi->addChild(cameraScaleUiText);
    //    cameraScaleUiAlphaSpriteCascade->addChild(cameraScaleUiSpNormal);
    //    cameraScaleUiAlphaSpriteCascade->addChild(cameraScaleUiSpSmall);
    //    cameraScaleUiAlphaSpriteCascade->addChild(cameraScaleUiSpBig);
    //    cameraScaleResetButton = CustomUi::createPlaceholderButton();
    //    uiHitEvents.push_back(cameraScaleResetButton);
    //    cameraScaleResetButton->addTouchEventListener([&](Ref* sender, ui::Widget::TouchEventType type) {
    //        switch (type)
    //        {
    //        case ui::Widget::TouchEventType::BEGAN:
    //        {
    //            auto anim = FadeTo::create(0.1, 120);
    //            cameraScaleUiAlphaSpriteCascade->runAction(anim);
    //            set_cameraScaleUiText(cameraScale);
    //            break;
    //        }
    //        case ui::Widget::TouchEventType::ENDED:
    //        {
    //            auto anim = FadeTo::create(0.1, 255);
    //            cameraScaleUiAlphaSpriteCascade->runAction(anim);
    //            if (cameraScale != 1)
    //            {
    //                i32 i = 0;
    //                i32 n = (sizeof(possibleCameraScales) / sizeof(possibleCameraScales[0])) - 1;
    //                while (i < n)
    //                {
    //                    if (possibleCameraScales[i] == 1.0F)
    //                        break;
    //                    i++;
    //                }
    //                cameraScaleIndex = i;
    //                cameraScale = possibleCameraScales[cameraScaleIndex];
    //                /*_defaultCamera->runAction(ease);
    //                auto moveAnim = MoveTo::create(5.3f, Vec3(0, 0, _defaultCamera->getPositionZ()));
    //                auto ease1 = EaseElasticOut::create(moveAnim);
    //                cameraLocation->runAction(ease1);*/
    //                set_cameraScaleUiText(cameraScale);
    //            }
    //            break;
    //        }
    //        case ui::Widget::TouchEventType::CANCELED:
    //        {
    //            isUiObstructing = false;
    //            auto anim = FadeTo::create(0.1, 255);
    //            cameraScaleUiAlphaSpriteCascade->runAction(anim);
    //            set_cameraScaleUiText(cameraScale);
    //            break;
    //        }
    //        default:
    //            break;
    //        }
    //        });
    //    CustomUi::hookPlaceholderButtonToNode(cameraScaleUiSpNormal, cameraScaleResetButton);
    //    rebuildableUiNodes->addChild(cameraScaleUi, 1);
    //    set_cameraScaleUiText(cameraScale);
    //}

    rebuildEntireUi();
}

void MapEditor::setUiTextDefaultShade(ui::Text* text_node, bool use_shadow)
{
    text_node->enableOutline(Color4B::BLACK, 1);
    if (use_shadow)
        text_node->enableShadow(Color4B::BLACK, Size(1, -1), 100);
}

void MapEditor::rebuildEntireUi()
{
    auto list = GameUtils::CocosExt::findComponentsByName(this, "UiRescaleComponent");
    for (auto i : list) dynamic_cast<GameUtils::CocosExt::CustomComponents::UiRescaleComponent*>(i)->windowSizeChange(visibleSize);
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

Rect MapEditor::createRemoveToolTileSelectionBox(Vec2 start_pos, Vec2 end_pos, i32 _tileSize)
{
    if (removeSelectionNode == nullptr) {
        removeSelectionNode = DrawNode::create();
        gridNode->addChild(removeSelectionNode, 2);
    }
    SelectionBox::Box box = SelectionBox::Box();
    Rect rect = createSelection(start_pos, end_pos, _tileSize, box);
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

void MapEditor::set_cameraScaleUiText(f32 scale)
{
    //if (scale == 1)
    //{
    //    cameraScaleUiSpNormal->setOpacity(255);
    //    cameraScaleUiSpSmall->setOpacity(0);
    //    cameraScaleUiSpBig->setOpacity(0);
    //}

    //if (scale < 1)
    //{
    //    cameraScaleUiSpNormal->setOpacity(0);
    //    cameraScaleUiSpSmall->setOpacity(255);
    //    cameraScaleUiSpBig->setOpacity(0);
    //}

    //if (scale > 1)
    //{
    //    cameraScaleUiSpNormal->setOpacity(0);
    //    cameraScaleUiSpSmall->setOpacity(0);
    //    cameraScaleUiSpBig->setOpacity(255);
    //}

    //char buff[8];
    //snprintf(buff, sizeof(buff), "x%g", scale);
    //str buffAsStdStr = buff;
    //cameraScaleUiText->setString(buffAsStdStr);

    //auto setVisiable = FadeTo::create(0.1f, 255);
    //auto setOpacityDelay = DelayTime::create(2);
    //auto setInvisiable = FadeTo::create(1, 100);
    //auto seq = Sequence::create(setVisiable, setOpacityDelay, setInvisiable, nullptr);
    //seq->setTag(7);
    //cameraScaleUi->stopAllActionsByTag(7);
    //cameraScaleUi->runAction(seq);
    
    //f32 spritesPosX = round(cameraScaleUiText->getContentSize().width / -2);
    //cameraScaleUiSpNormal->setPositionX(spritesPosX);
    //cameraScaleUiSpSmall->setPositionX(spritesPosX);
    //cameraScaleUiSpBig->setPositionX(spritesPosX);
}

void MapEditor::setWorldBoundsLayerColorTransforms(Camera* cam)
{
    BottomMapSizeNode->setVisible(false);
    TopMapSizeNode->setVisible(false);
    RightMapSizeNode->setVisible(false);
    LeftMapSizeNode->setVisible(false);

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
