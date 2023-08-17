#include <axmol.h>
#include "shared_scenes/GameUtils.h"

#ifndef __H_LERPFUNCCALLFLOATSHADERCOMPONENT__
#define __H_LERPFUNCCALLFLOATSHADERCOMPONENT__

class LerpFuncCallFloatShaderComponent : public Component {
public:
    std::function<void(ax::backend::ProgramState*, F32)> func;
    F32 current = 0, time = 0, start = 0, end = 0;
    ax::backend::ProgramState* shader = nullptr;

    LerpFuncCallFloatShaderComponent(ax::backend::ProgramState* _shader, std::function<void(ax::backend::ProgramState*, F32)> _func, F32 _time, F32 _start, F32 _end);

    void update(F32 dt);
};

#endif
