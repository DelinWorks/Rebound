#include "PhysicsWorld.h"

using namespace ReboundPhysics;

CollisionShape* ReboundPhysics::createRect(V2D pos, V2D size)
{
    CollisionShape* s = new CollisionShape();
    s->x = pos.x;
    s->y = pos.y;
    s->w = size.x;
    s->h = size.y;
    return s;
}

CollisionShape* ReboundPhysics::createSlope(V2D pos, F32 length, F32 base)
{
    CollisionShape* s = new CollisionShape();
    s->x = pos.x;
    s->y = pos.y;
    s->l = length;
    s->b = base;
    s->isTriangle = true;
    return s;
}

CollisionShape* ReboundPhysics::createRectDynamic(V2D pos, V2D size, F32 gravity)
{
    CollisionShape* s = new CollisionShape();
    s->x = pos.x;
    s->y = pos.y;
    s->w = size.x;
    s->h = size.y;
    s->gravity = gravity;
    return s;
}

bool ReboundPhysics::getCollisionShapeIntersect(const CollisionShape& s1, const CollisionShape& s2)
{
    return (s1.y <= s2.y + s2.h && s2.y <= s1.y + s1.h && s1.x <= s2.x + s2.w && s2.x <= s1.x + s1.w);
}

V2D ReboundPhysics::calculateRect2RectMTV(const CollisionShape& rect1, const CollisionShape& rect2)
{
    F32 dx = (rect1.x + rect1.w / 2) - (rect2.x + rect2.w / 2);
    F32 dy = (rect1.y + rect1.h / 2) - (rect2.y + rect2.h / 2);
    F32 combinedHalfWidths = rect1.w / 2 + rect2.w / 2;
    F32 combinedHalfHeights = rect1.h / 2 + rect2.h / 2;

    if (std::abs(dx) < combinedHalfWidths && std::abs(dy) < combinedHalfHeights) {
        F32 overlapX = combinedHalfWidths - std::abs(dx);
        F32 overlapY = combinedHalfHeights - std::abs(dy);

        if (overlapX >= overlapY) {
            if (rect1.y > rect2.y - rect1.h + VERTICAL_RESOLUTION_LEEWAY && rect1.y + VERTICAL_RESOLUTION_LEEWAY < rect2.y + rect2.h)
                overlapY = 0;

            if (dy > 0)
                return V2D(0, overlapY);
            else
                return V2D(0, -overlapY);
        }
        else {
            if (rect1.x > rect2.x - rect1.w + 1 && rect1.x + 1 < rect2.x + rect2.w)
                overlapX = 0;

            if (dx > 0)
                return V2D(overlapX, 0);
            else
                return V2D(-overlapX, 0);
        }
    }
    else return V2D::ZERO;
}

I32 ReboundPhysics::getLineOrientation(const V2D& a, const V2D& b, const V2D& c)
{
    I32 val = (b.y - a.y) * (c.x - b.x) - (b.x - a.x) * (c.y - b.y);
    if (val == 0)
        return 0;
    return (val > 0) ? 1 : 2;
}

bool ReboundPhysics::isLineSegment(const V2D& a, const V2D& c, const V2D& b)
{
    return (c.x <= MAX(a.x, b.x) && c.x >= MIN(a.x, b.x)) && (c.y <= MAX(a.y, b.y) && c.y >= MIN(a.y, b.y));
}

V2D ReboundPhysics::getIntersectionPoint(const V2D& a, const V2D& b, const V2D& c, const V2D& d)
{
    RayCastResult result{ false };
    F32 A1 = b.y - a.y;
    F32 B1 = a.x - b.x;
    F32 C1 = A1 * a.x + B1 * a.y;

    F32 A2 = d.y - c.y;
    F32 B2 = c.x - d.x;
    F32 C2 = A2 * c.x + B2 * c.y;

    F32 det = A1 * B2 - A2 * B1;
    if (std::abs(det) < 1e-9) {
        return V2D::ZERO;
    }
    else {
        return V2D((B2 * C1 - B1 * C2) / det, (A1 * C2 - A2 * C1) / det);
    }
}

