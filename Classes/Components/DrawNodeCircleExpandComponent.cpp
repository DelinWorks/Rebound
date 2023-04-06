#include "DrawNodeCircleExpandComponent.h"

DrawNodeCircleExpandComponent::DrawNodeCircleExpandComponent(f32 _time, f32 _amount, i32 _segs) {
    setName(__func__);
    setEnabled(true);
    r_start = 0;
    r_end = 1;
    time = 1.0 / _time;
    amount = _amount;
    segs = _segs;
}

void DrawNodeCircleExpandComponent::update(f32 dt) {
    ((DrawNode*)_owner)->clear();
    if (r_start < r_end)
        r_start += dt * time;
    if (r_start > r_end)
        r_start = 0;

    ((DrawNode*)_owner)->drawSolidCircle(Vec2::ZERO, r_start * amount,
        MATH_DEG_TO_RAD(0), segs, true, true, Color4B(255, 255, 255, (255 * (r_end - r_start)) / 10));
    ((DrawNode*)_owner)->drawCircle(Vec2::ZERO, r_start * amount,
        MATH_DEG_TO_RAD(0), segs, false, Color4B(255, 255, 255, (255 * (r_end - r_start))));
}

void DrawNodeCircleExpandComponent::onAdd()
{
}
