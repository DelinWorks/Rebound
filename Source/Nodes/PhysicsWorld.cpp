#include "PhysicsWorld.h"

ReboundPhysics::CollisionShape* ReboundPhysics::createRect(V2D pos, V2D size)
{
    CollisionShape* s = new CollisionShape();
    s->x = pos.x;
    s->y = pos.y;
    s->w = size.x;
    s->h = size.y;
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

V2D ReboundPhysics::calculateRect2RectMTV(const CollisionShape& rect1, const CollisionShape& rect2, bool both)
{
    F32 dx = (rect1.x + rect1.w / 2) - (rect2.x + rect2.w / 2);
    F32 dy = (rect1.y + rect1.h / 2) - (rect2.y + rect2.h / 2);
    F32 combinedHalfWidths = rect1.w / 2 + rect2.w / 2;
    F32 combinedHalfHeights = rect1.h / 2 + rect2.h / 2;

    if (std::abs(dx) < combinedHalfWidths && std::abs(dy) < combinedHalfHeights) {
        F32 overlapX = combinedHalfWidths - std::abs(dx);
        F32 overlapY = combinedHalfHeights - std::abs(dy);

        if (both)
            return V2D(dx > 0 ? overlapX : -overlapX, dy > 0 ? overlapY : -overlapY);

        else if (overlapX >= overlapY) {
            //if (rect1.y > rect2.y - rect1.h + VERTICAL_RESOLUTION_LEEWAY && rect1.y + VERTICAL_RESOLUTION_LEEWAY < rect2.y + rect2.h)
            //    overlapY = 0;

            if (dy > 0)
                return V2D(0, overlapY);
            else
                return V2D(0, -overlapY);
        }
        else {
            //if (rect1.x > rect2.x - rect1.w + VERTICAL_RESOLUTION_LEEWAY && rect1.x + VERTICAL_RESOLUTION_LEEWAY < rect2.x + rect2.w)
            //    overlapX = 0;

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

ReboundPhysics::ResolveResult ReboundPhysics::resolveCollisionRect(DynamicCollisionShape& _, CollisionShape& __, const V2D& mtv, bool ignoreVL)
{
    ResolveResult result{ false };

    F32 leeway = 1;// MAX(1, abs(MIN(200, _.vel.x)) / 50);
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

    //if (mtv.y < 0 && _.vel.y > 0) {
    //    //_.vel.y = 0;
    //    _.ny = 0.0f;
    //}

    bool vYT = _.y + _.h - VERTICAL_RESOLUTION_LEEWAY > __.y;
    bool vYB = _.y + VERTICAL_RESOLUTION_LEEWAY < __.y + __.h;
    bool vMtvApplied = false;

    if (vYT && vYB)
    {
        if (mtv.x > 0 && _.vel.x < 0 || mtv.x < 0 && _.vel.x > 0) {
            _.vel.x = 0;
            _.lerp_vel.x = 0;
            _.nx = 0.0f;
        }
        _.x += mtv.x;
        if (mtv.x != 0)
            _.nx = 0.0f;

        if (isWithinLeeway) {
            _.y += mtv.y;
            if (mtv.y != 0)
                _.ny = 0.0f;
            vMtvApplied = true;
        }
    }
    //else if (isWithinLeeway)
    //{
    //    F32 y = calculateRect2RectMTV(_, __, true).y;
    //    _.y += y;
    //    vMtvApplied = true;
    //}
    //else if (vYT && isWithinLeeway && !ignoreVL) _.y = __.y + __.h;
    //else if (vYB && isWithinLeeway && !ignoreVL) _.y = __.y - _.h;

    if (isWithinLeeway && ignoreVL || !vMtvApplied) {
        _.y += mtv.y;
        if (mtv.y != 0)
            _.ny = 0.0f;
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

        if (t.b > 0 && r.x + r.w / 4 > t.x + e.b || t.b < 0 && r.x + r.w / 1.5 < t.x + e.b)
            if (abs(e.l) <= abs(e.b))
                return false;

        F32 stickyness = abs(r.vel.y) > abs(forceUp) || (NUM_SIGN(r.gravity) < 0 && t.l < 0) || (NUM_SIGN(r.gravity) > 0 && t.l > 0) ? 0 : 6;

        if (t.l > 0 && r.y < incline + stickyness || t.l < 0 && r.y > incline - stickyness) {
            //
            //r.ny = 0.0f;

            if (NUM_SIGN(r.gravity) < 0 && r.vel.y < 0)
                r.vel.y = 0;
            else if (NUM_SIGN(r.gravity) > 0 && r.vel.y > 0)
                r.vel.y = 0;

            double theta_rad = std::atan(t.l / t.b);
            r.internalAngle = theta_rad;

            //r.x += -r.vel.x * delta * (1.0 - sin(cornerAngle));

            if (isTop) r.internalAngle = 0;

            if (isJumping && r.y > incline) {
                result.isGrounded = true;
                //return false;
            }
            if (t.l > 0 && verticalMtv < 0 || t.l < 0 && verticalMtv > 0) {
                r.x += abs(verticalMtv) / 2 * (e.b / e.l) * NUM_SIGN(t.l);
                r.vel.x = 0;
                r.lerp_vel.x = 0;
                //r.nx = 0.0f;
            }

            //if (t.b > 0 && r.y < incline - 3 || abs(verticalMtv) > 0)
            //    r.x += abs(verticalMtv) / 2 * (e.b / e.l) * NUM_SIGN(t.l);

            result.verticalMTV = r.y < incline ? incline - r.y : 0;

            r.y += incline - r.y;

            if (!isTop)
            {
                if (e.b > 0 && r.vel.x < 0 || e.b < 0 && r.vel.x > 0)
                    r.vel.y = forceUp * NUM_SIGN(t.l);
                else if (e.b > 0 && r.vel.x > 0 || e.b < 0 && r.vel.x < 0)
                    r.vel.y = forceUp * -NUM_SIGN(t.l);

                if (sin(cornerAngle) > sin(AX_DEGREES_TO_RADIANS(60)))
                    r.vel.x += 100 * delta * NUM_SIGN(t.b) * MIN(r.timeSpentOnSlope * 300, 20);
                //r.x += r.vel.x * delta / 2;
            }

            //if (sin(cornerAngle) > 0.5)
            //{
            //    r.x += NUM_SIGN(t.b) * 100 * sin(cornerAngle) * delta;
            //    r.vel.x /= 1.01;
            //    r.lerp_vel.x = 0;
            //    r.vel.y = 0;
            //}

            //r.ny = 0.0f;
            result.isGrounded = true;
        }

        return true;
    };

    //alpha = alpha - 1;
    //return sqrt(1 - alpha * alpha);
    //F32 cornerSlopeCurve = circEaseOut(cornerSlope / 10) * 10;

    r.slopeGround = &t;
    result.isSlope = true;

    //F32 tangent = Math::map_clamp_out(cornerSlope, 0, 8, 1, 0.13);
    //F32 offset = r.h * cornerSlope / (M_PI / tangent);
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

void ReboundPhysics::stepDynamic(DynamicCollisionShape& s, F32 delta, F32 fraction)
{
    s.vel.x = std::clamp<F32>(s.vel.x, -250000, 250000);
    s.vel.y = std::clamp<F32>(s.vel.y, -250000, 250000);

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

I32 ReboundPhysics::chunkGetCoverArea(V2DH* array, CollisionShape s)
{
    I32 startX = 0;
    I32 startY = 0;
    I32 endX = 0;
    I32 endY = 0;

    if (s.isTriangle)
        s = getTriangleEnvelop(s);

    startX = (s.x >= 0) ? s.x / PHYS_CHUNK_SIZE : (s.x - PHYS_CHUNK_SIZE + 1) / PHYS_CHUNK_SIZE;
    startY = (s.y >= 0) ? s.y / PHYS_CHUNK_SIZE : (s.y - PHYS_CHUNK_SIZE + 1) / PHYS_CHUNK_SIZE;
    endX = (s.x >= 0 && MAX(5, s.w) < PHYS_CHUNK_SIZE || s.x > -MAX(5, s.w) && MAX(5, s.w) >= PHYS_CHUNK_SIZE)
    ? (s.x + MAX(5, s.w) - 1) / PHYS_CHUNK_SIZE : (s.x - PHYS_CHUNK_SIZE + MAX(5, s.w) - 1) / PHYS_CHUNK_SIZE;
    endY = (s.y >= 0 && MAX(5, s.h) < PHYS_CHUNK_SIZE || s.y > -MAX(5, s.h) && MAX(5, s.h) >= PHYS_CHUNK_SIZE)
        ? (s.y + MAX(5, s.h) - 1) / PHYS_CHUNK_SIZE : (s.y - PHYS_CHUNK_SIZE + MAX(5, s.h) - 1) / PHYS_CHUNK_SIZE;

    int count = 0;

    for (I32 x = startX; x <= endX; x++) {
        for (I32 y = startY; y <= endY; y++) {
            //if (count >= CHUNK_MAX_TARGETS) break;
            array[count++] = V2DH(x, y);
        }
    }

    return count;
}

void ReboundPhysics::setShapePosition(PhysicsWorld* worldToRegisterSweep, CollisionShape& shape, const V2D& newPos)
{
    if (worldToRegisterSweep)
    {
        auto w = worldToRegisterSweep;
        w->_moveTargets[w->_moveTargetCount++] = &shape;
    }

    shape.nx = newPos.x;
    shape.ny = newPos.y;
}

void ReboundPhysics::PhysicsWorld::step(F64 delta)
{
    //BENCHMARK_SECTION_BEGIN("test move performance");
    for (U32 i = 0; i < _moveTargetCount; i++)
    {
        auto& _ = _moveTargets[i];

        _->x = _->nx;
        _->y = _->ny;

        I32 countOld = chunkGetCoverArea(_coveredChunksBuffers[0], CollisionShape(_->cx, _->cy, _->cw, _->ch));
        I32 countNew = chunkGetCoverArea(_coveredChunksBuffers[1], *_);

        if (countOld != countNew || memcmp(_coveredChunksBuffers[0], _coveredChunksBuffers[1], countNew * sizeof(V2DH)) != 0)
        {
            for (int i = 0; i < countNew; i++)
            {
                auto& newChunk = _coveredChunksBuffers[1][i];

                bool cont = false;
                for (int p = 0; p < countOld; p++)
                    if (newChunk == _coveredChunksBuffers[0][p])
                    {
                        cont = true;
                        break;
                    }
                if (cont) continue;

                _staticShapeChunks[newChunk].push_back(_);
            }

            for (int i = 0; i < countOld; i++)
            {
                auto& oldChunk = _coveredChunksBuffers[0][i];

                bool cont = false;
                for (int p = 0; p < countNew; p++)
                    if (oldChunk == _coveredChunksBuffers[1][p])
                    {
                        cont = true;
                        break;
                    }
                if (cont) continue;

                auto& arr = _staticShapeChunks[oldChunk];
                auto it = std::find(arr.begin(), arr.end(), _);
                if (it != arr.end())
                    arr.erase(it);
            }

            _->cx = _->x;
            _->cy = _->y;
            _->cw = _->w;
            _->ch = _->h;
        }
    }
    _moveTargetCount = 0;
    //BENCHMARK_SECTION_END();

    for (auto& _ : _dynamicShapes)
    {
        //if (isGrounded)
        _->vel.x = MathUtil::lerp(_->vel.x, _->pref_vel.x, 1 * delta);

        if (isJumping && isGrounded)
            _->vel.y = 2000 * -NUM_SIGN(_->gravity)/* MAX(1, isSlope ? 0.5 * abs(_->vel.x / 650) * slopeIncline : 1)*/;
        //_->vel.y = 8000;

        isGrounded = false;
        _->timeSpentOnSlope += delta * 32;
        //RLOG("{}", _->timeSpentOnSlope);

        //if (abs(_->pref_vel.x) > abs(_->lerp_vel.x))
        //    //_->vel.x = _->lerp_vel.x;

        //AXLOG("%f", _->internalAngle);

        _->slopeGround = nullptr;

        _->vel.x = std::clamp<F32>(_->vel.x, -1000000, 1000000);
        _->vel.y = std::clamp<F32>(_->vel.y, -1000000, 1000000);

        _->nx += _->vel.x * delta;
        _->ny += _->vel.y * delta;

        if (_->movableGround) {
            Vec2 oldPos = _->movableGroundPos;
            Vec2 newPos = Vec2(_->movableGround->x, _->movableGround->y);
            Vec2 size = Vec2(_->movableGround->w, _->movableGround->h);
            _->x += newPos.x - oldPos.x;
            _->y += newPos.y - oldPos.y;
            _->movableGroundPos = newPos;
            _->isMovableLerpApplied = false;

            if (!getCollisionShapeIntersect(*_, CollisionShape(newPos.x + 1, newPos.y - 3, size.width - 2, size.height + 6)))
            {
                float hpush = (newPos.x - oldPos.x) / delta;
                if (abs(hpush) > abs(_->pref_vel.x))
                _->vel.x = hpush;
                //_->vel.x = (newPos.x - oldPos.x) / delta;
                //if (_->movableGroundMtv.x == 0) {
                    float vpush = (newPos.y - oldPos.y) / delta;
                    _->vel.y = isJumping ? _->vel.y : vpush;
                //}
                _->movableGround = nullptr;
                _->isMovableLerpApplied = true;
            }
        }

        // an extent by 4 pixels is applied to the motion envelope so that
        // any motions that happen outside the envelope are picked.
        float envExtent = 32;
        {
            int size = 0;
            //int msize = 0;
            CollisionShape* targets[CCD_MAX_TARGETS];
            //CollisionShape* mtargets[MOVE_MAX_TARGETS];
            CollisionShape envelope;
            {
                //float gravity = _->vel.y + _->gravity * delta;

                float ox = _->x;
                float oy = _->y;
                float nx = ox + _->nx;
                float ny = oy + _->ny;

                CollisionShape oldPos = CollisionShape(ox, oy, _->w, _->h);
                CollisionShape newPos = CollisionShape(nx, ny, _->w, _->h);
                envelope = getRectSweepEnvelope(oldPos, newPos, envExtent);

                I32 count = chunkGetCoverArea(_coveredChunksBuffers[0], envelope);

                for (I32 i = 0; i < count; i++) {
                    for (auto& __ : _staticShapeChunks[_coveredChunksBuffers[0][i]]) {
                        CollisionShape rect = *__;
                        if (rect.isTriangle)
                            rect = getTriangleEnvelop(*__);
                        if (getCollisionShapeIntersect(rect, envelope)) {
                            if (size >= CCD_MAX_TARGETS) break;
                            targets[size++] = __;
                            //__->debugColor = Color4F::WHITE;
                        }
                        //else
                        //    __->debugColor = Color4F::RED;
                    }
                }

                //for (U32 i = 0; i < _moveTargetCount; i++) {
                //    CollisionShape rect = *_moveTargets[i];
                //    if (rect.isTriangle)
                //        rect = getTriangleEnvelop(*_moveTargets[i]);
                //    if (getCollisionShapeIntersect(rect, envelope)) {
                //        if (size >= CCD_MAX_TARGETS)
                //            break;
                //        targets[size++] = _moveTargets[i];
                //        mtargets[msize++] = _moveTargets[i];
                //        _moveTargets[i]->debugColor = Color4F::WHITE;
                //    }
                //    else
                //        _moveTargets[i]->debugColor = Color4F::RED;
                //}
            }

            int steps = 0;
            _->vel.y += _->gravity * delta;

            // OBSOLETE: don't use velocity to calculate CCD as that's sometimes inaccurate, 
            // and will result in tunneling or even worse, stutters or lag.
            // double ccdPrecession = CCD_STEPS_TO_PERC(getCCDPrecessionSteps(_->vel.length()));

            float sweptVolumePrec = 1;

            // calculate the required CCD steps based on the volume of the motion sweep envelope.
            // maybe later implement OBBs (Oriented Bounding Box) to gain extra performance,
            // when the player is accelerating in both coordinates which will give a gigantic envelope.
            // but that's not important for a minimal & simple arcade physics engine at this stage of development.
            double ccdPrecession = CCD_STEPS_TO_PERC(MAX(1,
                MAX(envelope.w * sweptVolumePrec - _->w - envExtent * 2,
                    envelope.h * sweptVolumePrec - _->h - envExtent * 2)));

            //for (U32 i = 0; i < msize; i++)
            //{
            //    float ox = mtargets[i]->x;
            //    float oy = mtargets[i]->y;
            //    float nx = ox + mtargets[i]->nx;
            //    float ny = oy + mtargets[i]->ny;

            //    CollisionShape oldPos = CollisionShape(ox, oy, mtargets[i]->w, mtargets[i]->h);
            //    CollisionShape newPos = CollisionShape(nx, ny, mtargets[i]->w, mtargets[i]->h);
            //    CollisionShape menvelope = getRectSweepEnvelope(oldPos, newPos, envExtent);

            //    ccdPrecession = MIN(ccdPrecession, CCD_STEPS_TO_PERC(MAX(1,
            //        MAX(menvelope.w * sweptVolumePrec - mtargets[i]->w - envExtent * 2,
            //            menvelope.h * sweptVolumePrec - mtargets[i]->h - envExtent * 2))));
            //}

            ccdPrecession *= 4;

            bool isSlope = false;
            float slopeIncline = 1.0f;
            float lastVerticalMtv = 0.0f;
            double step_perc = 1.0;

            double debug1 = 0.0f;
            if (size == 0) {
                stepDynamic(*_, delta, 1);
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
                        stepDynamic(*_, delta, ccdPrecession);
                        debug1 += ccdPrecession;
                        step_perc -= ccdPrecession;
                    }
                    // Edge case: make sure the entire ccd volume is swept and not leave atleast 0.0001 of volume,
                    //            or else there will be inaccuracies in velocity and may cause the player to jitter.
                    else
                    {
                        stepDynamic(*_, delta, step_perc);
                        debug1 += step_perc;
                        step_perc -= step_perc;
                        quitLater = true;
                    }
                    // // // // // // // // // // // // // // // // // // // // // // // // // // // // // //

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
                                //auto mtv = calculateRect2RectMTV(_, *targets[i]);
                                auto r = resolveCollisionSlope(*_, *targets[i], isJumping, lastVerticalMtv, delta);
                                lastVerticalMtv = r.verticalMTV;
                                isGrounded = r.isGrounded || isGrounded;
                                if (!r.isSlope) {
                                    if (isSlopeOccupied)
                                        isGrounded = resolveCollisionRect(*_, slopeMtvState, slopeMtvStateVec, true).isGrounded || isGrounded;
                                    auto t = getTriangleEnvelop(*targets[i]);
                                    auto mtv = calculateRect2RectMTV(*_, t);
                                    if (r.isSlopeOutsideH) mtv.x = 0.0f;
                                    slopeMtvState = t;
                                    slopeMtvStateVec = mtv;
                                    applyDominantSlope = true;
                                    lastVerticalMtv = mtv.y;
                                    isSlopeOccupied = true;
                                }

                                targets[i]->hasObjectCollidedChunk = true;

                                //else if (lastVerticalMtv != 0) {
                                //    isSlope = true;
                                //    slopeIncline = r.slopeIncline;
                                //    if (applyDominantSlope) {
                                //        applyDominantSlope = false;
                                //        ignoreNextSlope = true;
                                //    }
                                //    if (i < size - 1 && targets[i + 1]->isTriangle) {
                                //        i++;
                                //        continue;
                                //    }
                                //}
                            }
                        }
                        else if (intersects)
                        {
                            auto mtv = calculateRect2RectMTV(*_, *targets[i]);
                            lastVerticalMtv = mtv.y;
                            auto r = resolveCollisionRect(*_, *targets[i], mtv, false);
                            isGrounded = r.isGrounded || isGrounded;

                            if (intersects && targets[i]->isMovable && isGrounded) {
                                _->movableGround = targets[i];
                                _->movableGroundPos = Vec2(targets[i]->x, targets[i]->y);
                                //_->movableGroundMtv = V2D(mtv.x, mtv.y);
                            }

                            targets[i]->hasObjectCollidedChunk = true;
                        }
                    }
                    steps++;

                    if (applyDominantSlope) {

                        // Edge case: prevent the player from getting stuck in opposing slopes when climbing them
                        if (_->slopeGround)
                        {
                            auto t = getTriangleEnvelop(*_->slopeGround);
                            auto& tt = slopeMtvState;
                            bool isOutsideH = _->x < t.x && t.b > 0 || _->x + _->w > t.x + t.w && t.b < 0;
                            if ((_->y + _->h > t.y + t.h && isOutsideH || _->y < t.y && isOutsideH)
                                && FUZZY(t.y + t.l, tt.y + tt.l, 10))
                                slopeMtvStateVec = V2D::ZERO;
                        }
                        // // // // // // // // // // // // // // // // // // // // // // // // // // // // // //

                        isGrounded = resolveCollisionRect(*_, slopeMtvState, slopeMtvStateVec, true).isGrounded || isGrounded;
                    }

                    // Edge case: make sure the player gets pushed by lastVerticalMTV if this slope is before the next object with the last MTV,
                    //            we're just re-running the code to resolve the slope again before the lastVerticalMTV is reset.
                    if (_->slopeGround) {
                        auto r = resolveCollisionSlope(*_, *_->slopeGround, isJumping, lastVerticalMtv, delta);
                        isGrounded = r.isGrounded || isGrounded;
                    }
                    // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // //

                    if (quitLater) break;
                }

            _->internalAngleLerp = MathUtil::lerp(_->internalAngleLerp, _->internalAngle, 8 * delta);

            _->nx = 0.f;
            _->ny = 0.f;

            if (_->slopeGround == nullptr)
                _->timeSpentOnSlope = 0;

            //if (!_->slopeGround)
            //    _->internalAngle = MathUtil::lerp(_->internalAngle, 0, 8 * delta);

            if (!FUZZY(debug1, 1.0, DBL_EPSILON))
                DebugBreak();

            if (isGrounded && _->pref_vel.x == 0) {
                _->vel.x = MathUtil::lerp(_->vel.x, 0, 10 * delta);
            }

            if (!isGrounded && _->pref_vel.x == 0)
                _->internalAngle = MathUtil::lerp(_->internalAngle, 0, 2 * delta);

            //if (!isGrounded)
            //    _->internalAngle = MathUtil::lerp(_->internalAngle, sin(lastPhysicsDt * 10), 2 * delta);

            //if (/*size &&*/ steps > 1)
            //    RLOG("  {} CCD steps swept {} shapes", steps, size);
        }
        //else {
        //    for (auto& __ : _staticShapes)
        //    {
        //        _.vel.y += _.gravity * delta;

        //        //_.vel.x = std::clamp<float>(_.vel.x, -6000, 6000);
        //        //_.vel.y = std::clamp<float>(_.vel.y, -6000, 6000);

        //        float ox = _.x;
        //        float oy = _.y;
        //        _.x += _.vel.x * delta;
        //        _.y += _.vel.y * delta;

        //        if (getCollisionShapeIntersect(_, __))
        //        {
        //            auto mtv = calculateRect2RectMTV(_, __);
        //            isGrounded = resolveCollisionShape(_, __, mtv).isGrounded || isGrounded;
        //        }
        //    }
        //}
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
        I32 count = chunkGetCoverArea(_coveredChunksBuffers[0], *_);

        for (I32 i = 0; i < count; i++)
        {
            _staticShapeChunks[_coveredChunksBuffers[0][i]].push_back(_);

            if (_->isMovable)
            {
                _->cx = _->x;
                _->cy = _->y;
                _->cw = _->w;
                _->ch = _->h;
            }
        }
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

    //if (lastPhysicsDt < 1 && delta > 0.1)
    //    lastPhysicsDt = currentPhysicsDt;

    int physicsTPS = 1.0 / delta;

    physicsTPS = physicsTPS < 60 ? 60 : physicsTPS;

    //physicsTPS = 1000;

    //if (lastPhysicsDt + 10 < currentPhysicsDt)
    //    lastPhysicsDt = currentPhysicsDt;

    //modifySlope->l = 512 + 1024 * cos(lastPhysicsDt * 2);
    //modifySlope->l = 32;
    //modifySlope->b = 512 + 256 * cos(lastPhysicsDt * 10);

    while (lastPhysicsDt < currentPhysicsDt)
    {
        F64 substepRatio = 90.0 / physicsTPS;
        U8 substeps = substepRatio * 4;
        substeps = MAX(1, MIN(substeps, 4));
        //RLOG("{}", substeps);

        for (int i = 0; i < substeps; i++) {
            lastPhysicsDt += 1.0 / physicsTPS * (1.0 / substeps);

            for (int i = 0; i < movingPlat.size(); i++)
                setShapePosition(this, *movingPlat[i], V2D(100 + 200 * sawtoothSin(lastPhysicsDt * 7), 0 + 170 * sawtoothCos(lastPhysicsDt * 2)));

            step(1.0 / physicsTPS * (1.0 / substeps));
        }
    }

    //auto visibleSize = _director->getVisibleSize();
    //rayCastLocation = mouseLoc - visibleSize / 2;
    //for (int i = 0; i < 2880; i++) {
    //    Vec2 target = rayCastTargetVector.rotateByAngle(ax::Vec2::ZERO, AX_DEGREES_TO_RADIANS(i / 8.0));
    //    auto result1 = ReboundPhysics::doLineIntersectsRects(rayCastLocation, rayCastLocation + target, _staticShapes);
    //    auto result2 = ReboundPhysics::doLineIntersectsRects(rayCastLocation, rayCastLocation + target, _dynamicShapes);
    //    if (result1.intersects && result1.point.distanceSquared(rayCastLocation) < result2.point.distanceSquared(rayCastLocation))
    //        rays[i] = result1.point;
    //    else if (result2.intersects)
    //        rays[i] = result2.point;
    //    else rays[i] = { INFINITY, INFINITY };
    //}
    //auto result = ReboundPhysics::doLineIntersectsRects(rayCastLocation, rayCastLocation + rayCastTargetVector, _testShapes);

    int drawLimit = 0;
    for (auto& _ : _staticShapes) {
        if (_->isTriangle) {
            _physicsDebugNode->drawLine(Vec2(_->x, _->y), Vec2(_->x + _->b, _->y), _->debugColor);
            _physicsDebugNode->drawLine(Vec2(_->x, _->y), Vec2(_->x, _->y + _->l), _->debugColor);
            _physicsDebugNode->drawLine(Vec2(_->x, _->y + _->l), Vec2(_->x + _->b, _->y), _->debugColor);
        }
        else
            _physicsDebugNode->drawRect(Vec2(_->x, _->y), Vec2(_->x + _->w, _->y + _->h), _->debugColor);

        if (drawLimit++ > 2000) break;
    }

    for (auto& _ : _dynamicShapes)
    {
        if (!_physicsDebugNode) {
            _physicsDebugNode = DrawNode::create(1);
            addChild(_physicsDebugNode);
        }

        Vec2 bl = Vec2(_->x + 1, _->y + 1);
        Vec2 br = Vec2(_->x + _->w - 1, _->y + 1);
        Vec2 tl = Vec2(_->x + 1, _->y + _->h - 1);
        Vec2 tr = Vec2(_->x + 1 + _->w - 1, _->y + _->h - 1);

        bl = bl.rotateByAngle(Vec2(_->x + _->w / 2, _->y + _->h / 2), -_->internalAngleLerp);
        br = br.rotateByAngle(Vec2(_->x + _->w / 2, _->y + _->h / 2), -_->internalAngleLerp);
        tl = tl.rotateByAngle(Vec2(_->x + _->w / 2, _->y + _->h / 2), -_->internalAngleLerp);
        tr = tr.rotateByAngle(Vec2(_->x + _->w / 2, _->y + _->h / 2), -_->internalAngleLerp);

        _physicsDebugNode->drawLine(bl, br, Color4B::GREEN);
        _physicsDebugNode->drawLine(bl, tl, Color4B::GREEN);
        _physicsDebugNode->drawLine(tl, tr, Color4B::GREEN);
        _physicsDebugNode->drawLine(br, tr, Color4B::GREEN);

        _physicsDebugNode->drawCircle(Vec2(_->x + _->w / 2, _->y + _->h / 2), _->w / 2 - 1, 0, 32, false, Color4B::RED);

        //BENCHMARK_SECTION_BEGIN("chunk cover area");
        //for (int i = 0; i < 400000; i++)
        //    chunkGetCoverArea(_coveredChunks, CollisionShape(0, 0, 512, 512));
        //BENCHMARK_SECTION_END();

        I32 count = chunkGetCoverArea(_coveredChunksBuffers[0], *_);

        for (I32 i = 0; i < count; i++) {
            auto& v = _coveredChunksBuffers[0][i];
            V2D p = V2D(v.x, v.y) * PHYS_CHUNK_SIZE;
            _physicsDebugNode->drawRect(p, p + V2D(PHYS_CHUNK_SIZE, PHYS_CHUNK_SIZE), Color4B::ORANGE);
        }
    }
    //for (int i = 0; i < 2880; i++) {
    //    Vec2 target = Vec2(100, 0).rotateByAngle(ax::Vec2::ZERO, AX_DEGREES_TO_RADIANS(i));
    //    _physicsDebugNode->drawLine(rayCastLocation, rayCastLocation + target, Color4B(255, 127, 0, 20));
    //}
    //for (int i = 0; i < 2880; i++) {
    //    Vec2 target = rayCastTargetVector.rotateByAngle(ax::Vec2::ZERO, AX_DEGREES_TO_RADIANS(i));
    //    _physicsDebugNode->drawDot(rays[i], 8, Color4B(255, 127, 0, 20));
    //}
}
