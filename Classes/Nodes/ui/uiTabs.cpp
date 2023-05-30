#include "uiTabs.h"

CustomUi::Tabs::Tabs(Vec2 _prefferedSize)
{
    elementCont = CustomUi::Container::create();
    scrollCont = CustomUi::Container::create();

    setBackgroundSprite({10, 5});

    prefferedSize = _prefferedSize;
    setContentSize(_prefferedSize);

    elementCont->setLayout(FlowLayout(SORT_HORIZONTAL, STACK_RIGHT, 0, 0, false));
    elementCont->setConstraint(DependencyConstraint(this, LEFT));
    addChild(elementCont);

    scrollCont->setStatic();
    scrollCont->setContentSize(Vec2(20, _prefferedSize.y));
    scrollCont->setLayout(FlowLayout(SORT_HORIZONTAL, STACK_CENTER, 8));
    setChildRight(scrollCont);

    auto rightB = CustomUi::Button::create();
    rightB->initIcon("editor_arrow_right", {2, 5});
    rightB->_callback = [&](Button* target) {
        Vec2 pos = ePos - Vec2(elementCont->getChildren().at(0)->getContentSize().x / 2, 0);
        pos.x = Math::clamp(pos.x, elementCont->getContentSize().x / -2 + getContentSize().x / 2 - scrollCont->getContentSize().x, getContentSize().x / -2);
        ePos = pos;
        elementCont->stopAllActions();
        elementCont->runAction(EaseExponentialOut::create(MoveTo::create(0.5, pos)));
    };
    scrollCont->addChild(rightB);

    auto leftB = CustomUi::Button::create();
    leftB->initIcon("editor_arrow_left", {2, 5});
    leftB->_callback = [&](Button* target) {
        Vec2 pos = ePos + Vec2(elementCont->getChildren().at(0)->getContentSize().x / 2, 0);
        pos.x = Math::clamp(pos.x, elementCont->getContentSize().x / -2 + getContentSize().x / 2 - scrollCont->getContentSize().x, getContentSize().x / -2);
        ePos = pos;
        elementCont->stopAllActions();
        elementCont->runAction(EaseExponentialOut::create(MoveTo::create(0.5, pos)));
    };
    scrollCont->addChild(leftB);
}

CustomUi::Tabs* CustomUi::Tabs::create(Vec2 _prefferedSize)
{
    Tabs* ref = new Tabs(_prefferedSize);
    if (ref->init())
    {
        ref->autorelease();
    }
    else
    {
        AX_SAFE_DELETE(ref);
    }
    return ref;
}

void CustomUi::Tabs::calculateContentBoundaries()
{
    if (_background)
        _background->setContentSize(getContentSize() + _backgroundPadding);

    if (_bgButton)
        _bgButton->setContentSize(getContentSize());
}

void CustomUi::Tabs::updateLayoutManagers(bool recursive)
{
    Container::updateLayoutManagers();
    elementCont->updateLayoutManagers();
    ePos = elementCont->getPosition();
}

void CustomUi::Tabs::addElement(std::wstring e)
{
    auto b = CustomUi::Button::create();
    b->init(e, TTFFS);
    elementCont->addChild(b);
}
