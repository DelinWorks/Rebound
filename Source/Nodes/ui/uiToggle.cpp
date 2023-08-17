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

void CUI::Toggle::init(std::wstring _text, S2D _contentsize)
{
    if (_rescalingAllowed)
        addComponent((new UiRescaleComponent(Director::getInstance()->getVisibleSize()))->enableDesignScaleIgnoring());
    scheduleUpdate();
    cont = CUI::Container::create();
    cont->DenyRescaling();
    auto fl = FlowLayout();
    fl.spacing = Math::getOdd(20);
    fl.reverseStack = false;
    cont->setLayout(fl);
    knob = Button::create();
    knob->DenyRescaling();
    knob->initIcon(isToggled ? "toggle_selected" : "toggle_non");
    label = Label::create();
    label->DenyRescaling();
    label->init(_text, TTFFS);
    button = createPlaceholderButton();
    cont->addChild(knob);
    cont->addChild(label);
    addChild(button);
    addChild(cont);
    _callback = [](bool, Toggle*) {};
    cont->updateLayoutManagers(true);
    update(0);
}

void CUI::Toggle::update(F32 dt)
{
    auto dSize = (getDynamicContentSize() + _padding) * _UiScale;
    auto ns = FULL_HD_NODE_SCALING;
    //dSize = dSize / (_rescalingAllowed ? ns : 1.0 / ns);
    if (setContentSize(dSize * Vec2(ns.x * 0.75, ns.y) + Vec2(35, 10))) {
        cont->updateLayoutManagers();
        button->setContentSize(dSize * ns * 0.75 + Vec2(35, 10));
        HoverEffectGUI::update(dt, dSize * ns);
    }
}

bool CUI::Toggle::hover(V2D mouseLocationInView, Camera* cam)
{
    if (isEnabled())
    {
        if (!_pCurrentHeldItem) {
            setUiHovered(button->hitTest(mouseLocationInView, cam, _NOTHING));
            hover_cv.setValue(isUiHovered());
            _pCurrentHoveredTooltipItem = isUiHovered() ? this : (_pCurrentHoveredTooltipItem == this ? nullptr : _pCurrentHoveredTooltipItem);
            if (label) { if (isUiHovered()) label->field->enableUnderline(); else label->field->disableEffect(ax::LabelEffect::UNDERLINE); }
        }

        if (hover_cv.isChanged() && hover_cv.getValue())
            SoundGlobals::playUiHoverSound();
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
    auto fade = FadeTo::create(0, 100);
    knob->icon->stopAllActions();
    knob->icon->runAction(fade);
}

bool CUI::Toggle::press(V2D mouseLocationInView, Camera* cam)
{
    if (!isEnabled()) return false;
    if (button->hitTest(mouseLocationInView, cam, _NOTHING))
    {
        if (_pCurrentHeldItem) _pCurrentHeldItem->release({ INFINITY, INFINITY }, cam);
        _pCurrentHeldItem = this;
        SoundGlobals::playUiHoldSound();
        onDisable(); // Used for effects only
        return true;
    }
    hover(mouseLocationInView, cam);
	return false;
}

bool CUI::Toggle::release(V2D mouseLocationInView, Camera* cam)
{
    onEnable(); // Used for effects only
    if (button->hitTest(mouseLocationInView, cam, _NOTHING)) {
        toggle(!isToggled);
        SoundGlobals::playUiClickSound();
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

void CUI::Toggle::toggle(bool state)
{
    isToggled = state;
    if (group) {
        group->select(this);
        knob->icon->setSpriteFrame((isToggled = true) ? "toggle_selected" : "toggle_non");
    }
    else {
        _callback(state, this);
        knob->icon->setSpriteFrame(state ? "toggle_selected" : "toggle_non");
    }
}

CUI::RadioGroup::RadioGroup()
{
    SELF autorelease();
}

CUI::RadioGroup::~RadioGroup()
{
    LOG_RELEASE;
}

void CUI::RadioGroup::addChild(Toggle* t)
{
    t->group = this;
    SELF retain();
    radios.push_back(t);
}

void CUI::RadioGroup::select(Toggle* t)
{
    I8 count = 0;
    I8 selectedIndex = -1;
    for (auto& _ : radios) {
        if (_ != t)
            _->knob->icon->setSpriteFrame(
                (_->isToggled = false) ? "toggle_selected" : "toggle_non");
        else selectedIndex = count;
        count++;
    }
}

CUI::Toggle::~Toggle()
{
    if (group)
        group->release();
    LOG_RELEASE;
}
