#include "uiContainer.h"

CustomUi::CustomUiContainer* CustomUi::CustomUiContainer::create()
{
    CustomUiContainer* ret = new CustomUiContainer();
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

bool CustomUi::CustomUiContainer::update(cocos2d::Vec2 mouseLocationInView, cocos2d::Camera* cam)
{
    auto& list = getChildren();
    for (int i = list.size() - 1; i > -1; i--)
    {
        if (((GUI*)list.at(i))->update(mouseLocationInView, cam))
            return isHitSwallowed = true;
    }
    return isHitSwallowed = false;
}

bool CustomUi::CustomUiContainer::click(cocos2d::Vec2 mouseLocationInView, cocos2d::Camera* cam)
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

void CustomUi::CustomUiContainer::onEnable()
{
}

void CustomUi::CustomUiContainer::onDisable()
{
}
