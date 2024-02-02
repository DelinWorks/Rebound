#include "PhysicsWorld.h"

void ReboundPhysics::setChunkTier(CollisionShape* shape)
{
    CollisionShape tri;
    if (shape->isTriangle)
    {
        tri = getTriangleEnvelop(*shape);
        shape->chunkTier = 0;
        if (tri.w > PHYS_CHUNK_TIER3_SIZE.x || tri.h > PHYS_CHUNK_TIER3_SIZE.y)
            shape->chunkTier = 4;
        else if (tri.w > PHYS_CHUNK_TIER2_SIZE.x || tri.h > PHYS_CHUNK_TIER2_SIZE.y)
            shape->chunkTier = 3;
        else if (tri.w > PHYS_CHUNK_TIER1_SIZE.x || tri.h > PHYS_CHUNK_TIER1_SIZE.y)
            shape->chunkTier = 2;
        else if (tri.w > PHYS_CHUNK_TIER0_SIZE.x || tri.h > PHYS_CHUNK_TIER0_SIZE.y)
            shape->chunkTier = 1;
        return;
    }

    shape->chunkTier = 0;
    if (shape->w > PHYS_CHUNK_TIER3_SIZE.x || shape->h > PHYS_CHUNK_TIER3_SIZE.y)
        shape->chunkTier = 4;
    else if (shape->w > PHYS_CHUNK_TIER2_SIZE.x || shape->h > PHYS_CHUNK_TIER2_SIZE.y)
        shape->chunkTier = 3;
    else if (shape->w > PHYS_CHUNK_TIER1_SIZE.x || shape->h > PHYS_CHUNK_TIER1_SIZE.y)
        shape->chunkTier = 2;
    else if (shape->w > PHYS_CHUNK_TIER0_SIZE.x || shape->h > PHYS_CHUNK_TIER0_SIZE.y)
        shape->chunkTier = 1;
}

V2D ReboundPhysics::getChunkTierSize(U8 tier)
{
    switch (tier)
    {
    default:
        return PHYS_CHUNK_TIER0_SIZE;
        break;
    case 1:
        return PHYS_CHUNK_TIER1_SIZE;
        break;
    case 2:
        return PHYS_CHUNK_TIER2_SIZE;
        break;
    case 3:
        return PHYS_CHUNK_TIER3_SIZE;
        break;
    case 4:
        return PHYS_CHUNK_TIER4_SIZE;
        break;
    }
}

ReboundPhysics::CollisionShape* ReboundPhysics::createRect(V2D pos, V2D size)
{
    CollisionShape* s = new CollisionShape();
    s->x = pos.x;
    s->y = pos.y;
    s->w = size.x;
    s->h = size.y;
    setChunkTier(s);
    return s;
}

ReboundPhysics::CollisionShape* ReboundPhysics::createSlope(V2D pos, F32 length, F32 base)
{
    CollisionShape* s = new CollisionShape();
    s->x = pos.x;
    s->y = pos.y;
    s->l = length;
    s->b = base;
    s->isTriangle = true;
    setChunkTier(s);
    return s;
}

