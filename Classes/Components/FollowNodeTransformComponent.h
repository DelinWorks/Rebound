#include "axmol.h"
#include "Helper/short_types.h"

#define LERP ax::MathUtil::lerp

class FollowNodeTransformComponent : public ax::Component {
public:
    ax::Node* target;
    bool followPos, followScl, followRot;
    f32 lerpAmount;

    FollowNodeTransformComponent(ax::Node* _target, bool _followPos = true, bool _followScl = true, bool _followRot = true, f32 _lerpAmount = -1)
        : target(_target), followPos(_followPos), followScl(_followScl), followRot(_followRot), lerpAmount(_lerpAmount) { }

    void update(f32 dt);
};