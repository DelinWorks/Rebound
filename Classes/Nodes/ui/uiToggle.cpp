#include "uiToggle.h"

CUI::Toggle* CUI::Toggle::create()
{
    CUI::Toggle* ret = new CUI::Toggle();
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

void CUI::Toggle::init(std::wstring _text, Size _contentsize)
{
    scheduleUpdate();
    cont = CUI::Container::create();
    auto fl = FlowLayout();
    fl.spacing = Math::getOdd(20);
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

void CUI::Toggle::update(f32 dt)
{
    auto dSize = getDynamicContentSize();
    button->setContentSize(dSize);
    auto ns = GameUtils::getNodeIgnoreDesignScale();
    setContentSize(dSize / ns);
    HoverEffectGUI::update(dt, dSize);
}

bool CUI::Toggle::hover(cocos2d::Vec2 mouseLocationInView, Camera* cam)
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

void CUI::Toggle::focus()
{
}

void CUI::Toggle::defocus()
{
}

void CUI::Toggle::onEnable()
{
    auto fade = FadeTo::create(0.1f, 255);
    knob->icon->runAction(fade);
}

void CUI::Toggle::onDisable()
{
    auto fade = FadeTo::create(0.1f, 100);
    knob->icon->runAction(fade);
}

bool CUI::Toggle::press(cocos2d::Vec2 mouseLocationInView, Camera* cam)
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

bool CUI::Toggle::release(cocos2d::Vec2 mouseLocationInView, Camera* cam)
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

Size CUI::Toggle::getDynamicContentSize()
{
    return cont->getContentSize();
}

Size CUI::Toggle::getFitContentSize()
{
	return getDynamicContentSize();
}
