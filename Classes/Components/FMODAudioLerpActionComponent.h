#include <axmol.h>
#include "shared_scenes/GameUtils.h"

#ifndef __H_FMODAUDIOLERPACTIONCOMPONENT__
#define __H_FMODAUDIOLERPACTIONCOMPONENT__

class FMODAudioLerpActionComponent : public Component {
public:
    Node* owner;
    Node* actionNode;
    Sequence* action;
    FMOD::Channel* ref;
    f32 start, end;

    void onAdd();

    void update(f32 dt);

    FMODAudioLerpActionComponent(Node* _owner);

    FMODAudioLerpActionComponent* initComponent(FMOD::Channel* sound, f32 _duration, f32 _start, f32 _end);
};

#endif
