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

    _pw = PhysicsWorld::create();
    addChild(_pw);

    _pw->_staticShapes.push_back(_pw->ground = createRect(Vec2(1000000 / -2, -297), Vec2(float(1000000), 0)));
    _pw->_staticShapes.push_back(_pw->ground = createRect(Vec2(10000000 / -2, -50000000), Vec2(float(10000000), 0)));

    _pw->_staticShapes.push_back(createRect(Vec2(1000000 / -2, -297), Vec2(1, 32)));

    float iy = 10;
    //for (float i = -32 - 200; i < 320 - 200; i += 2) {
    //    _pw->_staticShapes.push_back(createRect(Vec2(-250 + float(i * 0), float(-307 + iy)), Vec2(40, 5)));
    //    iy += 14;
    //}

    iy = 10;
    for (float i = -32 - 200; i < 32 - 200; i += 2) {
        _pw->_staticShapes.push_back(createRect(Vec2(-350 + float(i * 0), float(-307 + iy)), Vec2(36, 32)));
        break;
        iy += 28;
    }

    for (int i = 0; i < 20; i++)
        _pw->_staticShapes.push_back(createRect(Vec2(-100 + float(i * 20), -200), Vec2(10, 250)));

    auto b = createRect(Vec2(-200, -340), Vec2(600, 600));
    //b->isTrigger = true;
    _pw->_staticShapes.push_back(b);

    auto s = createSlope(Vec2(-600, -50), 24, -134);
    _pw->_staticShapes.push_back(s);
    s = createSlope(Vec2(s->x + s->b, s->y + s->l - 0), 140, -90);
    _pw->_staticShapes.push_back(s);

    _pw->_staticShapes.push_back(_pw->modifySlope = createSlope(Vec2(-600, -280), 262, 64));
    //_pw->_staticShapes.push_back(createRect(Vec2(-600, 32 + 40), Vec2(32, 32)));
    //_staticShapes.push_back(createRect(Vec2(-720, -300 + 128 + 40), Vec2(32, 0)));
    _pw->_staticShapes.push_back(createSlope(Vec2(-720, -300), 128, 1));

    _pw->_staticShapes.push_back(createSlope(Vec2(-880, -300), 128, 32));
    _pw->_staticShapes.push_back(createSlope(Vec2(-880, -300), 128, -32));

    //_pw->_staticShapes.push_back(createSlope(Vec2(-1090, -300 + 512), -256, 80000));
    _pw->_staticShapes.push_back(createSlope(Vec2(-1090, -300), 256, 64));

    _pw->_staticShapes.push_back(createSlope(Vec2(-1890, -265), 1024, 512));

    s = createSlope(Vec2(-670, 113), 128, -64);
    _pw->_staticShapes.push_back(s);

    s = createSlope(Vec2(-670, 54), 64, -64);
    _pw->_staticShapes.push_back(s);

    for (int i = 0; i < 10000; i++)
    {
        auto m = createRect(Vec2(-1000 + RandomHelper::random_real<F32>(-100, 100), 360 + 20 + RandomHelper::random_real<F32>(-50, 50)), Vec2(20, RandomHelper::random_real<F32>(100, 200)));
        _pw->movingPlat.push_back(m);
        m->isMovable = true;
        _pw->_staticShapes.push_back(m);

        m = createRect(Vec2(-1000 + 200 + RandomHelper::random_real<F32>(-100, 100), 360 + RandomHelper::random_real<F32>(-50, 50)), Vec2(80000, 0));
        _pw->movingPlat.push_back(m);
        m->isMovable = true;
        _pw->_staticShapes.push_back(m);
    }

    fastShape = createRect(V2D(0, 0), V2D(0, 64));
    fastShape->isMovable = true;
    _pw->_staticShapes.push_back(fastShape);

    for (int i = 0; i < 10; i++)
        _pw->_staticShapes.push_back(createRect(V2D(700 + 284 * i, -100 + 0 * i), V2D(32, 48)));

    _pw->partition();

    //for (int i = 0; i < 8192; i++)
    //    _staticShapes.push_back(createRect(V2D(-800, 250), V2D(64, 64)));

    //_staticShapes.push_back(createSlope(Vec2(s->x - s->b + 32, s->y + s->l), -32, 128));
    //_staticShapes.push_back(createRect(Vec2(s->x - 64, s->y + s->l + 256), Vec2(32, 32)));
    //_staticShapes.push_back(createSlope(Vec2(-700, -300), 128, -64));
    //_staticShapes.push_back(createRect(Vec2(-700, -0 - 64), Vec2(32, 32)));
    //_staticShapes.push_back(createRect(Vec2(-732, -0 - 64), Vec2(32, 32)));
    //_staticShapes.push_back(createSlope(Vec2(-764, -110), 128, 64));

    _pw->_dynamicShapes.push_back(createRectDynamic(Vec2(300, 600), Vec2(64, 64), -9.8 * 10 * 100));

    test123 = Sprite::create("salene.png"sv);
    addChild(test123);
    test123->setPositionX(-1000);
    test123->setPositionY(300);
    test123->setOpacity(50);

    //auto s1 = createRect(Vec2(UINT16_MAX / -2, -297), Vec2(float(UINT16_MAX), 1));
    //auto v = chunkGetCoverArea(*s1);

    //for (auto& p : v)
    //    RLOG("{} {}", p.x, p.y);

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

