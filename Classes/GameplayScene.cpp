#include "GameplayScene.h"
#include "chipmunk/chipmunk_private.h"

#define LERP MathUtil::lerp

using namespace GameUtils::CocosExt::CustomComponents;

USING_NS_CC;

#define SET_UNIFORM(ps, name, value)  do { \
decltype(value) __v = value; \
auto __loc = (ps)->getUniformLocation(name); \
(ps)->setUniform(__loc, &__v, sizeof(__v)); \
} while(false) 

Scene* GameplayScene::createScene()
{
    return GameplayScene::create();
}

bool GameplayScene::init()
{
    if (!Scene::init())
    {
        return false;
    }

    REGISTER_SCENE(GameplayScene);

    scheduleUpdate();

    auto visibleSize = Director::getInstance()->getVisibleSize();

    _defaultCamera->setPosition({ 0,0 });
    //_defaultCamera->setZoom(5);

    initPhysicsWorld();
    getPhysicsWorld()->setGravity(Vec2(0, -2479));
    getPhysicsWorld()->setDebugDrawMask(0xffff);
    getPhysicsWorld()->setPreUpdateCallback([&] {
        p->physicsTick(getPhysicsWorld());
    });

    getPhysicsWorld()->setAutoStep(false);

    p = CatPlayer::createEntity();
    _eventDispatcher->addEventListenerWithSceneGraphPriority(p->contactor, this);
    p->attachCamera(_defaultCamera);


    cpSpaceSetCollisionSlop(getPhysicsWorld()->_cpSpace, 0);
    cpSpaceSetCollisionBias(getPhysicsWorld()->_cpSpace, 0);

    visibleSize = { 1280, 720 };

    lb = ax::Label::createWithSystemFont("0x0", "arial", 24);
    addChild(lb, 10);

    auto comp = new GameUtils::CocosExt::CustomComponents::UiRescaleComponent(visibleSize);
    comp->enableDesignScaleIgnoring();
    lb->addComponent(comp);

    int relativeSize = 32;

    auto tmx = TMXTiledMap::create("maps/level1/untitled.tmx");
    layerSolid = tmx->getLayer("Solid");
    auto layerPlayerSpawn = tmx->getLayer("PlayerSpawn");
    layerSolid->setAnchorPoint({ 0.5, 0.5 });
    addChild(layerSolid);
    int group = 199;
    auto size = tmx->getMapSize();
    auto tile = tmx->getTileSize();
    std::vector<ax::Vec2> blocked;

    auto getTileGIDAt = [&] (const Vec2& c) {
        int id = layerSolid->getTileGIDAt({ float(c.x),float(c.y) });
        if (id != 0) return id;
        int id1 = layerSolid->getTileGIDAt({ float(c.x + 1),float(c.y) });
        int id2 = layerSolid->getTileGIDAt({ float(c.x - 1),float(c.y) });
        int id3 = layerSolid->getTileGIDAt({ float(c.x),float(c.y + 1) });
        if (id1 != 0 && id2 != 0 && id3 != 0) return 1;
        return 0;
    };

#define IS_SOLID(X,Y) getTileGIDAt({ float(X),float(Y) }) != 0
#define IS_NOT_SOLID(X,Y) getTileGIDAt({ float(X),float(Y) }) == 0
#define IS_NOT_SOLID_AND_NOT_BLOCKED(X,Y) getTileGIDAt({ float(X),float(Y) }) == 0 \
    && std::count(blocked.begin(), blocked.end(), Vec2(X, Y)) == 0
#define IS_SOLID_AND_NOT_BLOCKED(X,Y) getTileGIDAt({ float(X),float(Y) }) != 0 \
    && std::count(blocked.begin(), blocked.end(), Vec2(X, Y)) == 0

    int solidTileCount = 0;
    int solidCollCount = 0;

    bool useColliderBatching = false;

    for (size_t y = 0; y < size.y; y += 1)
    {
        for (size_t x = 0; x < size.x; x += 1)
        {
            if (IS_SOLID(x, y))
                solidTileCount++;

            if (!useColliderBatching) {
                if (IS_SOLID(x, y)) {
                    auto w1 = Wall::createEntity({tile.x, tile.y}, {0, 0});
                    w1->setPositionX((x * tile.x + tile.x / 2));
                    w1->setPositionY((size.y - y) * tile.x - tile.x / 2);
                    layerSolid->addChild(w1);
                    solidCollCount++;
                }
                continue;
            }

            if (IS_SOLID_AND_NOT_BLOCKED(x, y))
            {
                int currentX = x;
                int currentY = y;
                int mostRightX = x;
                int mostDownY = y;

                for (size_t i = y; i <= size.y; i++)
                {
                    if (IS_SOLID_AND_NOT_BLOCKED(x, i)) {
                        blocked.push_back(Vec2(x, i));
                        mostDownY = i;
                    }
                    else break;
                }

                int shiftCount = 0;
                int columnYCount = y;
                if (y == mostDownY)
                    columnYCount++;
                while (true) {
                    shiftCount++;
                    for (size_t i = y; i <= mostDownY; i++)
                        if (IS_NOT_SOLID_AND_NOT_BLOCKED(x + shiftCount, i))
                            goto main_sub2;
                    for (size_t i = y; i <= mostDownY; i++)
                        blocked.push_back(Vec2(x + shiftCount, i));
                }

                main_sub2:
                mostRightX = currentX + shiftCount - 1;

                auto contourShift = Vec2(tile.x / 2 * (mostRightX - currentX), tile.y / -2 * (mostDownY - currentY));

                auto w1 = Wall::createEntity(
                    Vec2(tile.x + (mostRightX - currentX) * tile.x, tile.y + (mostDownY - currentY) * tile.y), contourShift
                );
                w1->setPositionX((x * tile.x + tile.x / 2));
                w1->setPositionY((size.y - y) * tile.x - tile.x / 2);
                layerSolid->addChild(w1);
                solidCollCount++;
            }
            //else {
            //    int xAfter = x + 1;
            //    int xBefore = x - 1;

            //    if (xAfter < size.x - 1 && xBefore > 0) {
            //        if (layerNonSolid->getTileGIDAt({ float(xAfter),float(y) }) != 0 &&
            //            layerNonSolid->getTileGIDAt({ float(xBefore),float(y) }) != 0)
            //        {
            //            auto w1 = Wall::createEntity(group++);
            //            w1->setPositionX((x * 32 - visibleSize.x / 2) + 16);
            //            w1->setPositionY((y * 32 - visibleSize.y / 2) * -1 + 8);
            //            //addChild(w1);
            //            //p->Colliders.push_back(w1);
            //        }
            //    }
            //}
        }
    }

    int batchingPercentage = (1.0 - float(solidCollCount - 1) / solidTileCount) * 100.0;

    AXLOG("physics_shape_batch: %d shapes / %d tiles (%d%%) of all tile collision shapes have been batched.\nGrid-Based Shape Batching Algorithm. More percentage = Better performance", solidCollCount, solidTileCount, batchingPercentage);

    layerSolid->addChild(p, 5);

    p->setScale(1.0 / (relativeSize / tile.x));
    layerSolid->setScaleX((relativeSize / tile.x));
    layerSolid->setScaleY((relativeSize / tile.y));

    for (size_t y = 0; y < size.y; y += 1)
    {
        for (size_t x = 0; x < size.x; x += 1)
        {
            if (layerPlayerSpawn->getTileGIDAt({ float(x),float(y) }) != 0)
            {
                p->setPositionX((x * tile.x + tile.x / 2));
                p->setPositionY((size.y - y) * tile.y - tile.y / 2);
                goto generate_sub1;
            }
        }
    }

    generate_sub1:

    return true;
}

