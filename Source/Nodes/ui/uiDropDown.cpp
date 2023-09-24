#include "uiDropDown.h"

CUI::DropDown* CUI::DropDown::create()
{
    CUI::DropDown* ret = new CUI::DropDown();
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

void CUI::DropDown::init(std::vector<std::wstring>& _items, Size _contentsize)
{
    if (_rescalingAllowed)
        addComponent((new UiRescaleComponent(Director::getInstance()->getVisibleSize()))->enableDesignScaleIgnoring());
    scheduleUpdate();
    cont = CUI::Container::create();
    cont->DenyRescaling();
    cont->setCascadeOpacityEnabled(true);
    auto fl = FlowLayout();
    fl.spacing = Math::getOdd(20);
    fl.reverseStack = true;
    cont->setLayout(fl);
    knob = Button::create();
    knob->DenyRescaling();
    knob->initIcon("editor_arrow_down");
    knob->update(0);
    knob->setCascadeOpacityEnabled(true);
    label = Label::create();
    label->DenyRescaling();
    label->init(_items[0], TTFFS);
    label->onFontScaleUpdate(1);
    label->update(0);
    button = createPlaceholderButton();
    cont->addChild(knob);
    cont->addChild(label);
    addChild(button);
    addChild(cont);
    SELF _items = std::move(_items);
    _callback = [](DropDown*) {};
    cont->updateLayoutManagers();
    update(0);
}

void CUI::DropDown::update(F32 dt)
{
    auto dSize = (getDynamicContentSize() + _padding) * _UiScale;
    auto ns = FULL_HD_NODE_SCALING;
    //dSize = dSize / (_rescalingAllowed ? ns : 1.0 / ns);
    if (setContentSize(dSize * V2D(ns.x * 0.75, ns.y) + V2D(24, 10))) {
        cont->updateLayoutManagers();
        button->setContentSize(dSize * ns * 0.75 + V2D(24, 10));
        HoverEffectGUI::update(dt, dSize * ns);
    }
}

bool CUI::DropDown::hover(V2D mouseLocationInView, Camera* cam)
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

void CUI::DropDown::focus()
{
}

void CUI::DropDown::defocus()
{
}

void CUI::DropDown::onEnable()
{
    auto fade = FadeTo::create(0.1f, 255);
    cont->runAction(fade);
}

void CUI::DropDown::onDisable()
{
    auto fade = FadeTo::create(0, 100);
    cont->stopAllActions();
    cont->runAction(fade);
}

bool CUI::DropDown::press(V2D mouseLocationInView, Camera* cam)
{
    if (!isEnabled()) return false;
    if (button->hitTest(mouseLocationInView, cam, _NOTHING))
    {
        if (_pCurrentHeldItem) _pCurrentHeldItem->release({ INFINITY, INFINITY }, cam);
        _pCurrentHeldItem = this;
        onDisable(); // Used for effects only
        SoundGlobals::playUiHoldSound();
        return true;
    }
    hover(mouseLocationInView, cam);
    return false;
}

bool CUI::DropDown::release(V2D mouseLocationInView, Camera* cam)
{
    onEnable(); // Used for effects only
    if (button->hitTest(mouseLocationInView, cam, _NOTHING)) {
        knob->icon->setSpriteFrame("editor_arrow_right");
        _callback(this);
        SoundGlobals::playUiClickSound();
        return true;
    }
    return false;
}

Size CUI::DropDown::getDynamicContentSize()
{
    return cont->getContentSize();
}

Size CUI::DropDown::getFitContentSize()
{
    return getDynamicContentSize();
}

void CUI::DropDown::showMenu(GUI* parent, BorderLayout b1, BorderLayout b2, V2D offset)
{
    parent = parent ? parent : knob;

    float maxVerticalSize = 250;

    auto menu = List::create({ 0, maxVerticalSize });
    menu->_onContainerDismiss = [=]() { knob->icon->setSpriteFrame("editor_arrow_down"); };
    menu->setBorderLayoutAnchor(b1);
    menu->setBackgroundSprite({ 15, 8 });
    menu->setBackgroundBlocking();
    menu->setBlocking();
    menu->setDismissible();
    auto vis = Director::getInstance()->getVisibleSize();
    menu->setConstraint(DependencyConstraint(parent, b2, offset, true, vis / -2));

    auto createItemW = [=](const std::wstring& text, int index) -> Container* {
        auto main = CUI::Container::create();
        //main->DenyRescaling();
        auto elem = CUI::Button::create();
        elem->DenyRescaling();
        V2D hpadding = V2D(UINT16_MAX, 12);
        elem->init(text, TTFFS, V2D::ZERO, hpadding);
        auto cont = Container::create();
        cont->addChild(elem);
        auto left = cont;
        left->DenyRescaling();
        left->setConstraint(CUI::DependencyConstraint(main, LEFT));
        left->setBorderLayoutAnchor(LEFT);
        main->setMargin({ 10, 5 });
        main->addChild(left);
        main->updateLayoutManagers(true);

        elem->_callback = [=](Button* target) {
            this->setSelection(index);
            menu->_onContainerDismiss();
            menu->removeFromParent();
        };

        return main;
    };

    int totalY = 0;
    int idx = 0;
    for (auto& _ : _items) {
        auto c = createItemW(_, idx++);
        auto ns = GameUtils::getNodeIgnoreDesignScale();
        menu->addElement(c, c->getContentSize().x);
        totalY += c->getContentSize().y + 1;
    }
    
    menu->prefferredListSize = V2D(menu->getContentSize().x - (totalY < maxVerticalSize ? 20 : 10),
        MIN(maxVerticalSize, totalY));
    menu->ePos.y = menu->prefferredListSize.y / 2;
    menu->setContentSize(menu->prefferredListSize);
    menu->updateLayoutManagers();
    callbackAccess["main"]->addChild(menu);

    float oldScale = menu->getScale();
    menu->setScale(0);
    menu->runAction(EaseBackOut::create(ScaleTo::create(0.16, oldScale)));
}

void CUI::DropDown::setSelection(int idx)
{
    selectedIndex = idx;
    label->setString(_items[idx]);
    cont->updateLayoutManagers(true);
}

CUI::DropDown::~DropDown()
{
    LOG_RELEASE;
}
