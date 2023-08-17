#ifndef _EMPTY_SCENE_H_
#define _EMPTY_SCENE_H_

#include "cocos2d.h"
#include "shared_scenes/GameUtils.h"
#include "fmod/include_fmod.h"
#include "AppDelegate.h"
#include "Helper/win32_error.h"
#include "Helper/short_types.h"

class EmptyScene : public cocos2d::Scene
{
public:
    static cocos2d::Scene* createScene();

    virtual bool init();

    void update(F32 dt);

    CREATE_FUNC(EmptyScene);
};

#endif