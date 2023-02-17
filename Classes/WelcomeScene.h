#ifndef _WELCOME_SCENE_H_
#define _WELCOME_SCENE_H_

#include "cocos2d.h"
#include "shared_scenes/GameUtils.h"
#include "fmod/include_fmod.h"
#include "AppDelegate.h"
#include "EmptyScene.h"
#include "GameplayScene.h"
#include "Helper/win32_error.h"
#include "Helper/short_types.h"

class WelcomeScene : public cocos2d::Scene
{
public:
    static cocos2d::Scene* createScene();

    void update(f32 dt);
    void runNext();
    void rebuildEntireUi();
    void lateUpdate(f32 dt);

    virtual bool init();
    bool isInitDone = false;
    void onInitDone();

    cocos2d::EventListenerKeyboard* listener;
    cocos2d::EventListenerMouse* _mouseListener;

    void onKeyPressed(cocos2d::EventKeyboard::KeyCode keyCode, cocos2d::Event* event);
    void onKeyReleased(cocos2d::EventKeyboard::KeyCode keyCode, cocos2d::Event* event);
    void onKeyHold(cocos2d::EventKeyboard::KeyCode keyCode, cocos2d::Event* event);
    void onMouseDown(cocos2d::Event* event);
    void onMouseUp(cocos2d::Event* event);
    void onMouseMove(cocos2d::Event* event);
    void onMouseScroll(cocos2d::Event* event);
    bool onTouchBegan(cocos2d::Touch* touch, cocos2d::Event* event);
    void onTouchMoved(cocos2d::Touch* touch, cocos2d::Event* event);
    void onTouchEnded(cocos2d::Touch* touch, cocos2d::Event* event);
    void onTouchCancelled(cocos2d::Touch* touch, cocos2d::Event* event);

    CREATE_FUNC(WelcomeScene);

    cocos2d::Size visibleSize;

    cocos2d::Sprite* cn;
    f32 _maxTime = 5;
    f32 animTime;
    f32 cnOpacity;
};

#endif