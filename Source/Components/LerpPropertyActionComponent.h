#ifndef __H_LERPPROPERTYACTIONCOMPONENT__
#define __H_LERPPROPERTYACTIONCOMPONENT__

#include <axmol.h>
#include "shared_scenes/GameUtils.h"

class LerpPropertyActionComponent : public Component {
public:
    Node* owner = nullptr;
    Node* actionNode = nullptr;
    Sequence* action = nullptr;
    bool isBy = false;
    float* ref_float = nullptr;
    int* ref_int = nullptr;
    uint8_t* ref_uint8 = nullptr;
    F32 start = 0, end = 0, finish = 0;
    I32 type = 0;

    void onAdd();

    void update(F32 dt);

    LerpPropertyActionComponent(Node* _owner);

    LerpPropertyActionComponent* initFloat(F32* _ref, F32 _duration, F32 _start, F32 _end, F32 _finish);
};

#endif
