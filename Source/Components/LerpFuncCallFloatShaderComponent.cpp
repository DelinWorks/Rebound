#include "LerpFuncCallFloatShaderComponent.h"

LerpFuncCallFloatShaderComponent::LerpFuncCallFloatShaderComponent(ax::backend::ProgramState* _shader, std::function<void(ax::backend::ProgramState*, F32)> _func, F32 _time, F32 _start, F32 _end) : func(_func), shader(_shader), current(0), time(_time), start(_start), end(_end)
{
    setName(__func__);
    setEnabled(true);
    current = start;
}

void LerpFuncCallFloatShaderComponent::update(F32 dt) {
    float exp = dt * time;
    current = current + (start > end ? -exp : exp);
    if (end > start && current > end)
    {
        func(shader, end);
        _owner->removeComponent(this);
        return;
    }
    if (start > end && current < end)
    {
        func(shader, end);
        _owner->removeComponent(this);
        return;
    }
    func(shader, current);
}
