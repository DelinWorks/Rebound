#include "FollowNodeTransformComponent.h"

void FollowNodeTransformComponent::update(F32 dt)
{
    if (lerpAmount == -1)
    {
        if (followPos) _owner->setPosition(target->getPosition());
        if (followScl) _owner->setScale(target->getScale());
        if (followRot) _owner->setRotation3D(target->getRotation3D());
    }
    else
    {
        V2D posFrom = _owner->getPosition();
        V2D posTo = target->getPosition();
        posFrom.x = LERP(posFrom.x, posTo.x, lerpAmount);
        posFrom.y = LERP(posFrom.y, posTo.y, lerpAmount);
        if (followPos) _owner->setPosition(posFrom);
        if (followScl) _owner->setScale(target->getScale());
        if (followRot) _owner->setRotation3D(target->getRotation3D());
    }
}
