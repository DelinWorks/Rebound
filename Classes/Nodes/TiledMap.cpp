#include "TiledMap.h"
#include "ccCArray.h"

#define IS_SOLID(L,X,Y) getTileGIDAt(L, { float(X),float(Y) }) != 0
#define IS_NOT_SOLID(L,X,Y) getTileGIDAt(L, { float(X),float(Y) }) == 0
#define IS_NOT_SOLID_OR_BLOCKED(L,X,Y) getTileGIDAt(L, { float(X),float(Y) }) == 0 \
    || std::count(blocked.begin(), blocked.end(), ax::Vec2(X, Y)) != 0
#define IS_SOLID_AND_NOT_BLOCKED(L,X,Y) getTileGIDAt(L, { float(X),float(Y) }) != 0 \
    && std::count(blocked.begin(), blocked.end(), ax::Vec2(X, Y)) == 0

enum LayerType {
    COLLISION = 0,
    ONE_WAY_COLLISION,
    OPPOSITE_WAY_COLLISION,
    RIGHT_ONLY_COLLISION,
    LEFT_ONLY_COLLISION,
    PLAYER_SPAWN,
    PLAYER_FINISH,
    SET_VELOCITY_ZONE,
    APPLY_IMPULSE_ZONE,
    SET_DIRECTION_ZONE,
    DECORATION,
    PARALLAX,
};

const char* LayerTypeLabel[] = { 
    "collision",
    "one_way_collision",
    "opposite_way_collision",
    "right_only_collision",
    "left_only_collision",
    "player_spawn",
    "player_finish",
    "set_velocity_zone",
    "apply_impulse_zone",
    "set_direction_zone",
    "decoration",
    "parallax",
};

#define IS_PROP_NOT_NULL_AND_FALSE(L,P) (!L->getProperty(P).isNull() && !L->getProperty(P).asBool())
#define IS_PROP_NOT_NULL_AND_TRUE(L,P) (!L->getProperty(P).isNull() && L->getProperty(P).asBool())

