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
    F32 start = 0, end = 0;

    void onAdd();

    void update(F32 dt);

    FMODAudioLerpActionComponent(Node* _owner);

    FMODAudioLerpActionComponent* initComponent(FMOD::Channel* sound, F32 _duration, F32 _start, F32 _end);
};

#endif
