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
    cont->updateLayoutManagers();
    update(0);
}

void CUI::Toggle::update(f32 dt)
{
    auto dSize = getDynamicContentSize();
    auto ns = GameUtils::getNodeIgnoreDesignScale();
    //dSize = dSize / (_rescalingAllowed ? ns : 1.0 / ns);
    if (setContentSize(dSize / ns)) {
        cont->updateLayoutManagers();
        button->setContentSize(dSize * ns);
        HoverEffectGUI::update(dt, dSize * ns);
    }
}

bool CUI::Toggle::hover(cocos2d::Vec2 mouseLocationInView, Camera* cam)
{
    if (isEnabled())
    {
        if (!_pCurrentHeldItem) {
            setUiHovered(button->hitTest(mouseLocationInView, cam, _NOTHING));
            hover_cv.setValue(isUiHovered());
            _pCurrentHoveredTooltipItem = isUiHovered() ? this : (_pCurrentHoveredTooltipItem == this ? nullptr : _pCurrentHoveredTooltipItem);
            if (label) { if (isUiHovered()) label->field->enableUnderline(); else label->field->disableEffect(ax::LabelEffect::UNDERLINE); }
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
        if (group) {
            group->select(this);
            knob->icon->setSpriteFrame((isToggled = true) ? "toggle_selected" : "toggle_non");
        } else {
            _callback(isToggled = !isToggled, this);
            knob->icon->setSpriteFrame(isToggled ? "toggle_selected" : "toggle_non");
        }
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
    i8 count = 0;
    i8 selectedIndex = -1;
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