void GameplayScene::awake()
{
    if (Node::isAwake()) {
        p->setInputState(true);
        currentPhysicsDt = lastPhysicsDt = 0;
    }
}

void GameplayScene::update(f32 dt)
{
    currentPhysicsDt += dt;

    awake();

#ifdef WIN32
    int physicsTPS = glfwGetVideoMode(glfwGetPrimaryMonitor())->refreshRate;
#else
    int physicsTPS = 120;
#endif
    physicsTPS = physicsTPS < 120 ? 120 : physicsTPS;
    
    /* 1: LOW ACCURACY PHYSICS */
    /* 2: HIGH ACCURACY PHYSICS */
    /* 3: ULTRA ACCURACY PHYSICS */
    /* 4: EXTREME ACCURACY PHYSICS */
    physicsTPS *= 2;

    if (lastPhysicsDt + 2 < currentPhysicsDt)
        lastPhysicsDt = currentPhysicsDt;

    while (lastPhysicsDt < currentPhysicsDt)
    {
        lastPhysicsDt += 1.0 / physicsTPS;
        getPhysicsWorld()->step(1.0 / physicsTPS);
    }

    lb->setString(TO_TEXT(p->numberOfFlips));
}

void GameplayScene::onKeyPressed(cocos2d::EventKeyboard::KeyCode keyCode, cocos2d::Event* event)
{
    p->onKeyPressed(keyCode);
}

void GameplayScene::onKeyReleased(cocos2d::EventKeyboard::KeyCode keyCode, cocos2d::Event* event)
{
    p->onKeyReleased(keyCode);
}

void GameplayScene::onKeyHold(cocos2d::EventKeyboard::KeyCode keyCode, cocos2d::Event* event)
{
}

void GameplayScene::onMouseDown(cocos2d::Event* event)
{
    p->onMouseDown(event);
}

void GameplayScene::onMouseUp(cocos2d::Event* event)
{
    p->onMouseUp(event);
}

void GameplayScene::onMouseMove(cocos2d::Event* event)
{
}

void GameplayScene::onMouseScroll(cocos2d::Event* event)
{
    p->onMouseScroll(event);
}

bool GameplayScene::onTouchBegan(cocos2d::Touch* touch, cocos2d::Event* event)
{
    return false;
}

void GameplayScene::onTouchMoved(cocos2d::Touch* touch, cocos2d::Event* event)
{
}

void GameplayScene::onTouchEnded(cocos2d::Touch* touch, cocos2d::Event* event)
{
}

void GameplayScene::onTouchCancelled(cocos2d::Touch* touch, cocos2d::Event* event)
{
}
