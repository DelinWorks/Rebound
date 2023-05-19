#include "uiSlider.h"

CustomUi::Slider* CustomUi::Slider::create()
{
    CustomUi::Slider* ret = new CustomUi::Slider();
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

void CustomUi::Slider::init(Size _contentsize)
{
    init(
        "slider_bar",
        "slider_progress",
        "slider_knob",
        ADVANCEDUI_SLIDER_CAP_INSETS,
        _contentsize
    );
}

void CustomUi::Slider::init(
    std::string_view barFrame,
    std::string_view progressFrame,
    std::string_view knob,
    ax::Rect _capinsets,
    ax::Size _contentsize)
{
    addComponent((new UiRescaleComponent(Director::getInstance()->getVisibleSize()))->enableDesignScaleIgnoring());
    slider = ui::Slider::create();
    slider->setScale9Enabled(true);
    slider->loadSlidBallTextures(knob, knob, knob, ui::Widget::TextureResType::PLIST);
    slider->loadBarTexture(barFrame, ui::Widget::TextureResType::PLIST);
    slider->loadProgressBarTexture(progressFrame, ui::Widget::TextureResType::PLIST);
    slider->setCapInsetsBarRenderer(_capinsets);
    slider->setCapInsetProgressBarRenderer(_capinsets);
    slider->ignoreContentAdaptWithSize(true);
    slider->setContentSize(_contentsize);
    slider->setScale(_PxArtMultiplier);
    button = createPlaceholderButton();
    button->setContentSize(_contentsize * _PxArtMultiplier + SLIDER_HITBOX_CORNER_TOLERANCE);
    addChild(button);
    addChild(slider);
    _callback = [](float, Slider*) {};
    update(0);
}

void CustomUi::Slider::update(f32 dt)
{
    slider->setEnabled(isUiHovered());
    auto dSize = getDynamicContentSize();
    setContentSize(dSize + getUiPadding() + SLIDER_HITBOX_CORNER_TOLERANCE / 1.5);
    HoverEffectGUI::update(dt);
    if (_pCurrentHeldItem == this) {
        auto v = slider->getPercent();
        if (currentValue != v) {
            currentValue = v;
            _callback(currentValue, this);
            RLOG("Slider: {}", currentValue * 255);
        }
    }
}

bool CustomUi::Slider::hover(cocos2d::Vec2 mouseLocationInView, Camera* cam)
{
    if (isEnabled())
    {
        if (!_pCurrentHeldItem) {
            setUiHovered(button->hitTest(mouseLocationInView, cam, _NOTHING) || isHeld);
            hover_cv.setValue(isUiHovered());
        }

        if (hover_cv.isChanged())
            HoverEffectGUI::hover();
    }

    return hover_cv.getValue();
}

void CustomUi::Slider::focus()
{
}

void CustomUi::Slider::defocus()
{
}

void CustomUi::Slider::onEnable()
{
}

void CustomUi::Slider::onDisable()
{
}

bool CustomUi::Slider::press(cocos2d::Vec2 mouseLocationInView, Camera* cam)
{
    isHeld = button->hitTest(mouseLocationInView, cam, _NOTHING);
    if (isHeld) _pCurrentHeldItem = this;
    return isHeld;
}

bool CustomUi::Slider::release(cocos2d::Vec2 mouseLocationInView, Camera* cam)
{
    isHeld = false;
    hover(mouseLocationInView, cam);
    return false;
}

Size CustomUi::Slider::getDynamicContentSize()
{
    return slider->getContentSize() * _PxArtMultiplier;
}

Size CustomUi::Slider::getFitContentSize()
{
    return slider->getContentSize() * 2 + SLIDER_HITBOX_CORNER_TOLERANCE;
}
