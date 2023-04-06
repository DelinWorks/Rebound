#ifndef __H_LERPPROPERTYACTIONCOMPONENT__
#define __H_LERPPROPERTYACTIONCOMPONENT__

#include <axmol.h>
#include "shared_scenes/GameUtils.h"

class LerpPropertyActionComponent : public Component {
public:
    Node* owner;
    Node* actionNode;
    Sequence* action;
    bool isBy = false;
    float* ref_float;
    int* ref_int;
    uint8_t* ref_uint8;
    f32 start, end, finish;
    i32 type;

    void onAdd();

    void update(f32 dt);

    LerpPropertyActionComponent(Node* _owner);

    LerpPropertyActionComponent* initFloat(f32* _ref, f32 _duration, f32 _start, f32 _end, f32 _finish);
};

#endif
