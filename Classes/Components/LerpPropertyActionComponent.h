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
    f32 start = 0, end = 0, finish = 0;
    i32 type = 0;

    void onAdd();

    void update(f32 dt);

    LerpPropertyActionComponent(Node* _owner);

    LerpPropertyActionComponent* initFloat(f32* _ref, f32 _duration, f32 _start, f32 _end, f32 _finish);
};

#endif
