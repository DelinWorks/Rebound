#include "uiSlider.h"

CUI::Slider* CUI::Slider::create()
{
    CUI::Slider* ret = new CUI::Slider();
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

void CUI::Slider::setValue(float v, bool call)
{
    currentValue = v;
    slider->setPercent(v);
    if (call) _callback(v, this);
}

void CUI::Slider::init(Size _contentsize)
{
    init(
        "slider_bar",
        "slider_progress",
        "slider_knob",
        ADVANCEDUI_SLIDER_CAP_INSETS,
        _contentsize
    );
}

void CUI::Slider::init(
    std::string_view barFrame,
    std::string_view progressFrame,
    std::string_view knob,
    ax::Rect _capinsets,
    ax::Size _contentsize)
{
    if (_rescalingAllowed)
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

void CUI::Slider::update(f32 dt)
{
    slider->setEnabled(isUiHovered());
    auto dSize = getDynamicContentSize();
    setContentSize(dSize + getUiPadding() + SLIDER_HITBOX_CORNER_TOLERANCE / 1.5);
    HoverEffectGUI::update(dt, getContentSize());
    if (_pCurrentHeldItem == this) {
        auto v = slider->getPercent();
        if (currentValue != v) {
            currentValue = v;
            _callback(currentValue, this);
        }
    }
}

bool CUI::Slider::hover(cocos2d::Vec2 mouseLocationInView, Camera* cam)
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

void CUI::Slider::focus()
{
}

void CUI::Slider::defocus()
{
}

void CUI::Slider::onEnable()
{
}

void CUI::Slider::onDisable()
{
}

bool CUI::Slider::press(cocos2d::Vec2 mouseLocationInView, Camera* cam)
{
    isHeld = button->hitTest(mouseLocationInView, cam, _NOTHING);
    if (isHeld) _pCurrentHeldItem = this;
    return isHeld;
}

bool CUI::Slider::release(cocos2d::Vec2 mouseLocationInView, Camera* cam)
{
    isHeld = false;
    hover(mouseLocationInView, cam);
    return false;
}

Size CUI::Slider::getDynamicContentSize()
{
    return slider->getContentSize() * _PxArtMultiplier;
}

Size CUI::Slider::getFitContentSize()
{
    return slider->getContentSize() * 2 + SLIDER_HITBOX_CORNER_TOLERANCE;
}
