#include "GameplayScene.h"

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

    visibleSize = { 1280, 720 };

    physics = new DarknessPhysicsWorld();
    if (physics->init({ 0, -24.79 * 2 }))
        addChild(physics, 10);
    physics->SetContinuousPhysics(true);
    physics->debug(true);

    physics->preStepCallback = [&](DarknessPhysicsWorld* world, f32 dt) -> void { p->physicsPreStep(world, dt); };
    physics->postStepCallback = [&](DarknessPhysicsWorld* world, f32 dt) -> void { p->physicsPostStep(world, dt); };

    p = CatPlayer::createPhysicalEntity(physics);
    p->attachCamera(_defaultCamera);

    map = new TiledMap();
    if (map->initWithFilename(this, physics, "maps/level1/untitled.tmx", p))
        addChild(map);
    else
    {
        AX_SAFE_DELETE(map);

        lb = ax::Label::createWithSystemFont("Press F5 to reload the map or ESC to quit.", "arial", 24);
        addChild(lb, 10);

        auto comp = new GameUtils::CocosExt::CustomComponents::UiRescaleComponent(visibleSize);
        comp->enableDesignScaleIgnoring();
        lb->addComponent(comp);
    }

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

    if (!map)
        return;

    map->update(dt);

    awake();

    if (lastPhysicsDt < 1 && dt > 0.1)
        lastPhysicsDt = currentPhysicsDt;

    int physicsTPS = 1.0 / dt;
    
    physicsTPS = physicsTPS < 120 ? 120 : physicsTPS;

    physicsTPS *= 1;

    if (lastPhysicsDt + 10 < currentPhysicsDt)
        lastPhysicsDt = currentPhysicsDt;

    while (lastPhysicsDt < currentPhysicsDt)
    {
        lastPhysicsDt += 1.0 / physicsTPS;
        physics->update(1.0 / physicsTPS);
    }

    p->tick(dt);
}

void GameplayScene::onKeyPressed(cocos2d::EventKeyboard::KeyCode keyCode, cocos2d::Event* event)
{
    if (map)
        p->onKeyPressed(keyCode);
}

void GameplayScene::onKeyReleased(cocos2d::EventKeyboard::KeyCode keyCode, cocos2d::Event* event)
{
    if (map)
        p->onKeyReleased(keyCode);
}

void GameplayScene::onKeyHold(cocos2d::EventKeyboard::KeyCode keyCode, cocos2d::Event* event)
{
}

void GameplayScene::onMouseDown(cocos2d::Event* event)
{
    if (map)
        p->onMouseDown(event);
}

void GameplayScene::onMouseUp(cocos2d::Event* event)
{
    if (map)
        p->onMouseUp(event);
}

void GameplayScene::onMouseMove(cocos2d::Event* event)
{
}

void GameplayScene::onMouseScroll(cocos2d::Event* event)
{
    if (map)
        p->onMouseScroll(event);
}

bool GameplayScene::onTouchBegan(cocos2d::Touch* touch, cocos2d::Event* event)
{
    if (map)
        p->onTouchBegan(touch, event);

    return true;
}

void GameplayScene::onTouchMoved(cocos2d::Touch* touch, cocos2d::Event* event)
{
}

void GameplayScene::onTouchEnded(cocos2d::Touch* touch, cocos2d::Event* event)
{
    if (map)
        p->onTouchEnded(touch, event);
}

void GameplayScene::onTouchCancelled(cocos2d::Touch* touch, cocos2d::Event* event)
{
}
