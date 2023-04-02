#include "uiContainer.h"

CustomUi::Container* CustomUi::Container::create()
{
    Container* ret = new Container();
    if (ret->init())
    {
        ret->setAsContainer();
        ret->autorelease();
    }
    else
    {
        AX_SAFE_DELETE(ret);
    }
    return ret;
}

bool CustomUi::Container::update(cocos2d::Vec2 mouseLocationInView, cocos2d::Camera* cam)
{
    auto& list = getChildren();
    for (int i = list.size() - 1; i > -1; i--)
    {
        if (((GUI*)list.at(i))->update(mouseLocationInView, cam))
            return _isHitSwallowed = true;
    }
    return _isHitSwallowed = false;
}

bool CustomUi::Container::click(cocos2d::Vec2 mouseLocationInView, cocos2d::Camera* cam)
{
    auto& list = getChildren();
    bool isClickSwallowed = false;
    for (int i = list.size() - 1; i > -1; i--)
    {
        if (((GUI*)list.at(i))->click(mouseLocationInView, cam))
            return true;
    }
    return false;
}

void CustomUi::Container::onEnable()
{
}

void CustomUi::Container::onDisable()
{
}
