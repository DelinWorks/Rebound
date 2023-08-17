#ifndef _WELCOME_SCENE_H_
#define _WELCOME_SCENE_H_

#include "axmol.h"
#include "shared_scenes/GameUtils.h"
#include "fmod/include_fmod.h"
#include "AppDelegate.h"
#include "EmptyScene.h"
#include "GameplayScene.h"
#include "Helper/win32_error.h"
#include "Helper/short_types.h"
#include "Components/Components.h"

class WelcomeScene : public ax::Scene, public SceneInputManager
{
public:
    static ax::Scene* createScene();

    void update(F32 dt);
    void runNext();
    void rebuildEntireUi();
    void lateUpdate(F32 dt);

    virtual bool init();
    bool isInitDone = false;
    void onInitDone();

    ax::EventListenerKeyboard* listener;
    ax::EventListenerMouse* _mouseListener;

    void onKeyPressed(ax::EventKeyboard::KeyCode keyCode, ax::Event* event);
    void onKeyReleased(ax::EventKeyboard::KeyCode keyCode, ax::Event* event);
    void onKeyHold(ax::EventKeyboard::KeyCode keyCode, ax::Event* event);
    void onMouseDown(ax::Event* event);
    void onMouseUp(ax::Event* event);
    void onMouseMove(ax::Event* event);
    void onMouseScroll(ax::Event* event);
    bool onTouchBegan(ax::Touch* touch, ax::Event* event);
    void onTouchMoved(ax::Touch* touch, ax::Event* event);
    void onTouchEnded(ax::Touch* touch, ax::Event* event);
    void onTouchCancelled(ax::Touch* touch, ax::Event* event);

    CREATE_FUNC(WelcomeScene);

    S2D visibleSize;

    ax::Sprite* cn;
    F32 _maxTime = 5;
    F32 animTime;
    F32 cnOpacity;
};

#endif