using namespace ReboundPhysics;

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
    for (auto& _ : _pw->_dynamicShapes) {
        auto visibleSize = _director->getVisibleSize();
        setBodyPosition(*_, V2D((e->getCursorX() - visibleSize.x / 2) * 2 - 16 + xPos, (e->getCursorY() - visibleSize.y / 2) * 2 - 16), false);
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
        _pw->isJumping = true;
    }

    if (code == EventKeyboard::KeyCode::KEY_GRAVE)
    {
        isPaused = !isPaused;
    }

    if (code == EventKeyboard::KeyCode::KEY_S)
    {
        //if (!isGrounded)
        for (auto& _ : _pw->_dynamicShapes)
            _->vel.y = -50000000;
    }

    if (code == EventKeyboard::KeyCode::KEY_D)
    {
        for (auto& _ : _pw->_dynamicShapes)
            _->pref_vel.x = 1500;
    }

    if (code == EventKeyboard::KeyCode::KEY_A)
    {
        for (auto& _ : _pw->_dynamicShapes)
            _->pref_vel.x = -1500;
    }

    if (code == EventKeyboard::KeyCode::KEY_T)
    {
        auto player = _pw->_dynamicShapes.at(0);

        player->x = 10000000 / -2;
        player->y = -50000000 + 256;
    }
}

void ReboundPhysicsTest::onKeyReleased(EventKeyboard::KeyCode code, Event* event)
{
    if (code == EventKeyboard::KeyCode::KEY_SPACE)
    {
        _pw->isJumping = false;
    }

    if (code == EventKeyboard::KeyCode::KEY_D)
    {
        for (auto& _ : _pw->_dynamicShapes)
            _->pref_vel.x = 0;
    }

    if (code == EventKeyboard::KeyCode::KEY_A)
    {
        for (auto& _ : _pw->_dynamicShapes)
            _->pref_vel.x = 0;
    }
}

void ReboundPhysicsTest::update(float delta)
{
    _pw->_moveTargetCount = 0;

    //delta *= isPaused ? 10 : 1;

    for (int i = 0; i < _pw->movingPlat.size(); i++)
        setShapePosition(_pw, *_pw->movingPlat[i], V2D(_pw->movingPlat[i]->x + delta * (isPaused ? 4010 : 512), _pw->movingPlat[i]->y) /*V2D(500 + 1000 * sin(_pw->lastPhysicsDt * 1), 0 + 100 * cos(_pw->lastPhysicsDt * 2))*/);

    test123->setPositionX(test123->getPositionX() + delta * (isPaused ? 4010 : 512));

    setShapePosition(_pw, *fastShape, V2D(900 + fmod(_pw->currentPhysicsDt * 100000, 1000), -300));

    //RLOG("PosX {}", test123->getPositionX());

    _pw->update(delta);

    //int totalSize = 0;//_pw->_staticShapeChunks.size();
    //for (auto& p : _pw->_staticShapeChunks)
    //        totalSize += p.second._capacity;

    ////for (auto& e : _pw->_staticShapes)
    ////    totalSize++;
    ////    
    ////for (auto& e : _pw->_dynamicShapes)
    ////    totalSize++;

    //RLOG("{} size", totalSize);

    auto p = _pw->_dynamicShapes.at(0);
    _defaultCamera->setPosition(V2D(p->x, p->y) + V2D(p->w, p->h) / 2);
    //_defaultCamera->setPosition(V2D::ZERO);
    _defaultCamera->setZoom(2.0f);
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
