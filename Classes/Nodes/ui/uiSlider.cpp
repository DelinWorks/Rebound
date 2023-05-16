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
        "slider_bar",
        "slider_knob",
        ADVANCEDUI_SLIDER_CAP_INSETS,
        _contentSize
    );
}

void CustomUi::Slider::init(
    std::string_view barFrame,
    std::string_view progressFrame,
    std::string_view knob,
    ax::Rect _capinsets,
    ax::Size _contentsize)
{
    slider = ui::Slider::create();
    slider->setScale9Enabled(true);
    slider->loadSlidBallTextures(knob, knob, knob, ui::Widget::TextureResType::PLIST);
    slider->setCapInsetsBarRenderer(_capinsets);
    slider->setCapInsetProgressBarRenderer(_capinsets);
    slider->setContentSize(_contentsize);
    button = createPlaceholderButton();
    addChild(slider);
}

void CustomUi::Slider::update(f32 dt)
{
}

bool CustomUi::Slider::hover(cocos2d::Vec2 mouseLocationInView, Camera* cam)
{
    return false;
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

Size CustomUi::Slider::getDynamicContentSize()
{
    return Size();
}
