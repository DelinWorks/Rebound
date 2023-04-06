#include "uiContainer.h"

CustomUi::Container* CustomUi::Container::createNullLayout()
{
    Container* ref = new Container();
    if (ref->init())
    {
        ref->setAsContainer();
        ref->autorelease();
    }
    else
    {
        AX_SAFE_DELETE(ref);
    }
    return ref;
}

CustomUi::Container* CustomUi::Container::create(BorderLayout border)
{
    auto ref = createNullLayout();
    if (ref) {
        ref->addComponent((new UiRescaleComponent(Director::getInstance()->getVisibleSize()))
            ->setBorderLayout(border));
        return ref;
    }
    else return nullptr;
}

bool CustomUi::Container::hover(cocos2d::Vec2 mouseLocationInView, cocos2d::Camera* cam)
{
    _isHitSwallowed = false;
    auto& list = getChildren();
    for (int i = list.size() - 1; i > -1; i--)
    {
        auto n = ((GUI*)list.at(i));
        if (n->hover(_isHitSwallowed ? Vec2(INFINITY, INFINITY) : mouseLocationInView, cam))
            _isHitSwallowed = true;
    }
    return _isHitSwallowed;
}

bool CustomUi::Container::click(cocos2d::Vec2 mouseLocationInView, cocos2d::Camera* cam)
{
    auto& list = getChildren();
    bool isClickSwallowed = false;
    for (int i = list.size() - 1; i > -1; i--)
    {
        auto n = ((GUI*)list.at(i));
        if (n->click(isClickSwallowed ? Vec2(INFINITY, INFINITY) : mouseLocationInView, cam)) {
            isClickSwallowed = true;
        }
    }
    return isClickSwallowed;
}

void CustomUi::Container::onEnable()
{
}

void CustomUi::Container::onDisable()
{
}
