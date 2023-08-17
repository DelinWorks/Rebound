#include "DestroyNodeComponent.h"

DestroyNodeComponent::DestroyNodeComponent(F32 _time) {
    autorelease();
    setName(__func__);
    setEnabled(true);
    time_s = 0;
    time_e = _time;
}

void DestroyNodeComponent::update(F32 dt) {
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
