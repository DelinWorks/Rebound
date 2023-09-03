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

    _pw->_staticShapes.pushBack(createRect(Vec2(UINT16_MAX / -2, -297), Vec2(float(UINT16_MAX), 0)));

    float iy = 10;
    for (float i = -32 - 200; i < 32 - 200; i += 2) {
        _pw->_staticShapes.pushBack(createRect(Vec2(-250 + float(i * 0), float(-307 + iy)), Vec2(40, 6)));
        iy += 14;
    }

    iy = 10;
    for (float i = -32 - 200; i < 32 - 200; i += 2) {
        _pw->_staticShapes.pushBack(createRect(Vec2(-350 + float(i * 0), float(-305 + iy)), Vec2(40, 1)));
        break;
        iy += 28;
    }

    for (int i = 0; i < 20; i++)
        _pw->_staticShapes.pushBack(createRect(Vec2(-100 + float(i * 20), -200), Vec2(10, 250)));


    _pw->movingPlat = createRect(Vec2(0, 360), Vec2(512, 512));
    _pw->movingPlat->isMovable = true;
    _pw->_staticShapes.pushBack(_pw->movingPlat);
    auto b = createRect(Vec2(-200, -340), Vec2(600, 600));
    //b->isTrigger = true;
    _pw->_staticShapes.pushBack(b);

    auto s = createSlope(Vec2(-600, -50), 24, -128);
    _pw->_staticShapes.pushBack(s);
    s = createSlope(Vec2(s->x + s->b, s->y + s->l - 0), 140, -90);
    _pw->_staticShapes.pushBack(s);

    _pw->_staticShapes.pushBack(_pw->modifySlope = createSlope(Vec2(-600, -240), 256, 64));
    //_pw->_staticShapes.pushBack(createRect(Vec2(-600, 32 + 40), Vec2(32, 32)));
    //_staticShapes.pushBack(createRect(Vec2(-720, -300 + 128 + 40), Vec2(32, 0)));
    _pw->_staticShapes.pushBack(createSlope(Vec2(-720, -300), 128, 1));

    _pw->_staticShapes.pushBack(createSlope(Vec2(-880, -300), 128, 32));
    _pw->_staticShapes.pushBack(createSlope(Vec2(-880, -300), 128, -32));

    _pw->_staticShapes.pushBack(createSlope(Vec2(-1090, -300), 256, 64));
    _pw->_staticShapes.pushBack(createSlope(Vec2(-1090, -300 + 512), -256, 64));

    s = createSlope(Vec2(-670, 113), 128, -64);
    _pw->_staticShapes.pushBack(s);

    s = createSlope(Vec2(-670, 54), 64, -64);
    _pw->_staticShapes.pushBack(s);

    //for (int i = 0; i < 8192; i++)
    //    _staticShapes.pushBack(createRect(V2D(-800, 250), V2D(64, 64)));

    //_staticShapes.pushBack(createSlope(Vec2(s->x - s->b + 32, s->y + s->l), -32, 128));
    //_staticShapes.pushBack(createRect(Vec2(s->x - 64, s->y + s->l + 256), Vec2(32, 32)));
    //_staticShapes.push_back(createSlope(Vec2(-700, -300), 128, -64));
    //_staticShapes.push_back(createRect(Vec2(-700, -0 - 64), Vec2(32, 32)));
    //_staticShapes.push_back(createRect(Vec2(-732, -0 - 64), Vec2(32, 32)));
    //_staticShapes.push_back(createSlope(Vec2(-764, -110), 128, 64));

    _pw->_dynamicShapes.pushBack(createRectDynamic(Vec2(300, 600), Vec2(32, 32), -9.8 * 10 * 100));

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

    if (code == EventKeyboard::KeyCode::KEY_S)
    {
        //if (!isGrounded)
        for (auto& _ : _pw->_dynamicShapes)
            _->vel.y = -5000;
    }

    if (code == EventKeyboard::KeyCode::KEY_D)
    {
        for (auto& _ : _pw->_dynamicShapes)
            _->pref_vel.x = 2500;
    }

    if (code == EventKeyboard::KeyCode::KEY_A)
    {
        for (auto& _ : _pw->_dynamicShapes)
            _->pref_vel.x = -2500;
    }

    if (code == EventKeyboard::KeyCode::KEY_T)
    {
        auto player = _pw->_dynamicShapes.at(0);

        player->x = _pw->movingPlat->x;
        player->y = _pw->movingPlat->y + 16;
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
    _pw->update(delta);

    _defaultCamera->setPosition(V2D::ZERO);
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
