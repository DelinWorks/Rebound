#ifndef _GAMEPLAY_SCENE_H_
#define _GAMEPLAY_SCENE_H_

#include "cocos2d.h"
#include "shared_scenes/GameUtils.h"
#include "fmod/include_fmod.h"
#include "AppDelegate.h"
#include "Helper/win32_error.h"
#include "Helper/short_types.h"
#include "Entities/CatPlayer.h"
#include "Nodes/TiledMap.h"
#include "Nodes/PhysicsWorld.h"

class GameplayScene : public cocos2d::Scene
{
public:
    static cocos2d::Scene* createScene();

    virtual bool init();

    CatPlayer* p;

    float elapsedDt;
    void awake();
    void update(f32 dt);
    
    ax::ui::Slider* slider;
    ax::backend::ProgramState* ps1;
    ax::MeshRenderer* renderer;

    ax::Label* lb;

    float currentPhysicsDt = 0;
    float lastPhysicsDt = 0;

    ax::TMXLayer* layerSolid;

    DarknessPhysicsWorld* physics;
    TiledMap* map;

    CREATE_FUNC(GameplayScene);

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
};

#endif