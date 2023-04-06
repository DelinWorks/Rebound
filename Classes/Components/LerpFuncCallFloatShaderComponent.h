#include <axmol.h>
#include "shared_scenes/GameUtils.h"

#ifndef __H_LERPFUNCCALLFLOATSHADERCOMPONENT__
#define __H_LERPFUNCCALLFLOATSHADERCOMPONENT__

class LerpFuncCallFloatShaderComponent : public Component {
public:
    std::function<void(ax::backend::ProgramState*, f32)> func;
    f32 current, time, start, end;
    ax::backend::ProgramState* shader;

    LerpFuncCallFloatShaderComponent(ax::backend::ProgramState* _shader, std::function<void(ax::backend::ProgramState*, f32)> _func, f32 _time, f32 _start, f32 _end);

    void update(f32 dt);
};

#endif