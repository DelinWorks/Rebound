#ifndef _GAMEPLAY_SCENE_H_
#define _GAMEPLAY_SCENE_H_

#include "cocos2d.h"
#include "shared_scenes/GameUtils.h"
#include "fmod/include_fmod.h"
#include "AppDelegate.h"
#include "Helper/win32_error.h"
#include "Helper/short_types.h"
#include "Entities/CatPlayer.h"
#include "Entities/Wall.h"
#include "Nodes/TiledMap.h"

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

    TiledMap* map;

    CREATE_FUNC(GameplayScene);

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
};

#endif