RayCastResult ReboundPhysics::doLinesIntersect(const V2D& a, const V2D& b, const V2D& c, const V2D& d)
{
    RayCastResult result{ false };
    I32 o1 = getLineOrientation(a, b, c);
    I32 o2 = getLineOrientation(a, b, d);
    I32 o3 = getLineOrientation(c, d, a);
    I32 o4 = getLineOrientation(c, d, b);

    result.point = getIntersectionPoint(a, b, c, d);
    if (o1 != o2 && o3 != o4) {
        result.intersects = true;
        return result;
    }

    if (o1 == 0 && isLineSegment(a, c, b)) {
        result.intersects = true;
        return result;
    }
    if (o2 == 0 && isLineSegment(a, d, b)) {
        result.intersects = true;
        return result;
    }
    if (o3 == 0 && isLineSegment(c, a, d)) {
        result.intersects = true;
        return result;
    }
    if (o4 == 0 && isLineSegment(c, b, d)) {
        result.intersects = true;
        return result;
    }

    return result;
}

RayCastResult ReboundPhysics::doLineIntersectsRect(const V2D& p1, const V2D& p2, const CollisionShape& rect)
{
    RayCastResult result{ false };
    F32 minimumDistance = INFINITY;
    auto result1 = doLinesIntersect(p1, p2, V2D(rect.x, rect.y), V2D(rect.x + rect.w, rect.y));
    if (result1.intersects) {
        result.intersects = true;
        minimumDistance = p1.distance(result1.point);
        result.point = result1.point;
    }
    auto result2 = doLinesIntersect(p1, p2, V2D(rect.x, rect.y), V2D(rect.x, rect.y + rect.h));
    F32 dist = p1.distance(result2.point);
    if (result2.intersects && dist < minimumDistance) {
        result.intersects = true;
        minimumDistance = dist;
        result.point = result2.point;
    }
    auto result3 = doLinesIntersect(p1, p2, V2D(rect.x + rect.w, rect.y), V2D(rect.x + rect.w, rect.y + rect.h));
    dist = p1.distance(result3.point);
    if (result3.intersects && dist < minimumDistance) {
        result.intersects = true;
        minimumDistance = dist;
        result.point = result3.point;
    }

    auto result4 = doLinesIntersect(p1, p2, V2D(rect.x, rect.y + rect.h), V2D(rect.x + rect.w, rect.y + rect.h));
    dist = p1.distance(result4.point);
    if (result4.intersects && dist < minimumDistance) {
        result.intersects = true;
        minimumDistance = dist;
        result.point = result4.point;
    }
    return result;
}

RayCastResult ReboundPhysics::doLineIntersectsRects(const V2D& p1, const V2D& p2, std::vector<CollisionShape>& _shapeCollection)
{
    RayCastResult result{ false };
    bool didIntersect = false;
    V2D rayCastHitLocation;
    F32 minimumDistance = INFINITY;
    for (auto& _ : _shapeCollection) {
        result = doLineIntersectsRect(p1, p2, _);
        if (result.intersects) {
            didIntersect = true;
            if (p1.distanceSquared(result.point) < minimumDistance) {
                rayCastHitLocation = result.point;
                result.point = rayCastHitLocation;
                minimumDistance = p1.distanceSquared(result.point);
            }
        }
    }
    result.intersects = didIntersect;
    result.point = rayCastHitLocation;
    return result;
}

