#include "WelcomeScene.h"
#include "shared_scenes/GameUtils.h"

USING_NS_CC;

using namespace GameUtils::CocosExt::CustomComponents;

Scene* WelcomeScene::createScene()
{
    return WelcomeScene::create();
}

bool WelcomeScene::init()
{
    //CCLOG("EXE");
    //////////////////////////////
    // 1. super init first
    if (!Scene::init())
    {
        return false;
    }

    REGISTER_SCENE(WelcomeScene);

    scheduleUpdate();

    visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();

    listener = EventListenerKeyboard::create();
    listener->onKeyPressed = CALL2(WelcomeScene::onKeyPressed);
    listener->onKeyReleased = CALL2(WelcomeScene::onKeyReleased);
    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);

    _mouseListener = EventListenerMouse::create();
    _mouseListener->onMouseMove = CALL1(WelcomeScene::onMouseMove);
    _mouseListener->onMouseUp = CALL1(WelcomeScene::onMouseUp);
    _mouseListener->onMouseDown = CALL1(WelcomeScene::onMouseDown);
    _mouseListener->onMouseScroll = CALL1(WelcomeScene::onMouseScroll);
    _eventDispatcher->addEventListenerWithSceneGraphPriority(_mouseListener, this);

    auto textureCompanyLogo = Director::getInstance()->getTextureCache()->addImage("c4417b4b-a526-466e-ae41-5c75a1cc0f02.png");
    if (textureCompanyLogo) textureCompanyLogo->setAliasTexParameters();

    auto cnnode = Node::create();
    cnnode->addComponent((new GameUtils::CocosExt::CustomComponents::UiRescaleComponent(visibleSize))
        ->setVisibleSizeHints());

    cn = Sprite::createWithTexture(textureCompanyLogo);
    {
        // position the sprite on the center of the screen
        cnnode->setPosition(Vec2::ZERO);
        cn->setOpacity(0);
        cn->setScale(0.5f);
        cnnode->addChild(cn, 0);
        addChild(cnnode, 0);
    }
    Director::getInstance()->getTextureCache()->removeTexture(textureCompanyLogo);

    //Size actualFrameSize = Director::getInstance()->getOpenGLView()->getFrameSize();
    //Size actualWinSize = Director::getInstance()->getWinSizeInPixels();
    //cn->setScaleX(actualWinSize.width / actualFrameSize.width);
    //cn->setScaleY(actualWinSize.height / actualFrameSize.height);

    //auto sound = FMODAudioEngine::getInstance()->playSound("chase_music_RND_INC", FMODAudioEngine::platformResourcePath("RLGrimeJuelzFormula.mp3"), true, true);
    //sound->setReverbProperties(0, 20.0F);
    //sound->setVolume(10.0F);

    return true;
}

void WelcomeScene::onInitDone()
{
    if (isInitDone)
        return;
    isInitDone = true;
    //Camera::getDefaultCamera()->setZoom(2);
}

void WelcomeScene::update(f32 dt)
{
    onInitDone();

    REBUILD_UI;

    if (animTime > _maxTime && cnOpacity <= 0)
    {
        GameUtils::CocosExt::addSpriteFramesFromJson("player/player_atlas.png", "player/player_atlas.json");
        auto scene = GameplayScene::createScene();
        Director::getInstance()->pushScene(scene);
    }

    animTime += dt;

    if (animTime > 0.5 && animTime < _maxTime)
        cnOpacity += 200 * dt;

    if (animTime > _maxTime)
        cnOpacity -= 200 * dt;

    if (cnOpacity > 255)
        cnOpacity = 255;

    if (cnOpacity < 0)
        cnOpacity = 0;

    //if (animTime > 6 && cnOpacity == 0 && cn != NULL)
    //{
    //    removeChild(cn);
    //    cn->autorelease();
    //    cn = NULL;
    //    CCLOG("WOW");
    //}

    cn->setOpacity((int)cnOpacity);
    //CCLOG(std::to_string(cnOpacity).c_str());
}

void WelcomeScene::runNext()
{
    animTime = _maxTime;
}

void WelcomeScene::rebuildEntireUi()
{
    RESIZE_UI_ELEMENTS;
}

void WelcomeScene::onKeyHold(cocos2d::EventKeyboard::KeyCode keyCode, cocos2d::Event* event)
{

}

void WelcomeScene::onKeyPressed(cocos2d::EventKeyboard::KeyCode keyCode, cocos2d::Event* event)
{
    if (animTime < _maxTime)
        runNext();
}

void WelcomeScene::onKeyReleased(cocos2d::EventKeyboard::KeyCode keyCode, cocos2d::Event* event)
{

}

void WelcomeScene::onMouseDown(Event* event)
{
    if (animTime < _maxTime)
        runNext();
}

void WelcomeScene::onMouseUp(Event* event)
{

}

void WelcomeScene::onMouseMove(Event* event)
{

}

void WelcomeScene::onMouseScroll(Event* event)
{

}

bool WelcomeScene::onTouchBegan(cocos2d::Touch* touch, cocos2d::Event* event)
{
    if (animTime < _maxTime)
        runNext();

    return false;
}

void WelcomeScene::onTouchMoved(cocos2d::Touch* touch, cocos2d::Event* event)
{
}

void WelcomeScene::onTouchEnded(cocos2d::Touch* touch, cocos2d::Event* event)
{
}

void WelcomeScene::onTouchCancelled(cocos2d::Touch* touch, cocos2d::Event* event)
{
}
