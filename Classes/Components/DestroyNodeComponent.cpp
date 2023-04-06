#include "DestroyNodeComponent.h"

DestroyNodeComponent::DestroyNodeComponent(f32 _time) {
    setName(__func__);
    setEnabled(true);
    time_s = 0;
    time_e = _time;
}

void DestroyNodeComponent::update(f32 dt) {
    if (time_s < time_e)
        time_s += dt;
    else
    {
        _owner->removeFromParentAndCleanup(true);
        setEnabled(false);
    }
}

void DestroyNodeComponent::onAdd()
{
}
