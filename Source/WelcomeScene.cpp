#include "WelcomeScene.h"
#include "shared_scenes/GameUtils.h"
#include "MapEditor.h"
#include "PhysicsTestScene.h"

USING_NS_CC;

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

    auto textureCompanyLogo = Director::getInstance()->getTextureCache()->addImage("c4417b4b-a526-466e-ae41-5c75a1cc0f02.png");
    if (textureCompanyLogo) textureCompanyLogo->setAliasTexParameters();

    auto cnnode = Node::create();

    cn = Sprite::createWithTexture(textureCompanyLogo);
    {
        // position the sprite on the center of the screen
        //cnnode->setPosition(Vec2::ZERO);
        cn->setOpacity(0);
        cn->setScale(0.5f);
        cnnode->addChild(cn, 0);
        addChild(cnnode, 0);
    }
    Director::getInstance()->getTextureCache()->removeTexture(textureCompanyLogo);

    auto tex = Director::getInstance()->getTextureCache()->addImage("pixel.png");
    tex->setAliasTexParameters();

    tex = Director::getInstance()->getTextureCache()->addImage("star.png");
    tex->setAliasTexParameters();

    cnnode->addComponent((new UiRescaleComponent(visibleSize))
        ->setVisibleSizeHints());

    SCENE_BUILD_UI;

    return true;
}

void WelcomeScene::onInitDone()
{
    if (isInitDone)
        return;
    isInitDone = true;
    //Camera::getDefaultCamera()->setZoom(2);
}

void WelcomeScene::update(F32 dt)
{
    onInitDone();

    REBUILD_UI;

    if (animTime > _maxTime && cnOpacity <= 0 || true)
    {
        GameUtils::addSpriteFramesFromJson("shared/shared_ui.png", "shared/shared_ui.json");
        auto scene = MapEditor::createScene();
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
    SCENE_BUILD_UI;
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
