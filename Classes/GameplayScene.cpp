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

    initPhysicsWorld();
    getPhysicsWorld()->setGravity(Vec2(0, -2479));
    // optional: enable debug draw
    getPhysicsWorld()->setDebugDrawMask(0xffff);

    getPhysicsWorld()->setAutoStep(true);
    getPhysicsWorld()->setSubsteps(32);

    p = CatPlayer::createEntity();
    addChild(p);
    p->attachCamera(_defaultCamera);
    p->setInputState(true);

    auto w1 = Wall::createEntity();
    w1->setScaleX(10);
    w1->setPositionY(-300);
    addChild(w1);

    w1 = Wall::createEntity();
    w1->setScaleY(3);
    w1->setPositionX(-166);
    w1->setPositionY(-220);
    addChild(w1);

    w1 = Wall::createEntity();
    w1->setScaleY(3);
    w1->setPositionX(166);
    w1->setPositionY(-220);
    addChild(w1);

    w1 = Wall::createEntity();
    w1->setScaleX(4);
    w1->setPositionX(266);
    w1->setPositionY(-220);
    addChild(w1);

    w1 = Wall::createEntity();
    w1->setScaleX(4);
    w1->setPositionX(-266);
    w1->setPositionY(-220);
    addChild(w1);

    w1 = Wall::createEntity();
    w1->setScaleY(140);
    w1->setPositionX(-366);
    w1->setPositionY(-120);
    addChild(w1);

    w1 = Wall::createEntity();
    w1->setScaleY(4);
    w1->setPositionX(366);
    w1->setPositionY(-120);
    addChild(w1);

    w1 = Wall::createEntity();
    w1->setScaleY(4);
    w1->setPositionX(166);
    addChild(w1);

    w1 = Wall::createEntity();
    w1->setScaleX(4);
    w1->setPositionX(400);
    w1->setPositionY(66);
    addChild(w1);

    w1 = Wall::createEntity();
    w1->setScaleY(4);
    w1->setPositionX(507);
    w1->setPositionY(0);
    addChild(w1);

    w1 = Wall::createEntity();
    w1->setScaleY(14);
    w1->setPositionX(574);
    w1->setPositionY(0);
    addChild(w1);

    w1 = Wall::createEntity();
    w1->setScaleX(.01);
    w1->setPositionX(540);
    w1->setPositionY(0);
    addChild(w1);

    lb = ax::Label::createWithSystemFont("0x0", "arial", 24);
    addChild(lb);

    return true;
}

void GameplayScene::awake()
{
    if (Node::isAwake()) {

    }
}

void GameplayScene::update(f32 dt)
{
    awake();

    p->update(dt);

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
