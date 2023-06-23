#include "LerpFuncCallFloatShaderComponent.h"

LerpFuncCallFloatShaderComponent::LerpFuncCallFloatShaderComponent(ax::backend::ProgramState* _shader, std::function<void(ax::backend::ProgramState*, f32)> _func, f32 _time, f32 _start, f32 _end) : func(_func), shader(_shader), current(0), time(_time), start(_start), end(_end)
{
    setName(__func__);
    setEnabled(true);
    current = start;
}

void LerpFuncCallFloatShaderComponent::update(f32 dt) {
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