bool ReboundPhysics::getCollisionTriangleIntersect(const CollisionShape& r, const CollisionShape& t)
{
    // we perform the rect intersect test first for performance.
    //if (!getCollisionShapeIntersect(e, t1)) return false;

    return
        doLinesIntersect(V2D(r.x, r.y), V2D(r.x + r.w, r.y), V2D(t.x, t.y), V2D(t.x, t.y + t.l)).intersects ||
        doLinesIntersect(V2D(r.x, r.y), V2D(r.x + r.w, r.y), V2D(t.x, t.y + t.l), V2D(t.x + t.b, t.y)).intersects ||
        doLinesIntersect(V2D(r.x, r.y + r.h), V2D(r.x + r.w, r.y + r.h), V2D(t.x, t.y), V2D(t.x, t.y + t.l)).intersects ||
        doLinesIntersect(V2D(r.x, r.y + r.h), V2D(r.x + r.w, r.y + r.h), V2D(t.x, t.y + t.l), V2D(t.x + t.b, t.y)).intersects ||
        doLinesIntersect(V2D(r.x, r.y), V2D(r.x, r.y + r.h), V2D(t.x, t.y), V2D(t.x + t.b, t.y)).intersects ||
        doLinesIntersect(V2D(r.x, r.y), V2D(r.x, r.y + r.h), V2D(t.x, t.y + t.l), V2D(t.x + t.b, t.y)).intersects ||
        doLinesIntersect(V2D(r.x + r.w, r.y), V2D(r.x + r.w, r.y + r.h), V2D(t.x, t.y), V2D(t.x + t.b, t.y)).intersects ||
        doLinesIntersect(V2D(r.x + r.w, r.y), V2D(r.x + r.w, r.y + r.h), V2D(t.x, t.y + t.l), V2D(t.x + t.b, t.y)).intersects;
}

ResolveResult ReboundPhysics::resolveCollisionRect(CollisionShape& _, CollisionShape& __, const V2D& mtv)
{
    ResolveResult result{ false };

    F32 leeway = MAX(0, abs(_.vel.x) / 100);
    bool isWithinLeeway = _.x + _.w - leeway > __.x && __.x + __.w - leeway > _.x;

    if (mtv.y != 0 && isWithinLeeway) {
        result.isGrounded = _.gravity > 0 && mtv.y < 0 || _.gravity < 0 && mtv.y > 0;
        if (_.vel.y < -400999999)
            _.vel.y = abs(_.vel.y) / 2;
        else
            _.vel.y = 0;
    }

    if (mtv.y < 0 && _.vel.y > 0)
        _.vel.y = 0;

    if (_.y + _.h - VERTICAL_RESOLUTION_LEEWAY > __.y &&
        _.y + VERTICAL_RESOLUTION_LEEWAY < __.y + __.h)
    {
        if (mtv.x > 0 && _.vel.x < 0 || mtv.x < 0 && _.vel.x > 0) {
            _.vel.x = 0;
            _.lerp_vel.x = 0;
        }

        _.x += mtv.x;
    }

    if (isWithinLeeway)
        _.y += mtv.y;

    result.slopeAngle = 0.0f;
    return result;
}

CollisionShape ReboundPhysics::getTriangleEnvelop(const CollisionShape& triangle)
{
    if (!triangle.isTriangle) return triangle;

    CollisionShape result = triangle;
    result.w = triangle.b;
    result.h = triangle.l;

    if (triangle.b > 0 && triangle.l > 0) return result;

    if (triangle.b < 0) {
        result.x += triangle.b;
        result.w = -triangle.b;
    }
    if (triangle.l < 0) {
        result.y += triangle.l;
        result.h = -triangle.l;
    }

    return result;
}

