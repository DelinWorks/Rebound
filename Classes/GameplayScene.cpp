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
    //_defaultCamera->setZoom(3);

    initPhysicsWorld();
    getPhysicsWorld()->setGravity(Vec2(0, -2479));
    getPhysicsWorld()->setPreUpdateCallback([&] { p->physicsPreTick(getPhysicsWorld()); });
    getPhysicsWorld()->setPostUpdateCallback([&] { p->physicsPostTick(getPhysicsWorld()); });

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

    auto tiledMap = new TiledMap();
    tiledMap->createWithFilename(this, "maps/level1/untitled.tmx", p);
    addChild(tiledMap);

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

    if (lastPhysicsDt < 1 && dt > 0.1)
        lastPhysicsDt = currentPhysicsDt;

    int physicsTPS = 1.0 / dt;

    physicsTPS = physicsTPS < 120 ? 120 : physicsTPS;
    physicsTPS = physicsTPS > 240 ? 240 : physicsTPS;

    physicsTPS *= 2;

    if (lastPhysicsDt + 1 < currentPhysicsDt)
        lastPhysicsDt = currentPhysicsDt;

    while (lastPhysicsDt < currentPhysicsDt)
    {
        auto world = getPhysicsWorld();
        lastPhysicsDt += 1.0 / physicsTPS;
        world->update(1.0 / physicsTPS, true);
    }

    lb->setString((""));
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