ReboundPhysics::DynamicCollisionShape* ReboundPhysics::createRectDynamic(V2D pos, V2D size, F32 gravity)
{
    DynamicCollisionShape* s = new DynamicCollisionShape();
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

V2D ReboundPhysics::calculateRect2RectMTV(const DynamicCollisionShape& rect1, const CollisionShape& rect2, bool both, F32 delta)
{
    F32 dx = ((rect1.x - 0) + (rect1.w + 0) / 2) - (rect2.x + rect2.w / 2);
    F32 dy = ((rect1.y - 0) + (rect1.h + 0) / 2) - (rect2.y + rect2.h / 2);
    F32 combinedHalfWidths = (rect1.w + 0) / 2 + rect2.w / 2;
    F32 combinedHalfHeights = (rect1.h + 0) / 2 + rect2.h / 2;

    if (abs(dx) < combinedHalfWidths && abs(dy) < combinedHalfHeights) {
        F32 overlapX = combinedHalfWidths - abs(dx);
        F32 overlapY = combinedHalfHeights - abs(dy);

        bool vTY = rect1.y + VERTICAL_RESOLUTION_LEEWAY >= rect2.y + rect2.h;
        bool vBY = rect1.y + rect1.h - VERTICAL_RESOLUTION_LEEWAY <= rect2.y;

        F32 leeway = MIN(4, abs(rect1.hSpeed) * 5);
        bool isWithinLeeway = (rect1.x + rect1.w - leeway > rect2.x) && (rect2.x + rect2.w - leeway > rect1.x);

        if (both)
            return V2D(dx > 0 ? overlapX : -overlapX, dy > 0 ? overlapY : -overlapY);
        else if (overlapX >= overlapY || vTY || vBY)
        {
            if (vTY && rect1.vel.y > 0 && !isWithinLeeway) if (dx > 0) return V2D(overlapX, 0); else return V2D(-overlapX, 0);
            if (vBY && rect1.vel.y < 0 && !isWithinLeeway) if (dx > 0) return V2D(overlapX, 0); else return V2D(-overlapX, 0);

            if (dy > 0)
                return V2D(0, overlapY);
            else
                return V2D(0, -overlapY);
        }
        else {
            if (dx > 0) return V2D(overlapX, 0); else return V2D(-overlapX, 0);
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

ReboundPhysics::RayCastResult ReboundPhysics::doLinesIntersect(const V2D& a, const V2D& b, const V2D& c, const V2D& d)
{
    RayCastResult result{ false };
    I32 o1 = ReboundPhysics::getLineOrientation(a, b, c);
    I32 o2 = ReboundPhysics::getLineOrientation(a, b, d);
    I32 o3 = ReboundPhysics::getLineOrientation(c, d, a);
    I32 o4 = ReboundPhysics::getLineOrientation(c, d, b);

    result.point = ReboundPhysics::getIntersectionPoint(a, b, c, d);
    if (o1 != o2 && o3 != o4) {
        result.intersects = true;
        return result;
    }

    if (o1 == 0 && ReboundPhysics::isLineSegment(a, c, b)) {
        result.intersects = true;
        return result;
    }
    if (o2 == 0 && ReboundPhysics::isLineSegment(a, d, b)) {
        result.intersects = true;
        return result;
    }
    if (o3 == 0 && ReboundPhysics::isLineSegment(c, a, d)) {
        result.intersects = true;
        return result;
    }
    if (o4 == 0 && ReboundPhysics::isLineSegment(c, b, d)) {
        result.intersects = true;
        return result;
    }

    return result;
}

ReboundPhysics::RayCastResult ReboundPhysics::doLineIntersectsRect(const V2D& p1, const V2D& p2, const CollisionShape& rect)
{
    RayCastResult result{ false };
    F32 minimumDistance = INFINITY;
    auto result1 = ReboundPhysics::doLinesIntersect(p1, p2, V2D(rect.x, rect.y), V2D(rect.x + rect.w, rect.y));
    if (result1.intersects) {
        result.intersects = true;
        minimumDistance = p1.distance(result1.point);
        result.point = result1.point;
    }
    auto result2 = ReboundPhysics::doLinesIntersect(p1, p2, V2D(rect.x, rect.y), V2D(rect.x, rect.y + rect.h));
    F32 dist = p1.distance(result2.point);
    if (result2.intersects && dist < minimumDistance) {
        result.intersects = true;
        minimumDistance = dist;
        result.point = result2.point;
    }
    auto result3 = ReboundPhysics::doLinesIntersect(p1, p2, V2D(rect.x + rect.w, rect.y), V2D(rect.x + rect.w, rect.y + rect.h));
    dist = p1.distance(result3.point);
    if (result3.intersects && dist < minimumDistance) {
        result.intersects = true;
        minimumDistance = dist;
        result.point = result3.point;
    }

    auto result4 = ReboundPhysics::doLinesIntersect(p1, p2, V2D(rect.x, rect.y + rect.h), V2D(rect.x + rect.w, rect.y + rect.h));
    dist = p1.distance(result4.point);
    if (result4.intersects && dist < minimumDistance) {
        result.intersects = true;
        minimumDistance = dist;
        result.point = result4.point;
    }
    return result;
}

ReboundPhysics::RayCastResult ReboundPhysics::doLineIntersectsRects(const V2D& p1, const V2D& p2, std::vector<CollisionShape>& _shapeCollection)
{
    RayCastResult result{ false };
    bool didIntersect = false;
    V2D rayCastHitLocation;
    F32 minimumDistance = INFINITY;
    for (auto& _ : _shapeCollection) {
        result = ReboundPhysics::doLineIntersectsRect(p1, p2, _);
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
        ReboundPhysics::doLinesIntersect(V2D(r.x, r.y), V2D(r.x + r.w, r.y), V2D(t.x, t.y), V2D(t.x, t.y + t.l)).intersects ||
        ReboundPhysics::doLinesIntersect(V2D(r.x, r.y), V2D(r.x + r.w, r.y), V2D(t.x, t.y + t.l), V2D(t.x + t.b, t.y)).intersects ||
        ReboundPhysics::doLinesIntersect(V2D(r.x, r.y + r.h), V2D(r.x + r.w, r.y + r.h), V2D(t.x, t.y), V2D(t.x, t.y + t.l)).intersects ||
        ReboundPhysics::doLinesIntersect(V2D(r.x, r.y + r.h), V2D(r.x + r.w, r.y + r.h), V2D(t.x, t.y + t.l), V2D(t.x + t.b, t.y)).intersects ||
        ReboundPhysics::doLinesIntersect(V2D(r.x, r.y), V2D(r.x, r.y + r.h), V2D(t.x, t.y), V2D(t.x + t.b, t.y)).intersects ||
        ReboundPhysics::doLinesIntersect(V2D(r.x, r.y), V2D(r.x, r.y + r.h), V2D(t.x, t.y + t.l), V2D(t.x + t.b, t.y)).intersects ||
        ReboundPhysics::doLinesIntersect(V2D(r.x + r.w, r.y), V2D(r.x + r.w, r.y + r.h), V2D(t.x, t.y), V2D(t.x + t.b, t.y)).intersects ||
        ReboundPhysics::doLinesIntersect(V2D(r.x + r.w, r.y), V2D(r.x + r.w, r.y + r.h), V2D(t.x, t.y + t.l), V2D(t.x + t.b, t.y)).intersects;
}

ReboundPhysics::ResolveResult ReboundPhysics::resolveCollisionRect(DynamicCollisionShape& _, CollisionShape& __, F32 delta, const V2D& mtv, bool ignoreVL)
{
    ResolveResult result{ false };

    F32 leeway = MIN(4, abs(_.hSpeed) * 5);
    bool isWithinLeeway = (_.x + _.w - leeway > __.x) && (__.x + __.w - leeway > _.x);

    if (mtv.y != 0 && isWithinLeeway) {
        result.isGrounded = _.gravity > 0 && mtv.y < 0 || _.gravity < 0 && mtv.y > 0;
        if (_.vel.y < -400999999)
            _.vel.y = abs(_.vel.y) / 2;
        else {
            if (mtv.y < 0 && _.vel.y > 0 || mtv.y > 0 && _.vel.y < 0) {
                _.vel.y = 0;
                _.ny = 0.0f;
            }
        }
    }
    _.x += mtv.x;

    if (isWithinLeeway) {
        _.y += mtv.y;
        if (mtv.y != 0)
            _.ny = 0.0f;
    }

    if (mtv.x > 0 && _.vel.x < 0 || mtv.x < 0 && _.vel.x > 0) {
        _.vel.x = 0;
        _.lerp_vel.x = 0;
        _.nx = 0.0f;
        if (!__.isMovable)
            _.relativeMovableForce.x = 0;
    }

    _.internalAngle = 0.0f;
    return result;
}

ReboundPhysics::CollisionShape ReboundPhysics::getTriangleEnvelop(const CollisionShape& triangle)
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

ReboundPhysics::ResolveResult ReboundPhysics::resolveCollisionSlope(DynamicCollisionShape& r, CollisionShape& t, bool isJumping, F32 verticalMtv, F32 delta)
{
    ResolveResult result;
    auto e = ReboundPhysics::getTriangleEnvelop(t);

    if (t.b > 0 && r.x + r.w / 4 > t.x + e.b || t.b < 0 && r.x + r.w / 1.5 < t.x + e.b)
        result.isSlopeOutsideH = true;

    if (t.l > 0 && r.y + r.h > t.y + t.l || t.l < 0 && r.y < t.y + t.l)
        result.isSlopeOutsideV = true;

    if (t.l > 0 && r.y + r.h / 2 < e.y || t.l < 0 && r.y + r.h / 2 > e.y + e.h)
        return result;

    if (r.x + r.w / 2 < e.x - 1 && t.b > 0 || r.x + r.w / 2 > e.x + e.w + 1 && t.b < 0) {
        return result;
    }

    F32 cornerSlope = abs(e.l / e.b);
    F32 cornerSlopeDiff = MAX(abs(e.l), abs(e.b)) / MIN(abs(e.l), abs(e.b));
    F32 cornerAngle = atan2(abs(e.l), abs(e.b));

    F32 incline = 0.0f;
    F32 forceUp = abs(r.vel.x) * cornerSlope * std::clamp<F32>(r.timeSpentOnSlope, 0, 1);
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

        if (t.b > 0 && r.x + r.w / 4 > t.x + e.b || t.b < 0 && r.x + r.w / 1.5 < t.x + e.b)
            if (abs(e.l) <= abs(e.b))
                return false;

        F32 stickyness = abs(r.vel.y) > abs(forceUp) || (NUM_SIGN(r.gravity) < 0 && t.l < 0) || (NUM_SIGN(r.gravity) > 0 && t.l > 0) ? 0 : 3;

        if (t.l > 0 && r.y < incline || t.l < 0 && r.y > incline)
        {
            if (NUM_SIGN(r.gravity) < 0 && r.vel.y < 0)
                r.vel.y = 0;
            else if (NUM_SIGN(r.gravity) > 0 && r.vel.y > 0)
                r.vel.y = 0;

            double theta_rad = std::atan(t.l / t.b);
            r.internalAngle = theta_rad;

            if (isTop) r.internalAngle = 0;

            if (isJumping && r.y > incline)
                result.isGrounded = true;

            if (t.l > 0 && verticalMtv < 0 || t.l < 0 && verticalMtv > 0) {
                r.x += abs(verticalMtv) / 2 * (e.b / e.l) * NUM_SIGN(t.l);
                r.vel.x = 0;
                r.lerp_vel.x = 0;
            }

            result.verticalMTV = r.y < incline ? incline - r.y : 0;

            r.y += incline - r.y;

            if (!isTop)
            {
                if (e.b > 0 && r.vel.x < 0 || e.b < 0)
                    r.vel.y = forceUp * NUM_SIGN(t.l);
                if (e.b > 0 && r.vel.x > 0 || e.b < 0 && r.vel.x < 0)
                    r.vel.y = forceUp * -NUM_SIGN(t.l);

                //if (sin(cornerAngle) > sin(AX_DEGREES_TO_RADIANS(60)))
                //{
                //    result.isSlopeGroundSteep = true;
                //    if (r.vel.x < 0 && t.b > 0 || r.vel.x > 0 && t.b < 0) r.vel = V2D(LERP(r.vel.x, 0, 20 * delta), 0);
                //    r.vel.x += 200 * delta * NUM_SIGN(t.b) * MIN(MAX(1, r.timeSpentOnSlope) * 300, 20);
                //}
            }

            result.isGrounded = true;
        }

        return true;
    };

    r.slopeGround = &t;
    result.isSlope = true;

    F32 offset = sin(cornerAngle) * (r.h / 2);
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

    return result;
}

ReboundPhysics::CollisionShape ReboundPhysics::getRectSweepEnvelope(const CollisionShape& rect1, const CollisionShape& rect2, F32 oExtend)
{
    F32 minX = MIN(rect1.x, rect2.x);
    F32 minY = MIN(rect1.y, rect2.y);
    F32 maxX = MAX(rect1.x + rect1.w, rect2.x + rect2.w);
    F32 maxY = MAX(rect1.y + rect1.h, rect2.y + rect2.h);

    CollisionShape boundingRect;
    boundingRect.x = minX - oExtend / 2;
    boundingRect.y = minY - oExtend / 2;
    boundingRect.w = maxX - minX + oExtend;
    boundingRect.h = maxY - minY + oExtend;

    return boundingRect;
}

I32 ReboundPhysics::getCCDPrecessionSteps(F32 velocityMagnitude)
{
    F32 minVelocity = 0.0f;
    F32 maxVelocity = 40000.0f;
    I32 minCCDPrecession = 5;
    I32 maxCCDPrecession = 50;

    F32 t = (velocityMagnitude - minVelocity) / (maxVelocity - minVelocity);
    F32 ccdPrecession = minCCDPrecession + t * (maxCCDPrecession - minCCDPrecession);

    return std::clamp<int>(ccdPrecession, 1, 200);
}

void ReboundPhysics::stepDynamic(DynamicCollisionShape& s, F32 fraction)
{
    s.x += s.nx * fraction;
    s.y += s.ny * fraction;
}

void ReboundPhysics::setBodyPosition(CollisionShape& s, V2D newPos, bool sweep)
{
    if (sweep)
    {
        s.nx = newPos.x - s.x;
        s.ny = newPos.y - s.y;
    }
    else
    {
        s.x = newPos.x;
        s.y = newPos.y;
    }
}

I32 ReboundPhysics::chunkGetCoverArea(V2DH* array, CollisionShape s, bool oneChunk)
{
    I32 startX = 0;
    I32 startY = 0;
    I32 endX = 0;
    I32 endY = 0;

    if (s.isTriangle)
        s = getTriangleEnvelop(s);

    if (oneChunk)
    {
        V2D chunkSize = getChunkTierSize(s.chunkTier);

        startX = s.x + s.w / 2;
        startY = s.y + s.h / 2;
        endX = (startX >= 0) ? startX / chunkSize.x : (startX - chunkSize.x + 1) / chunkSize.x;
        endY = (startY >= 0) ? startY / chunkSize.y : (startY - chunkSize.y + 1) / chunkSize.y;
        array[0] = V2DH(endX, endY);
        return 1;
    }
    else
    {
        V2D chunkSize = getChunkTierSize(s.chunkTier);

        startX = (s.x >= 0) ? s.x / chunkSize.x : (s.x - chunkSize.x + 1) / chunkSize.x;
        startY = (s.y >= 0) ? s.y / chunkSize.y : (s.y - chunkSize.y + 1) / chunkSize.y;
        endX = (s.x >= 0 && MAX(5, s.w) < chunkSize.x || s.x > -MAX(5, s.w) && MAX(5, s.w) >= chunkSize.x)
            ? (s.x + MAX(5, s.w) - 1) / chunkSize.x : (s.x - chunkSize.x + MAX(5, s.w) - 1) / chunkSize.x;
        endY = (s.y >= 0 && MAX(5, s.h) < chunkSize.y || s.y > -MAX(5, s.h) && MAX(5, s.h) >= chunkSize.y)
            ? (s.y + MAX(5, s.h) - 1) / chunkSize.y : (s.y - chunkSize.y + MAX(5, s.h) - 1) / chunkSize.y;

        int count = 0;
        for (I32 x = startX; x <= endX; x++) {
            for (I32 y = startY; y <= endY; y++) {
                if (count >= CHUNK_MAX_TARGETS) break;
                array[count++] = V2DH(x, y);
            }
        }

        return count;
    }
}

void ReboundPhysics::setShapePosition(PhysicsWorld* worldToRegisterSweep, CollisionShape& shape, const V2D& newPos)
{
    if (worldToRegisterSweep)
    {
        auto w = worldToRegisterSweep;
        w->_moveTargets[w->_moveTargetCount++] = &shape;
    }

    shape.dx = shape.x;
    shape.dy = shape.y;
    shape.nx = newPos.x;
    shape.ny = newPos.y;
}

void ReboundPhysics::PhysicsWorld::move(F64 subdt)
{
    for (U32 i = 0; i < _moveTargetCount; i++)
    {
        auto& _ = _moveTargets[i];

        _->x = LERP(_->dx, _->nx, subdt);
        _->y = LERP(_->dy, _->ny, subdt);

        chunkGetCoverArea(_coveredChunksBuffers[1], *_, true);
        V2DH oldChunkPos  = _->currentChunk;
        V2DH newChunkPos = _coveredChunksBuffers[1][0];

        if (oldChunkPos != newChunkPos)
        {
            auto& __ = chunks[_];
            auto& oldChunk = getChunkTierMap(_->chunkTier)[oldChunkPos];
            auto& newChunk = getChunkTierMap(_->chunkTier)[newChunkPos];
            oldChunk.erase_index(__[&oldChunk]);
            __.erase(&oldChunk);
            __[&newChunk] = newChunk.shove(_);
            _->currentChunk = newChunkPos;
        }
    }
}

void ReboundPhysics::PhysicsWorld::step(F64 delta)
{
    stepElapsedDelta += delta;

    for (auto& _ : _dynamicShapes)
    {
        if (isJumping && isGrounded && !_->isSlopeGroundSteep)
            _->vel.y = 2000 * -NUM_SIGN(_->gravity);

        if (delta == 0) return;

        if (!_->isSlopeGroundSteep)
            _->vel.x = MathUtil::lerp(_->vel.x, _->pref_vel.x, 5 * delta);

        _->slopeGround = nullptr;

        _->vel.x = std::clamp<F32>(_->vel.x, -100000, 100000);
        _->vel.y = std::clamp<F32>(_->vel.y, -100000, 100000);

        _->nx += _->vel.x * delta;
        _->ny += _->vel.y * delta;

        _->hSpeed = _->nx;

        if (_->movableGround)
        {
            V2D oldPos = _->movableGroundPos;
            V2D newPos = V2D(_->movableGround->x, _->movableGround->y);
            V2D size = V2D(_->movableGround->w, _->movableGround->h);
            _->movableGroundPos = newPos;

            if (!getCollisionShapeIntersect(*_, CollisionShape(newPos.x - 1, newPos.y - 1, size.width + 2, size.height + 2)))
                _->movableGround = nullptr;

            _->relativeMovableForce.x = LERP(_->relativeMovableForce.x, (newPos.x - oldPos.x) / delta, MIN(1.0, 10 * delta));
            _->relativeMovableForce.y = LERP(_->relativeMovableForce.y, (newPos.y - oldPos.y) / delta, MIN(1.0, 10 * delta));
        }

        _->x += _->relativeMovableForce.x * delta;
        _->y += _->relativeMovableForce.y * delta;

        isGrounded = false;
        _->timeSpentOnSlope += delta * 32;

        {
            int size = 0;
            CollisionShape* targets[CCD_MAX_TARGETS];
            {
                float ox = _->x;
                float oy = _->y;
                float nx = ox + _->nx;
                float ny = oy + _->ny;

                for (U8 t = 0; t < 5; t++)
                {
                    V2D chunkSize = getChunkTierSize(t);
                    float envExtent = (chunkSize.x + chunkSize.y) / 2;
                    envExtent *= 2;

                    CollisionShape oldPos = CollisionShape(ox, oy, _->w, _->h);
                    CollisionShape newPos = CollisionShape(nx, ny, _->w, _->h);
                    envelope = getRectSweepEnvelope(oldPos, newPos, envExtent);

                    if (_extraDraw.size() < 6)
                        _extraDraw.push_back({
                            {envelope.x, envelope.y},
                            {envelope.x + envelope.w, envelope.y + envelope.h},
                            Color4B::RED
                            });

                    envelope.chunkTier = t;
                    I32 count = chunkGetCoverArea(_coveredChunksBuffers[0], envelope, false);

                    for (I32 i = 0; i < count; i++) {
                        auto& vec = getChunkTierMap(t)[_coveredChunksBuffers[0][i]];
                        for (int i = 0; i < vec.size(); i++) {
                            CollisionShape* __ = vec.get(i);
                            if (__ == 0) continue;
                            CollisionShape rect = *__;
                            if (rect.isTriangle)
                                rect = getTriangleEnvelop(*__);
                            if (getCollisionShapeIntersect(rect, envelope)) {
                                if (size >= CCD_MAX_TARGETS) break;
                                targets[size++] = __;
                            }
                        }
                    }
                }

                CollisionShape oldPos = CollisionShape(ox, oy, _->w, _->h);
                CollisionShape newPos = CollisionShape(nx, ny, _->w, _->h);
                envelope = getRectSweepEnvelope(oldPos, newPos, 32);
            }

            int steps = 0;
            _->vel.y += _->gravity * delta * !isGrounded;

            float sweptVolumePrec = 1;

            double ccdPrecession = CCD_STEPS_TO_PERC(MAX(1,
                MAX(envelope.w * sweptVolumePrec - _->w - 32 * 2,
                    envelope.h * sweptVolumePrec - _->h - 32 * 2)));

            ccdPrecession = MAX(0.1, ccdPrecession);

            bool isSlope = false;
            float slopeIncline = 1.0f;
            float lastVerticalMtv = 0.0f;
            double step_perc = 1.0;

            double debug1 = 0.0f;
            if (size == 0) {
                stepDynamic(*_, 1);
                debug1 += 1;
                step_perc -= step_perc;
            }
            else
                for (;;)
                {
                    float ox = _->x;
                    float oy = _->y;

                    bool isSlopeOccupied = false;
                    bool applyDominantSlope = false;
                    CollisionShape slopeMtvState;
                    V2D slopeMtvStateVec;

                    bool quitLater = false;
                    if (step_perc > 0.0) {
                        float ccdPrecessionT = ccdPrecession;
                        if (step_perc - ccdPrecession < 0) ccdPrecessionT = step_perc;
                        stepDynamic(*_, ccdPrecessionT);
                        debug1 += ccdPrecessionT;
                        step_perc -= ccdPrecessionT;
                        if (step_perc <= 0) quitLater = true;
                    }

                    for (int i = 0; i < size; i++)
                        targets[i]->hasObjectCollidedChunk = false;

                    for (int i = 0; i < size; i++)
                    {
                        if (targets[i]->hasObjectCollidedChunk) continue;

                        bool intersects = getCollisionShapeIntersect(*_, *targets[i]);

                        if (intersects && targets[i]->isTrigger) continue;

                        if (targets[i]->isTriangle)
                        {
                            if (getCollisionTriangleIntersect(*_, *targets[i])) {
                                auto r = resolveCollisionSlope(*_, *targets[i], isJumping, lastVerticalMtv, delta);
                                lastVerticalMtv = r.verticalMTV;
                                isGrounded = r.isGrounded || isGrounded;
                                if (intersects && targets[i]->isMovable && isGrounded) {
                                    _->movableGround = targets[i];
                                    _->movableGroundPos = V2D(targets[i]->x, targets[i]->y);
                                }

                                if (r.isGrounded) _->isSlopeGroundSteep = r.isSlopeGroundSteep;
                                if (!r.isSlope) {
                                    if (isSlopeOccupied)
                                        isGrounded = resolveCollisionRect(*_, slopeMtvState, delta, slopeMtvStateVec, true).isGrounded || isGrounded;
                                    auto t = getTriangleEnvelop(*targets[i]);
                                    auto mtv = calculateRect2RectMTV(*_, t, false, delta);
                                    if (r.isSlopeOutsideH) mtv.x = 0.0f;
                                    slopeMtvState = t;
                                    slopeMtvStateVec = mtv;
                                    applyDominantSlope = true;
                                    lastVerticalMtv = mtv.y;
                                    isSlopeOccupied = true;
                                }

                                targets[i]->hasObjectCollidedChunk = true;
                            }
                        }
                        else if (intersects)
                        {
                            auto mtv = calculateRect2RectMTV(*_, *targets[i], false, delta);
                            lastVerticalMtv = mtv.y;
                            auto r = resolveCollisionRect(*_, *targets[i], delta, mtv, false);
                            isGrounded = r.isGrounded || isGrounded;

                            if (r.isGrounded) _->isSlopeGroundSteep = r.isSlopeGroundSteep;

                            if (intersects && targets[i]->isMovable && isGrounded) {
                                _->movableGround = targets[i];
                                _->movableGroundPos = V2D(targets[i]->x, targets[i]->y);
                            }

                            targets[i]->hasObjectCollidedChunk = true;
                        }
                    }
                    steps++;

                    if (applyDominantSlope)
                    {
                        if (_->slopeGround)
                        {
                            auto t = getTriangleEnvelop(*_->slopeGround);
                            auto& tt = slopeMtvState;
                            bool isOutsideH = _->x < t.x && t.b > 0 || _->x + _->w > t.x + t.w && t.b < 0;
                            if ((_->y + _->h > t.y + t.h && isOutsideH || _->y < t.y && isOutsideH)
                                && FUZZY(t.y + t.l, tt.y + tt.l, 10))
                                slopeMtvStateVec = V2D::ZERO;
                        }
                        auto r = resolveCollisionRect(*_, slopeMtvState, delta, slopeMtvStateVec, true);
                        if (r.isGrounded) _->isSlopeGroundSteep = r.isSlopeGroundSteep;
                        isGrounded = r.isGrounded || isGrounded;
                    }

                    // Edge case: make sure the player gets pushed by lastVerticalMTV if this slope is before the next object with the last MTV,
                    //            we're just re-running the code to resolve the slope again before the lastVerticalMTV is reset.
                    if (_->slopeGround) {
                        auto r = resolveCollisionSlope(*_, *_->slopeGround, isJumping, lastVerticalMtv, delta);
                        isGrounded = r.isGrounded || isGrounded;
                    }

                    if (quitLater) break;
                }

            _->internalAngleLerp = MathUtil::lerp(_->internalAngleLerp, _->internalAngle, 8 * delta);

            if (isGrounded && !_->movableGround)
            {
                _->relativeMovableForce.x = LERP(_->relativeMovableForce.x, 0, 10 * delta);
                _->relativeMovableForce.y = LERP(_->relativeMovableForce.y, 0, 10 * delta);
            }

            _->nx = 0.f;
            _->ny = 0.f;

            if (_->slopeGround == nullptr && !_->isSlopeGroundSteep)
                _->timeSpentOnSlope = 0;

            if (!FUZZY(debug1, 1.0, DBL_EPSILON))
                DebugBreak();

            if (isGrounded && _->pref_vel.x == 0 && !_->isSlopeGroundSteep) {
                _->vel.x = MathUtil::lerp(_->vel.x, 0, 10 * delta);
            }

            if (!isGrounded && _->pref_vel.x == 0)
                _->internalAngle = MathUtil::lerp(_->internalAngle, 0, 2 * delta);

            if (steps > 4)
                RLOGW("  {} CCD steps swept {} shapes. TOO MUCH!!!", steps, size);
        }
    }
}

inline double sawtoothSin(double angle) {
    angle = fmod(angle, 2.0 * M_PI);
    if (angle < M_PI) {
        return -1.0 + 2.0 * angle / M_PI;
    }
    else {
        return 3.0 - 2.0 * angle / M_PI;
    }
}

inline double sawtoothCos(double angle) {
    return sawtoothSin(angle + M_PI / 2.0);
}

void ReboundPhysics::PhysicsWorld::partition()
{
    for (auto& _ : _staticShapes)
    {
        chunkGetCoverArea(_coveredChunksBuffers[0], *_, true);
        auto& vec = getChunkTierMap(_->chunkTier)[_coveredChunksBuffers[0][0]];
        chunks[_][&vec] = vec.shove(_);
    }
}

ReboundPhysics::CollisionChunkMap& ReboundPhysics::PhysicsWorld::getChunkTierMap(U8 tier)
{
    switch (tier)
    {
    default:
        return _staticShapeChunksTier0;
        break;
    case 1:
        return _staticShapeChunksTier1;
        break;
    case 2:
        return _staticShapeChunksTier2;
        break;
    case 3:
        return _staticShapeChunksTier3;
        break;
    case 4:
        return _staticShapeChunksTier4;
        break;
    }
}

ReboundPhysics::PhysicsWorld* ReboundPhysics::PhysicsWorld::create()
{
    auto ret = new PhysicsWorld();
    ret->autorelease();
    return ret;
}

void ReboundPhysics::PhysicsWorld::update(F32 delta)
{
    if (!_physicsDebugNode) {
        _physicsDebugNode = DrawNode::create(1);
        addChild(_physicsDebugNode);
    }

    if (_physicsDebugNode)
        _physicsDebugNode->clear();

    currentPhysicsDt += delta;

    int physicsTPS = 1.0 / delta;

    physicsTPS = physicsTPS < 60 ? 60 : physicsTPS;

    float currentLastPhysicsDt = lastPhysicsDt;
    while (lastPhysicsDt < currentPhysicsDt)
    {
        F64 substepRatio = 90.0 / physicsTPS;
        U8 substeps = substepRatio * 4;
        substeps = MAX(1, MIN(substeps, 4));
        //RLOG("substeps: {}", substeps);

        for (int i = 0; i < substeps; i++) {
            float lastPhysicsDtT = lastPhysicsDt;
            lastPhysicsDt += 1.0 / physicsTPS * (1.0 / substeps);
            float dtPercLeftAmount = 1.0;
            if (lastPhysicsDt > currentPhysicsDt)
            {
                dtPercLeftAmount = Math::map(currentPhysicsDt, lastPhysicsDt, lastPhysicsDtT, 1.0, 0.0);
                lastPhysicsDt = currentPhysicsDt;
                i = substeps;
            }

            move(Math::map(lastPhysicsDt, currentLastPhysicsDt, currentPhysicsDt, 0.0, 1.0));
            step(1.0 / physicsTPS * (1.0 / substeps) * dtPercLeftAmount);
        }
    }

    _moveTargetCount = 0;

    int drawLimit = 0;
    for (auto& _ : _staticShapes) {
        if (_->isTriangle) {
            _physicsDebugNode->drawLine(Vec2(_->x, _->y), Vec2(_->x + _->b, _->y), _->debugColor);
            _physicsDebugNode->drawLine(Vec2(_->x, _->y), Vec2(_->x, _->y + _->l), _->debugColor);
            _physicsDebugNode->drawLine(Vec2(_->x, _->y + _->l), Vec2(_->x + _->b, _->y), _->debugColor);
        }
        else
            _physicsDebugNode->drawRect(Vec2(_->x, _->y), Vec2(_->x + _->w, _->y + _->h), _->debugColor);

        if (drawLimit++ > 200) break;
    }

    for (auto& _ : _dynamicShapes)
    {
        if (!_physicsDebugNode) {
            _physicsDebugNode = DrawNode::create(1);
            addChild(_physicsDebugNode);
        }

        Vec2 bl = Vec2(_->x, _->y);
        Vec2 br = Vec2(_->x + _->w, _->y);
        Vec2 tl = Vec2(_->x, _->y + _->h);
        Vec2 tr = Vec2(_->x + _->w, _->y + _->h);

        bl = bl.rotateByAngle(Vec2(_->x + _->w / 2, _->y + _->h / 2), -_->internalAngleLerp);
        br = br.rotateByAngle(Vec2(_->x + _->w / 2, _->y + _->h / 2), -_->internalAngleLerp);
        tl = tl.rotateByAngle(Vec2(_->x + _->w / 2, _->y + _->h / 2), -_->internalAngleLerp);
        tr = tr.rotateByAngle(Vec2(_->x + _->w / 2, _->y + _->h / 2), -_->internalAngleLerp);

        _physicsDebugNode->drawLine(bl, br, Color4B::GREEN);
        _physicsDebugNode->drawLine(bl, tl, Color4B::GREEN);
        _physicsDebugNode->drawLine(tl, tr, Color4B::GREEN);
        _physicsDebugNode->drawLine(br, tr, Color4B::GREEN);

        _physicsDebugNode->drawCircle(Vec2(_->x + _->w / 2, _->y + _->h / 2), _->w / 2 - 1, 0, 32, false, Color4B::RED);
    }

    for (auto& dat : _extraDraw)
    {
        _physicsDebugNode->drawRect(dat.orig, dat.dest, dat.col);
    }
    _extraDraw.clear();
}