ResolveResult ReboundPhysics::resolveCollisionSlope(CollisionShape& r, CollisionShape& t, bool isJumping, F32 verticalMtv)
{
    ResolveResult result{ false, false };
    auto e = getTriangleEnvelop(t);

    if (t.l > 0 && r.y + r.h / 2 < e.y || t.l < 0 && r.y + r.h / 2 > e.y + e.h)
        return result;

    if (r.x + r.w / 2 < e.x - 1 && t.b > 0 || r.x + r.w / 2 > e.x + e.w + 1 && t.b < 0) {
        return result;
    }

    F32 cornerSlope = abs(e.l / e.b);
    F32 cornerSlopeDiff = MAX(abs(e.l), abs(e.b)) / MIN(abs(e.l), abs(e.b));

    F32 incline = 0.0f;
    F32 forceUp = abs(r.vel.x) * cornerSlope;
    //if (abs(forceUp) < 3000) forceUp = 0.0f;
    bool isTop = false;
    auto resolveSlope = [&]() -> bool
    {
        if (t.l > 0 && incline > e.y + e.h) {
            incline = e.y + e.h;
            isTop = true;
        }

        if (t.l < 0 && incline < e.y - r.h) {
            incline = e.y - r.h;
            isTop = true;
        }

        F32 stickyness = isJumping || (NUM_SIGN(r.gravity) < 0 && t.l < 0) || (NUM_SIGN(r.gravity) > 0 && t.l > 0) ? 0 : 3;

        if (t.l > 0 && r.y < incline + stickyness || t.l < 0 && r.y > incline - stickyness) {
            r.vel.y = 0;

            if (isJumping && r.y > incline) {
                result.isGrounded = true;
                //return false;
            }
            if (t.l > 0 && verticalMtv < 0 || t.l < 0 && verticalMtv > 0) {
                r.x += abs(verticalMtv) / 2 * (e.b / e.l) * NUM_SIGN(t.l);
                r.vel.x = 0;
                r.lerp_vel.x = 0;
            }
            r.y += incline - r.y;
            if (e.b > 0 && r.vel.x < 0 || e.b < 0 && r.vel.x > 0)
                r.vel.y = forceUp * NUM_SIGN(t.l);
            else if (e.b > 0 && r.vel.x > 0 || e.b < 0 && r.vel.x < 0)
                r.vel.y = forceUp * -NUM_SIGN(t.l);
            result.isGrounded = true;
        }
        return true;
    };

    //alpha = alpha - 1;
    //return sqrt(1 - alpha * alpha);
    //F32 cornerSlopeCurve = circEaseOut(cornerSlope / 10) * 10;
    F32 tangent = Math::map_clamp_out(cornerSlope, 0, 8, 1, 0.13);
    F32 offset = r.h * cornerSlope / (M_PI / tangent);
    if (t.b > 0) {
        if (t.l > 0)
            incline = Math::map(r.x, e.x + e.b, e.x, e.y, e.y + e.l) - offset;
        else
            incline = Math::map(r.x - r.w / cornerSlope, e.x, e.x + e.b, t.y + e.l, t.y) + offset;
        if (!resolveSlope()) return result;
    }
    else if (t.b < 0) {
        if (t.l > 0)
            incline = Math::map(r.x + r.w, e.x - e.b, e.x, e.y + e.l, e.y) - offset;
        else
            incline = Math::map(r.x + r.w + r.w / cornerSlope, e.x - e.b, e.x, t.y + e.l, t.y) + offset;
        if (!resolveSlope()) return result;
    }

    result.isSlope = true;

    double theta_rad = std::atan(t.l / t.b);
    result.slopeAngle = theta_rad;

    if (isTop) result.slopeAngle = 0;

    r.slopeGround = &t;

    return result;
}

CollisionShape ReboundPhysics::getRectSweepEnvelope(const CollisionShape& rect1, const CollisionShape& rect2, F32 oExtend)
{
    F32 minX = std::min(rect1.x, rect2.x);
    F32 minY = std::min(rect1.y, rect2.y);
    F32 maxX = std::max(rect1.x + rect1.w, rect2.x + rect2.w);
    F32 maxY = std::max(rect1.y + rect1.h, rect2.y + rect2.h);

    CollisionShape boundingRect;
    boundingRect.x = minX - oExtend;
    boundingRect.y = minY - oExtend;
    boundingRect.w = maxX - minX + oExtend * 2;
    boundingRect.h = maxY - minY + oExtend * 2;

    return boundingRect;
}

I32 ReboundPhysics::getCCDPrecessionSteps(F32 velocityMagnitude)
{
    F32 minVelocity = 0.0f;
    F32 maxVelocity = 40000.0f;
    I32 minCCDPrecession = 5;
    I32 maxCCDPrecession = 50;

    // Calculate the CCD precession using linear interpolation
    F32 t = (velocityMagnitude - minVelocity) / (maxVelocity - minVelocity);
    F32 ccdPrecession = minCCDPrecession + t * (maxCCDPrecession - minCCDPrecession);

    return std::clamp<int>(ccdPrecession, 1, 200);
}

void ReboundPhysics::stepDynamic(CollisionShape& s, double delta, double fraction)
{
    s.vel.x = std::clamp<F32>(s.vel.x, -250000, 250000);
    s.vel.y = std::clamp<F32>(s.vel.y, -250000, 250000);

    s.x += s.vel.x * (fraction * delta);
    s.y += s.vel.y * (fraction * delta);
}