bool TiledMap::initWithFilename(ax::Scene* scene, std::string_view file, CatPlayer* _player)
{
    player = _player;

    int relativeSize = 32;

    auto tmx = tiledMap = ax::FastTMXTiledMap::create(file);

    if (!tmx) {
        tiledMap = nullptr;
        Darkness::getInstance()->MessageBoxWin32("Failed to load tmx map!", StringUtils::format("Map 'resources/%s' couldn't be loaded\nMake sure the map export format is set to CSV", std::string(file).c_str()));
        return false;
    }
    tiledMap->retain();

    //layerSolid = tmx->getLayer("Solid");
    //auto layerPlayerSpawn = tmx->getLayer("PlayerSpawn");
    //addChild(layerSolid);

    cam = scene->getDefaultCamera();

    tmx->setTileAnimEnabled(true);

    auto mapSize = tmx->getMapSize();
    auto tile = tmx->getTileSize();
    ax::Vec2 mapSizeInPixels = ax::Vec2(mapSize.x * tile.x, mapSize.y * tile.y);
    mapSizeInPixels -= mapSizeInPixels / 2;
    std::vector<ax::Vec2> blocked;

    auto getTileGIDAt = [&](ax::TMXLayer* layer, const ax::Vec2& c) {
        int id = layer->getTileGIDAt({ float(c.x),float(c.y) });
        if (id != 0) return id;
        int id1 = layer->getTileGIDAt({ float(c.x + 1),float(c.y) });
        int id2 = layer->getTileGIDAt({ float(c.x - 1),float(c.y) });
        int id3 = layer->getTileGIDAt({ float(c.x),float(c.y + 1) });
        //if (id1 != 0 && id2 != 0 && id3 != 0) return 1;
        return 0;
    };

    int solidTileCount = 0;
    int solidCollCount = 0;

    auto collideIfInvisible = tmx->getProperty("invisible_collision").isNull() ? true : tmx->getProperty("invisible_collision").asBool();

    auto cameraZoom = tmx->getProperty("camera_zoom");
    if (!cameraZoom.isNull()) player->cam->setZoom(cameraZoom.asFloat());

    if (IS_PROP_NOT_NULL_AND_TRUE(tmx, "debug_show_collision")) {
        scene->getPhysicsWorld()->setDebugDrawMask(0xffff);
        player->debugMode = true;
    }

    int renderOrder = 0;

    for (auto& n : tmx->getChildren())
    {
        auto layer = DCAST(ax::FastTMXLayer, n);
        if (!layer) continue;

        //if (layer->getLayerName().length() > 0 && layer->_isSubLayer) {
        //    auto type = layer->getProperty("layer_type");
        //    if (!type.isNull()) {
        //        if (type.asString() != LayerTypeLabel[DECORATION])
        //        {
        //            //Darkness::getInstance()->MessageBoxWin32("Failed to load tmx map!", StringUtils::format("Layer '%s' with type '%s' cannot have multiple tilesets\nonly decoration layers can render multiple tilesets", std::string(layer->getLayerName()).c_str(), std::string(type.asString()).c_str()));
        //            continue;
        //        }
        //    }
        //}

        if (IS_PROP_NOT_NULL_AND_TRUE(layer, "camera_center_layer"))
        {
            int maxX = 0;
            int minX = mapSize.x;
            int maxY = 0;
            int minY = mapSize.y;

            for (size_t y = 0; y < mapSize.y; y += 1)
                for (size_t x = 0; x < mapSize.x; x += 1)
                {
                    if (IS_NOT_SOLID(layer, x, y)) continue;

                    if (maxX < x) maxX = x;
                    if (minX > x) minX = x;
                    if (maxY < y) maxY = y;
                    if (minY > y) minY = y;
                }

            float offsetX = Math::map(((minX + maxX) / 2.0), 0.0, mapSize.x, -mapSizeInPixels.x, mapSizeInPixels.x);
            float offsetY = Math::map(((mapSize.y - minY) + (mapSize.y - maxY)) / 2.0, 0.0, mapSize.y, -mapSizeInPixels.y, mapSizeInPixels.y);

            player->camPos = Vec2(offsetX * (relativeSize / tile.x) + (relativeSize / 2.0), offsetY * (relativeSize / tile.y) - (relativeSize / 2.0));
        }

        auto type = layer->getProperty("layer_type").asString();
        auto visible = layer->getProperty("visible").isNull() ? true : layer->getProperty("visible").asBool();
        layer->setVisible(visible);

        if ((type == LayerTypeLabel[COLLISION] || type == LayerTypeLabel[ONE_WAY_COLLISION] || type == LayerTypeLabel[OPPOSITE_WAY_COLLISION]
            || type == LayerTypeLabel[RIGHT_ONLY_COLLISION] || type == LayerTypeLabel[LEFT_ONLY_COLLISION])
            && (collideIfInvisible && layer->isVisible())) {
            for (size_t y = 0; y < mapSize.y; y += 1)
            {
                for (size_t x = 0; x < mapSize.x; x += 1)
                {
                    if (IS_SOLID(layer, x, y))
                        solidTileCount++;

                    if (IS_SOLID_AND_NOT_BLOCKED(layer, x, y))
                    {
                        int currentX = x;
                        int currentY = y;
                        int mostRightX = x;
                        int mostDownY = y;

                        for (size_t i = x; i <= mapSize.x; i++)
                        {
                            if (IS_SOLID_AND_NOT_BLOCKED(layer, i, y)) {
                                blocked.push_back(ax::Vec2(i, y));
                                mostRightX = i;
                            }
                            else break;
                        }

                        int shiftCount = 0;
                        int columnXCount = x;
                        if (x == mostRightX)
                            columnXCount++;
                        while (true) {
                            shiftCount++;
                            for (size_t i = x; i <= mostRightX; i++)
                                if (IS_NOT_SOLID_OR_BLOCKED(layer, i, y + shiftCount))
                                    goto main_sub2;
                            for (size_t i = x; i <= mostRightX; i++)
                                blocked.push_back(ax::Vec2(i, y + shiftCount));
                        }

                    main_sub2:
                        mostDownY = currentY + shiftCount - 1;

                        auto size = ax::Vec2(relativeSize + (mostRightX - currentX) * relativeSize, relativeSize + (mostDownY - currentY) * relativeSize);

                        float intrepX = (mostRightX + currentX) / 2.0;
                        intrepX = Math::map(intrepX, 0, mapSize.x, 0.0, 1.0);
                        float intrepY = (mostDownY + currentY) / 2.0;
                        intrepY = Math::map(intrepY, 0, mapSize.y, 0.0, 1.0);

                        float offsetX = Math::map(intrepX, 0.0, 1.0, -mapSizeInPixels.x, mapSizeInPixels.x);
                        float offsetY = Math::map(intrepY, 0.0, 1.0, mapSizeInPixels.y, -mapSizeInPixels.y);

                        auto contourShift = ax::Vec2(offsetX * (relativeSize / tile.x) + (relativeSize / 2.0),
                            offsetY * (relativeSize / tile.y) - (relativeSize / 2.0));

                        int collisionGroup = 1;

                        if (type == LayerTypeLabel[ONE_WAY_COLLISION])
                            collisionGroup = ONE_WAY_COLLISION_INDEX;

                        if (type == LayerTypeLabel[OPPOSITE_WAY_COLLISION])
                            collisionGroup = OPPOSITE_WAY_COLLISION_INDEX;

                        if (type == LayerTypeLabel[RIGHT_ONLY_COLLISION])
                            collisionGroup = RIGHT_ONLY_COLLISION_INDEX;

                        if (type == LayerTypeLabel[LEFT_ONLY_COLLISION])
                            collisionGroup = LEFT_ONLY_COLLISION_INDEX;

                        if (IS_PROP_NOT_NULL_AND_FALSE(layer, "can_jump"))
                            collisionGroup |= DISABLE_JUMP_COLLISION_INDEX;

                        if (IS_PROP_NOT_NULL_AND_TRUE(layer, "can_wall_jump"))
                            collisionGroup |= WALL_JUMP_COLLISION_INDEX;

                        if (IS_PROP_NOT_NULL_AND_FALSE(layer, "can_turn"))
                            collisionGroup |= DISABLE_TURN_COLLISION_INDEX;

                        auto w1 = Wall::createEntity(size, contourShift, collisionGroup);
                        layer->addChild(w1);
                        solidCollCount++;
                    }
                }
            }

            layer->setAnchorPoint({ 0.5, 0.5 });
            addChild(layer, renderOrder++);
        }
        else if (type == LayerTypeLabel[PLAYER_SPAWN]) {
            if (layer->getProperty("player_direction").asString() == "left")
                player->playerDirection *= -1;

            for (size_t y = 0; y < mapSize.y; y += 1)
            {
                for (size_t x = 0; x < mapSize.x; x += 1)
                {
                    if (layer->getTileGIDAt({ (float)x, (float)y }) != 0)
                    {
                        float offsetX = Math::map(x, 0.0, mapSize.x, -mapSizeInPixels.x, mapSizeInPixels.x);
                        float offsetY = Math::map(mapSize.y - y, 0.0, mapSize.y, -mapSizeInPixels.y, mapSizeInPixels.y);
                        player->player_sprite_parent->setPositionX(offsetX * (relativeSize / tile.x) + (relativeSize / 2.0));
                        player->player_sprite_parent->setPositionY(offsetY * (relativeSize / tile.y) - (relativeSize / 2.0));
                        player->debugLineTraceY.fill(player->player_sprite_parent->getPosition());
                        addChild(player, renderOrder++);
                        player->setVisible(visible);
                    }
                }
            }

            auto camWobbleSpeed = layer->getProperty("camera_wobble_speed_vector");
            auto camWobbleAmount = layer->getProperty("camera_wobble_amount_vector");
            auto camDisplaceVector = layer->getProperty("camera_displacement_vector");

            if (!camWobbleSpeed.isNull())
                player->camWobbleSpeed = GameUtils::Parser::parseVector2D(camWobbleSpeed.asString());

            if (!camWobbleAmount.isNull())
                player->camWobbleAmount = GameUtils::Parser::parseVector2D(camWobbleAmount.asString());

            if (!camDisplaceVector.isNull())
                player->camDisplaceVector = GameUtils::Parser::parseVector2D(camDisplaceVector.asString());
        }
        else if (type == LayerTypeLabel[DECORATION]) {
            layer->setAnchorPoint({ 0.5, 0.5 });
            addChild(layer, renderOrder++);
        }
        else if (type == LayerTypeLabel[PARALLAX]) {
            layer->setAnchorPoint({ 0.5, 0.5 });

            auto parallax = ParallaxNode::create();
            addChild(parallax, renderOrder++);

            auto parallaxRatioProp = layer->getProperty("parallax_ratio_vector");
            Vec2 parallaxRatio = parallaxRatioProp.isNull() ? Vec2::ZERO : 
                GameUtils::Parser::parseVector2D(parallaxRatioProp.asString());
            parallax->addChild(layer, 0, parallaxRatio, Vec2::ZERO);
        }

        layer->setEditorColor(ColorConversion::hex2argb(layer->getEditorRawTint()));

        auto opacityProp = layer->getProperty("fragment_opacity");
        if (!opacityProp.isNull())
            layer->setOpacity(opacityProp.asFloat() * 255);

        auto tintProp = layer->getProperty("fragment_tint");
        if (!tintProp.isNull()) {
            auto colorVect = tintProp.asString();
            layer->setColor(ColorConversion::hex2argb(colorVect));
        }
        else layer->setColor(Color4B::WHITE);

        auto tileAnimVariance = layer->getProperty("layer_anim_variance");
        if (!tileAnimVariance.isNull())
            if (layer->getTileAnimManager())
                for (auto& t : layer->getTileAnimManager()->getTasks())
                    t->update(Random::maxFloat(tileAnimVariance.asFloat()));

        auto tileAnimSpeedVariance = layer->getProperty("layer_anim_time_variance");
        if (!tileAnimSpeedVariance.isNull())
            if (layer->getTileAnimManager())
                for (auto& t : layer->getTileAnimManager()->getTasks())
                    t->setTimeScale(1.0 + Random::maxFloat(tileAnimSpeedVariance.asFloat()));

        layer->setProgramState(GameUtils::CocosExt::createGPUProgram());
    }

    int batchingPercentage = (1.0 - float(solidCollCount - 1) / solidTileCount) * 100.0;
    AXLOG("physics_shape_batch: %d shapes / %d tiles (%d%%) of all tile collision shapes have been batched.\nGrid-Based Shape Batching Algorithm. More percentage = Better performance\nBatching may break raycasting so consider seperating solid type layers\nwhen the player glitches or teleports outside the map.\n", solidCollCount, solidTileCount, batchingPercentage);

    player->setScale(1.0 / (relativeSize / tile.x));
    setScaleX((relativeSize / tile.x));
    setScaleY((relativeSize / tile.y));

	return true;
}

TiledMap::~TiledMap() {
    if (tiledMap)
        tiledMap->release();
}

void TiledMap::update(f32 dt) {

    for (auto&& c : _children)
    {
        auto p = DCAST(ax::ParallaxNode, c);
        if (p) {
            ax::PointObject* point = (ax::PointObject*)p->getParallaxArray()->arr[0];
            point->setOffset(player->camPos / -1.5);
            p->setPosition(cam->getPosition().x, cam->getPosition().y);
        }
    }

    tiledMap->update(dt);
}
