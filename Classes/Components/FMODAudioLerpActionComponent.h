#include <axmol.h>
#include "shared_scenes/GameUtils.h"

#ifndef __H_FMODAUDIOLERPACTIONCOMPONENT__
#define __H_FMODAUDIOLERPACTIONCOMPONENT__

class FMODAudioLerpActionComponent : public Component {
public:
    Node* owner = nullptr;
    Node* actionNode = nullptr;
    Sequence* action = nullptr;
    FMOD::Channel* ref = nullptr;
    f32 start = 0, end = 0;

    void onAdd();

    void update(f32 dt);

    FMODAudioLerpActionComponent(Node* _owner);

    FMODAudioLerpActionComponent* initComponent(FMOD::Channel* sound, f32 _duration, f32 _start, f32 _end);
};

#endif
