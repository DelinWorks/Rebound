/****************************************************************************
 Copyright (c) 2017-2018 Xiamen Yaji Software Co., Ltd.
 Copyright (c) 2021 Bytedance Inc.

 https://axmolengine.github.io/

 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:

 The above copyright notice and this permission notice shall be included in
 all copies or substantial portions of the Software.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.
 ****************************************************************************/

#include "PhysicsTestScene.h"
#include <iostream>;

USING_NS_AX;

Scene* ReboundPhysicsTest::createScene()
{
    return ReboundPhysicsTest::create();
}

// on "init" you need to initialize your instance
bool ReboundPhysicsTest::init()
{
    //////////////////////////////
    // 1. super init first
    if (!Scene::init())
    {
        return false;
    }

    auto visibleSize = _director->getVisibleSize();
    auto origin = _director->getVisibleOrigin();
    auto safeArea = _director->getSafeAreaRect();
    auto safeOrigin = safeArea.origin;

    scheduleUpdate();

    _defaultCamera->setPosition({ 0, 0 });

    _staticShapes.pushBack(createRect(Vec2(UINT16_MAX / -2, -332), Vec2(float(UINT16_MAX), 32)));

    float iy = 10;
    for (float i = -32 - 200; i < 32 - 200; i += 2) {
        _staticShapes.pushBack(createRect(Vec2(-250 + float(i * 0), float(-307 + iy)), Vec2(40, 1)));
        iy += 14;
    }

    iy = 10;
    for (float i = -32 - 200; i < 32 - 200; i += 2) {
        _staticShapes.pushBack(createRect(Vec2(-350 + float(i * 0), float(-305 + iy)), Vec2(40, 1)));
        break;
        iy += 28;
    }

    for (int i = 0; i < 20; i++)
        _staticShapes.pushBack(createRect(Vec2(-100 + float(i * 20), -200), Vec2(10, 250)));


    movingPlat = createRect(Vec2(0, 360), Vec2(512, 512));
    movingPlat->isMovable = true;
    _staticShapes.pushBack(movingPlat);
    auto b = createRect(Vec2(-200, -340), Vec2(600, 600));
    //b->isTrigger = true;
    _staticShapes.pushBack(b);

    auto s = createSlope(Vec2(-600, -50), 24, -128);
    _staticShapes.pushBack(s);
    s = createSlope(Vec2(s->x + s->b, s->y + s->l - 0), 140, -90);
    _staticShapes.pushBack(s);

    _staticShapes.pushBack(modifySlope = createSlope(Vec2(-600, -200), 512, 64));
    _staticShapes.pushBack(createRect(Vec2(-600, 256 + 86), Vec2(32, 32)));
    //_staticShapes.pushBack(createRect(Vec2(-720, -300 + 128 + 40), Vec2(32, 0)));
    _staticShapes.pushBack(createSlope(Vec2(-720, -300), 128, 1));

    _staticShapes.pushBack(createSlope(Vec2(-880, -300), 128, 32));
    _staticShapes.pushBack(createSlope(Vec2(-880, -300), 128, -32));

    s = createSlope(Vec2(-700, 150), 32, -64);
    _staticShapes.pushBack(s);

    s = createSlope(Vec2(-700, 50), 64, -64);
    _staticShapes.pushBack(s);

    //_staticShapes.pushBack(createSlope(Vec2(s->x - s->b + 32, s->y + s->l), -32, 128));
    //_staticShapes.pushBack(createRect(Vec2(s->x - 64, s->y + s->l + 256), Vec2(32, 32)));
    //_staticShapes.push_back(createSlope(Vec2(-700, -300), 128, -64));
    //_staticShapes.push_back(createRect(Vec2(-700, -0 - 64), Vec2(32, 32)));
    //_staticShapes.push_back(createRect(Vec2(-732, -0 - 64), Vec2(32, 32)));
    //_staticShapes.push_back(createSlope(Vec2(-764, -110), 128, 64));

    _dynamicShapes.pushBack(createRectDynamic(Vec2(300, 600), Vec2(32, 32), -9.8 * 10 * 100));

    EventListenerMouse* ml = EventListenerMouse::create();
    ml->onMouseDown = AX_CALLBACK_1(ReboundPhysicsTest::onMouseDown, this);
    ml->onMouseMove = AX_CALLBACK_1(ReboundPhysicsTest::onMouseMove, this);
    Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(ml, this);

    EventListenerKeyboard* kl = EventListenerKeyboard::create();
    kl->onKeyPressed = AX_CALLBACK_2(ReboundPhysicsTest::onKeyPressed, this);
    kl->onKeyReleased = AX_CALLBACK_2(ReboundPhysicsTest::onKeyReleased, this);
    Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(kl, this);

    //std::sort(_staticShapes.begin(), _staticShapes.end(), sortCollisionShapesBasedOnTriangle);
    return true;
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

using namespace ReboundPhysics;
void ReboundPhysicsTest::step(double delta)
{
    for (auto& _ : _dynamicShapes)
    {
        //if (isGrounded)
        _->vel.x = MathUtil::lerp(_->vel.x, _->pref_vel.x, 1 * delta);

        isGrounded = false;

        //if (abs(_->pref_vel.x) > abs(_->lerp_vel.x))
        //    //_->vel.x = _->lerp_vel.x;

        //AXLOG("%f", _->internalAngle);

        _->slopeGround = nullptr;

        if (_->movableGround) {
            Vec2 oldPos = _->movableGroundPos;
            Vec2 newPos = Vec2(_->movableGround->x, _->movableGround->y);
            Vec2 size = Vec2(_->movableGround->w, _->movableGround->h);
            _->x += newPos.x - oldPos.x;
            _->y += newPos.y - oldPos.y;
            _->movableGroundPos = newPos;
            _->isMovableLerpApplied = false;

            if (!getCollisionShapeIntersect(*_, CollisionShape(newPos.x, newPos.y - 3, size.width, size.height + 6)))
            {
                float hpush = (newPos.x - oldPos.x) / delta;
                if (abs(_->vel.x) < 100)
                    _->vel.x = hpush;
                //_->vel.x = (newPos.x - oldPos.x) / delta;
                float vpush = (newPos.y - oldPos.y) / delta;
                _->vel.y = isJumping ? _->vel.y : vpush;
                _->movableGround = nullptr;
                _->isMovableLerpApplied = true;
            }
        }

        // an extent by 4 pixels is applied to the motion envelope so that
        // any motions that happen outside the envelope are picked.
        float envExtent = 32;
        {
            int size = 0;
            CollisionShape* targets[CCD_MAX_TARGETS];
            CollisionShape envelope;
            {
                //float gravity = _->vel.y + _->gravity * delta;

                float ox = _->x;
                float oy = _->y;
                float nx = _->x + _->vel.x * delta;
                float ny = _->y + _->vel.y * delta;

                CollisionShape oldPos = CollisionShape(ox, oy, _->w, _->h);
                CollisionShape newPos = CollisionShape(nx, ny, _->w, _->h);
                envelope = getRectSweepEnvelope(oldPos, newPos, envExtent);

                for (auto& __ : _staticShapes) {
                    CollisionShape rect = *__;
                    if (rect.isTriangle)
                        rect = getTriangleEnvelop(*__);
                    if (getCollisionShapeIntersect(rect, envelope)) {
                        if (size >= CCD_MAX_TARGETS)
                            break;
                        targets[size++] = __;
                        __->debugColor = Color4F::WHITE;
                    }
                    else
                        __->debugColor = Color4F::RED;
                }
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
                MAX(envelope.w * sweptVolumePrec - _->w - envExtent * 2, envelope.h * sweptVolumePrec - _->h - envExtent * 2)));

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
                    ax::Vec2 slopeMtvStateVec;

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
                    {
                        bool intersects = getCollisionShapeIntersect(*_, *targets[i]);

                        if (intersects && targets[i]->isTrigger) continue;

                        if (targets[i]->isTriangle)
                        {
                            if (getCollisionTriangleIntersect(*_, *targets[i])) {
                                //auto mtv = calculateRect2RectMTV(_, *targets[i]);
                                auto r = resolveCollisionSlope(*_, *targets[i], isJumping, lastVerticalMtv);
                                isGrounded = r.isGrounded || isGrounded;
                                if (!r.isSlope) {
                                    if (isSlopeOccupied)
                                        isGrounded = resolveCollisionRect(*_, slopeMtvState, slopeMtvStateVec).isGrounded || isGrounded;
                                    auto t = getTriangleEnvelop(*targets[i]);
                                    auto mtv = calculateRect2RectMTV(*_, t);
                                    slopeMtvState = t;
                                    slopeMtvStateVec = mtv;
                                    applyDominantSlope = true;
                                    lastVerticalMtv = mtv.y;
                                    isSlopeOccupied = true;
                                }
                                _->internalAngle = MathUtil::lerp(_->internalAngle, r.slopeAngle, 8 * delta * ccdPrecession);
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
                            auto r = resolveCollisionRect(*_, *targets[i], mtv);
                            isGrounded = r.isGrounded || isGrounded;
                            _->internalAngle = MathUtil::lerp(_->internalAngle, r.slopeAngle, 8 * delta * ccdPrecession);

                            if (intersects && targets[i]->isMovable && isGrounded) {
                                _->movableGround = targets[i];
                                _->movableGroundPos = Vec2(targets[i]->x, targets[i]->y);
                            }
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
                                slopeMtvStateVec = ax::Vec2::ZERO;
                        }
                        // // // // // // // // // // // // // // // // // // // // // // // // // // // // // //

                        isGrounded = resolveCollisionRect(*_, slopeMtvState, slopeMtvStateVec).isGrounded || isGrounded;
                    }

                    // Edge case: make sure the player gets pushed by lastVerticalMTV if this slope is before the next object with the last MTV,
                    //            we're just re-running the code to resolve the slope again before the lastVerticalMTV is reset.
                    if (_->slopeGround) {
                        auto r = resolveCollisionSlope(*_, *_->slopeGround, isJumping, lastVerticalMtv);
                        isGrounded = r.isGrounded || isGrounded;
                    }
                    // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // //

                    if (quitLater) break;
                }

            //if (!_->slopeGround)
            //    _->internalAngle = MathUtil::lerp(_->internalAngle, 0, 8 * delta);

            if (isJumping && isGrounded)
                _->vel.y = 1500 * -NUM_SIGN(_->gravity)/* MAX(1, isSlope ? 0.5 * abs(_->vel.x / 650) * slopeIncline : 1)*/;

            if (!FUZZY(debug1, 1.0, DBL_EPSILON))
                DebugBreak();

            if (isGrounded && _->pref_vel.x == 0) {
                _->vel.x = MathUtil::lerp(_->vel.x, 0, 5 * delta);
            }

            if (!isGrounded && _->pref_vel.x == 0)
                _->internalAngle = MathUtil::lerp(_->internalAngle, 0, 2 * delta);

            //if (!isGrounded)
            //    _->internalAngle = MathUtil::lerp(_->internalAngle, sin(lastPhysicsDt * 10), 2 * delta);

            if (/*size &&*/ steps > 5)
                AXLOG("  %d CCD steps swept %d shapes", steps, size);
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

void ReboundPhysicsTest::onTouchesBegan(const std::vector<ax::Touch*>& touches, ax::Event* event)
{
    for (auto&& t : touches)
    {
        AXLOG("onTouchesBegan detected, X:%f  Y:%f", t->getLocation().x, t->getLocation().y);
    }
}

void ReboundPhysicsTest::onTouchesMoved(const std::vector<ax::Touch*>& touches, ax::Event* event)
{
    for (auto&& t : touches)
    {
        AXLOG("onTouchesMoved detected, X:%f  Y:%f", t->getLocation().x, t->getLocation().y);
    }
}

void ReboundPhysicsTest::onTouchesEnded(const std::vector<ax::Touch*>& touches, ax::Event* event)
{
    for (auto&& t : touches)
    {
        AXLOG("onTouchesEnded detected, X:%f  Y:%f", t->getLocation().x, t->getLocation().y);
    }
}

void ReboundPhysicsTest::onMouseDown(Event* event)
{
    EventMouse* e = static_cast<EventMouse*>(event);

    //if (e->getMouseButton() == EventMouse::MouseButton::BUTTON_RIGHT) {
    //    rayCastTargetVector = rayCastTargetVector.rotateByAngle(ax::Vec2::ZERO, AX_DEGREES_TO_RADIANS(-10));
    //    return;
    //}

    int xPos = 0;
    for (auto& _ : _dynamicShapes) {
        auto visibleSize = _director->getVisibleSize();
        _->x = (e->getCursorX() - visibleSize.x / 2) * 2 - 16 + xPos;
        _->y = (e->getCursorY() - visibleSize.y / 2) * 2 - 16;
        xPos += 1;
    }
    //_.vel = Vec2::ZERO;
}

void ReboundPhysicsTest::onMouseUp(Event* event)
{
    EventMouse* e = static_cast<EventMouse*>(event);
}

void ReboundPhysicsTest::onMouseMove(Event* event)
{
    EventMouse* e = static_cast<EventMouse*>(event);
    mouseLoc = e->getLocationInView();
}

void ReboundPhysicsTest::onMouseScroll(Event* event)
{
    EventMouse* e = static_cast<EventMouse*>(event);
}

void ReboundPhysicsTest::onKeyPressed(EventKeyboard::KeyCode code, Event* event)
{
    if (code == EventKeyboard::KeyCode::KEY_SPACE)
    {
        isJumping = true;
    }

    if (code == EventKeyboard::KeyCode::KEY_S)
    {
        //if (!isGrounded)
        for (auto& _ : _dynamicShapes)
            _->vel.y = -5000;
    }

    if (code == EventKeyboard::KeyCode::KEY_D)
    {
        for (auto& _ : _dynamicShapes)
            _->pref_vel.x = 2500;
    }

    if (code == EventKeyboard::KeyCode::KEY_A)
    {
        for (auto& _ : _dynamicShapes)
            _->pref_vel.x = -2500;
    }

    if (code == EventKeyboard::KeyCode::KEY_T)
    {
        auto player = _dynamicShapes.at(0);

        player->x = movingPlat->x;
        player->y = movingPlat->y + 16;
    }
}

void ReboundPhysicsTest::onKeyReleased(EventKeyboard::KeyCode code, Event* event)
{
    if (code == EventKeyboard::KeyCode::KEY_SPACE)
    {
        isJumping = false;
    }

    if (code == EventKeyboard::KeyCode::KEY_D)
    {
        for (auto& _ : _dynamicShapes)
            _->pref_vel.x = 0;
    }

    if (code == EventKeyboard::KeyCode::KEY_A)
    {
        for (auto& _ : _dynamicShapes)
            _->pref_vel.x = 0;
    }
}

void ReboundPhysicsTest::update(float delta)
{
    if (!_physicsDebugNode) {
        _physicsDebugNode = DrawNode::create(1);
        //_physicsDebugNode->setScale(0.5);
        addChild(_physicsDebugNode);
    }

    _defaultCamera->setPosition(V2D::ZERO);
    _defaultCamera->setZoom(2.0f);

    if (_physicsDebugNode)
        _physicsDebugNode->clear();

    currentPhysicsDt += delta;

    //if (lastPhysicsDt < 1 && delta > 0.1)
    //    lastPhysicsDt = currentPhysicsDt;

    int physicsTPS = 1.0 / delta;

    physicsTPS = physicsTPS < 120 ? 120 : physicsTPS;

    //physicsTPS = 1000;

    //if (lastPhysicsDt + 10 < currentPhysicsDt)
    //    lastPhysicsDt = currentPhysicsDt;

    //modifySlope->l = 200;
    //modifySlope->l = 32;
    //modifySlope->b = 0 + 32 * cos(lastPhysicsDt * 1);

    while (lastPhysicsDt < currentPhysicsDt)
    {
        lastPhysicsDt += 1.0 / physicsTPS;
        step(1.0 / physicsTPS);

        movingPlat->x = 100 + 200 * sawtoothSin(lastPhysicsDt * 7);
        movingPlat->y = 0 + 170 * sawtoothCos(lastPhysicsDt * 2);
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

    for (auto& _ : _staticShapes) {
        if (_->isTriangle) {
            _physicsDebugNode->drawLine(Vec2(_->x, _->y), Vec2(_->x + _->b, _->y), _->debugColor);
            _physicsDebugNode->drawLine(Vec2(_->x, _->y), Vec2(_->x, _->y + _->l), _->debugColor);
            _physicsDebugNode->drawLine(Vec2(_->x, _->y + _->l), Vec2(_->x + _->b, _->y), _->debugColor);
        }
        else
            _physicsDebugNode->drawRect(Vec2(_->x, _->y), Vec2(_->x + _->w, _->y + _->h), _->debugColor);
    }

    for (auto& _ : _dynamicShapes)
    {
        if (!_physicsDebugNode) {
            _physicsDebugNode = DrawNode::create(2);
            addChild(_physicsDebugNode);
        }

        Vec2 bl = Vec2(_->x + 1, _->y + 1);
        Vec2 br = Vec2(_->x + _->w - 1, _->y + 1);

        Vec2 tl = Vec2(_->x + 1, _->y + _->h - 1);
        Vec2 tr = Vec2(_->x + 1 + _->w - 1, _->y + _->h - 1);

        bl = bl.rotateByAngle(Vec2(_->x + _->w / 2, _->y + _->h / 2), -_->internalAngle);
        br = br.rotateByAngle(Vec2(_->x + _->w / 2, _->y + _->h / 2), -_->internalAngle);
        tl = tl.rotateByAngle(Vec2(_->x + _->w / 2, _->y + _->h / 2), -_->internalAngle);
        tr = tr.rotateByAngle(Vec2(_->x + _->w / 2, _->y + _->h / 2), -_->internalAngle);

        _physicsDebugNode->drawLine(bl, br, Color4B::GREEN);
        _physicsDebugNode->drawLine(bl, tl, Color4B::GREEN);
        _physicsDebugNode->drawLine(tl, tr, Color4B::GREEN);
        _physicsDebugNode->drawLine(br, tr, Color4B::GREEN);

        _physicsDebugNode->drawCircle(Vec2(_->x + _->w / 2, _->y + _->h / 2), _->w / 2 - 1, 0, 12, false, Color4B::RED);
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

void ReboundPhysicsTest::menuCloseCallback(Ref* sender)
{
    // Close the axmol game scene and quit the application
    _director->end();

    /*To navigate back to native iOS screen(if present) without quitting the application  ,do not use
     * _director->end() as given above,instead trigger a custom event created in RootViewController.mm
     * as below*/

     // EventCustom customEndEvent("game_scene_close_event");
     //_eventDispatcher->dispatchEvent(&customEndEvent);
}
