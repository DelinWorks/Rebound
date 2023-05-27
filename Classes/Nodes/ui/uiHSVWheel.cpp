#include "uiHSVWheel.h"

CustomUi::HSVWheel* CustomUi::HSVWheel::create(float scale)
{
    CustomUi::HSVWheel* ret = new CustomUi::HSVWheel();
    if (ret->init(scale))
    {
        ret->autorelease();
    }
    else
    {
        AX_SAFE_DELETE(ret);
    }
    return ret;
}

bool CustomUi::HSVWheel::init(float scale)
{
    addComponent((new UiRescaleComponent(Director::getInstance()->getVisibleSize()))->enableDesignScaleIgnoring());
    lp = Node::create();
    lp->setPositionX(-50);
    button = createPlaceholderButton();
    resetButton = createPlaceholderButton();
    wButton = createPlaceholderButton();
    sqButton = createPlaceholderButton();
    wheel = ax::Sprite::createWithSpriteFrameName("hsv_wheel");
    square = ax::Sprite::createWithSpriteFrameName("hsv_square");
    wheelHandle = ax::Sprite::createWithSpriteFrameName("hsv_handle");
    squareHandle = ax::Sprite::createWithSpriteFrameName("hsv_handle");
    square->setProgramState(GameUtils::createGPUProgram("hsv_square.frag", "default.vert"));
    wButton->setContentSize(wheel->getContentSize());
    sqButton->setContentSize({ 142,142 });
    addChild(button, -1);
    addChild(lp);
    lp->addChild(wButton, -1);
    lp->addChild(sqButton, -1);
    lp->addChild(wheel);
    lp->addChild(square);
    lp->addChild(wheelHandle);
    lp->addChild(squareHandle);
    auto colorP = GameUtils::createGPUProgram("hsv_distinct_alpha.frag", "default.vert");
    oldColor = ax::Sprite::create("pixel.png");
    oldColor->setProgramState(colorP);
    oldColor->setContentSize(Vec2(85, 50));
    oldColor->addChild(resetButton);
    hookPlaceholderButtonToNode(oldColor, resetButton);
    newColor = ax::Sprite::create("pixel.png");
    newColor->setProgramState(colorP);
    newColor->setContentSize(Vec2(85, 50));
    oldColor->setPositionY(75);
    oldColor->setPositionX(128);
    newColor->setPositionY(25);
    newColor->setPositionX(128);
    addChild(oldColor);
    addChild(newColor);
    wheelHandle->setPosition(Vec2(0, 114));
    squareHandle->setPosition(Vec2(142 / 2.0, 142 / 2.0));
    wheelHandle->setScale(_PxArtMultiplier);
    squareHandle->setScale(_PxArtMultiplier);
    setContentSize(wheel->getContentSize() + Vec2(100, 0));
    button->setContentSize(getContentSize());
    opacity = Slider::create();
    opacity->DenyRescaling();
    opacity->init({ 50, 10 });
    opacity->setRotation(-90);
    opacity->setPositionX(128);
    opacity->setPositionY(-62);
    opacity->setValue(1);
    opacity->_callback = [=](float alpha, Slider* target) {
        hsv.a = alpha;
        updateColorValues();
    };
    hsv.fromRgba(Color4F::RED);
    sqHsv.fromRgba(Color4F::RED);
    square->setColor(sqHsv.toColor3B());
    oldColor->setColor(sqHsv.toColor3B());
    newColor->setColor(sqHsv.toColor3B());
    addChild(opacity);
    return true;
}

void CustomUi::HSVWheel::update(f32 dt)
{
    opacity->update(dt);
}

void CustomUi::HSVWheel::updateColorValues()
{
    wheelHandle->setPosition(Vec2(114, 0).rotateByAngle(ax::Vec2::ZERO, AX_DEGREES_TO_RADIANS(180 - (hsv.h + 90))));
    auto saturation = Math::map(hsv.s, 0, 1, -71, 71);
    auto value = Math::map(hsv.v, 0, 1, -71, 71);
    squareHandle->setPosition(Vec2(saturation, value));
    square->setColor(sqHsv.toColor3B());
    newColor->setColor(hsv.toColor3B());
    newColor->setOpacity(hsv.a * 255);
}

void CustomUi::HSVWheel::updateColorValues(Color4F color)
{
    hsv.fromRgba(color);
    opacity->setValue(hsv.a);
    sqHsv.h = hsv.h;
    oldColor->setColor(sqHsv.toColor3B());
    oldColor->setOpacity(color.a * 255);
    updateColorValues();
}

bool CustomUi::HSVWheel::hover(cocos2d::Vec2 mouseLocationInView, Camera* cam)
{
    if (opacity->hover(mouseLocationInView, cam)) return true;
    if (_pCurrentHeldItem == this) {
        if (isPickingWheel) {
            Vec3 p;
            wButton->hitTest(mouseLocationInView, cam, &p);
            auto normal = Vec2(p.x, p.y);
            normal -= wButton->getContentSize() / 2;
            auto theta = atan2(normal.y, normal.x);
            auto angle = AX_RADIANS_TO_DEGREES(theta) - 90;
            if (angle < 0) angle += 360; // -180 -> 360
            angle = 360 - angle; // 0 -> 360
            hsv.h = angle;
            sqHsv.h = angle;
        }
        if (isPickingSquare) {
            Vec3 p;
            sqButton->hitTest(mouseLocationInView, cam, &p);
            Vec2 pos(p.x, p.y);
            pos.x = Math::clamp(Math::map(pos.x, 0, sqButton->getContentSize().x, -71, 71), -71, 71);
            pos.y = Math::clamp(Math::map(pos.y, 0, sqButton->getContentSize().y, -71, 71), -71, 71);
            auto saturation = Math::map(pos.x, -71, 71, 0, 1);
            auto value = Math::map(pos.y, -71, 71, 0, 1);
            hsv.s = saturation;
            hsv.v = value;
        }
        updateColorValues();
        return true;
    }
    else {
        isPickingWheel = false;
        isPickingSquare = false;
        if (isEnabled() && button->hitTest(mouseLocationInView, cam, nullptr)) {
            if (wButton->hitTest(mouseLocationInView, cam, nullptr)) {
                isPickingWheel = true;
                if (sqButton->hitTest(mouseLocationInView, cam, nullptr)) {
                    isPickingSquare = true;
                    isPickingWheel = false;
                }
                else if (wButton->getWorldPosition().distance(mouseLocationInView) < 100 * getScale())
                    isPickingWheel = false;
            }
            return true;
        }
    }
    return false;
}

bool CustomUi::HSVWheel::press(cocos2d::Vec2 mouseLocationInView, Camera* cam)
{
    if (isEnabled() && button->hitTest(mouseLocationInView, cam, nullptr)) {
        if (resetButton->hitTest(mouseLocationInView, cam, nullptr)) {
            auto c = Color4F(oldColor->getColor());
            c.a = oldColor->getOpacity() / 255.0;
            updateColorValues(c);
            return true;
        }
        if (opacity->press(mouseLocationInView, cam)) return true;
        if (wButton->hitTest(mouseLocationInView, cam, nullptr)
            && wButton->getWorldPosition().distance(mouseLocationInView) < 130 * getScale()) {
            _pCurrentHeldItem = this;
        }
        return true;
    }
    return false;
}

bool CustomUi::HSVWheel::release(cocos2d::Vec2 mouseLocationInView, Camera* cam)
{
    if (opacity->release(mouseLocationInView, cam)) return true;
    if (_pCurrentHeldItem == this) {
        _pCurrentHeldItem = nullptr;
        return true;
    }
    return false;
}
