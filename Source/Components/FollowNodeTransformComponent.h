#include "axmol.h"
#include "Helper/short_types.h"

#ifndef __H_FOLLOWNODETRANSFORMCOMPONENT__
#define __H_FOLLOWNODETRANSFORMCOMPONENT__

#define LERP ax::MathUtil::lerp

class FollowNodeTransformComponent : public ax::Component {
public:
    ax::Node* target = nullptr;
    bool followPos = false, followScl = false, followRot = false;
    F32 lerpAmount = 0;

    FollowNodeTransformComponent(ax::Node* _target, bool _followPos = true, bool _followScl = true, bool _followRot = true, F32 _lerpAmount = -1)
        : target(_target), followPos(_followPos), followScl(_followScl), followRot(_followRot), lerpAmount(_lerpAmount) { }

    void update(F32 dt);
};

#endif
