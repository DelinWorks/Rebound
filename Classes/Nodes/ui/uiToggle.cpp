#include "uiToggle.h"

CustomUi::Toggle* CustomUi::Toggle::create()
{
    CustomUi::Toggle* ret = new CustomUi::Toggle();
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

void CustomUi::Toggle::init(std::wstring _text, Size _contentsize)
{
    scheduleUpdate();
    cont = CustomUi::Container::create();
    auto fl = FlowLayout();
    fl.spacing = 10;
    fl.reverseStack = false;
    cont->setLayout(fl);
    knob = Button::create();
    knob->initIcon(isToggled ? "toggle_selected" : "toggle_non");
    label = Label::create();
    label->init(_text, TTFFS);
    button = createPlaceholderButton();
    cont->addChild(knob);
    cont->addChild(label);
    addChild(button);
    addChild(cont);
    _callback = [](bool, Toggle*) {};
}

void CustomUi::Toggle::update(f32 dt)
{
    auto dSize = getDynamicContentSize();
    button->setContentSize(dSize);
    auto ns = GameUtils::getNodeIgnoreDesignScale();
    setContentSize(dSize / ns);
    HoverEffectGUI::update(dt, dSize);
}

bool CustomUi::Toggle::hover(cocos2d::Vec2 mouseLocationInView, Camera* cam)
{
    if (isEnabled())
    {
        if (!_pCurrentHeldItem) {
            setUiHovered(button->hitTest(mouseLocationInView, cam, _NOTHING));
            hover_cv.setValue(isUiHovered());
        }

        if (hover_cv.isChanged())
            HoverEffectGUI::hover();
    }

    return hover_cv.getValue();
}

void CustomUi::Toggle::focus()
{
}

void CustomUi::Toggle::defocus()
{
}

void CustomUi::Toggle::onEnable()
{
    auto fade = FadeTo::create(0.1f, 255);
    knob->icon->runAction(fade);
}

void CustomUi::Toggle::onDisable()
{
    auto fade = FadeTo::create(0.1f, 100);
    knob->icon->runAction(fade);
}

bool CustomUi::Toggle::press(cocos2d::Vec2 mouseLocationInView, Camera* cam)
{
    if (!isEnabled()) return false;
    if (button->hitTest(mouseLocationInView, cam, _NOTHING))
    {
        if (_pCurrentHeldItem) _pCurrentHeldItem->release({ INFINITY, INFINITY }, cam);
        _pCurrentHeldItem = this;
        auto fade = FadeTo::create(0, 100);
        auto tint = TintTo::create(0, Color3B::GRAY);
        onDisable(); // Used for effects only
        return true;
    }
    hover(mouseLocationInView, cam);
	return false;
}

bool CustomUi::Toggle::release(cocos2d::Vec2 mouseLocationInView, Camera* cam)
{
    onEnable(); // Used for effects only
    if (button->hitTest(mouseLocationInView, cam, _NOTHING)) {
        _callback(isToggled = !isToggled, this);
        knob->icon->setSpriteFrame(isToggled ? "toggle_selected" : "toggle_non");
        SoundGlobals::playUiHoverSound();
        return true;
    }
	return false;
}

Size CustomUi::Toggle::getDynamicContentSize()
{
    return cont->getContentSize();
}

Size CustomUi::Toggle::getFitContentSize()
{
	return getDynamicContentSize();
}
