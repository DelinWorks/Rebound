#include "uiButton.h"

CustomUi::Button* CustomUi::Button::create()
{
    CustomUi::Button* ret = new CustomUi::Button();
    if (((Node*)ret)->init())
    {
        ret->autorelease();
    }
    else
    {
        AX_SAFE_DELETE(ret);
    }
    return ret;
}

void CustomUi::Button::init(
    cocos2d::Rect _capinsets, cocos2d::Size _contentsize,
    std::string _normal_sp, ax::Color3B _normal_color)
{
    normal_sp = _normal_sp;
    capinsets = _capinsets;
    normal_color = _normal_color;
    sprite = cocos2d::ui::Scale9Sprite::createWithSpriteFrameName(normal_sp, capinsets);
    sprite->setContentSize(_contentsize);
    addChild(sprite);
}

bool CustomUi::Button::update(cocos2d::Vec2 mouseLocationInView, ax::Camera* cam)
{
    return false;
}

void CustomUi::Button::onEnable()
{
}

void CustomUi::Button::onDisable()
{
}

bool CustomUi::Button::click(cocos2d::Vec2 mouseLocationInView, Camera* cam)
{
    return false;
}
