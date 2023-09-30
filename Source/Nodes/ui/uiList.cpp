#include "uiList.h"

CUI::List::List(V2D _prefferedSize, bool rescalingAllowed)
{
    scheduleUpdate();
    _rescalingAllowed = rescalingAllowed;
    if (rescalingAllowed)
        addComponent((new UiRescaleComponent(Director::getInstance()->getVisibleSize()))->enableDesignScaleIgnoring());

    elementCont = CUI::Container::create();
    scrollCont = CUI::Container4Edge::create(V2D::ZERO);
    clipping = EventPassClippingNode::create(elementCont);

    setBackgroundBlocking();
    setElementBlocking();
    setSelfHover();
    disableProcessToggleTree();

    setStatic();
    SELF prefferredListSize = _prefferedSize;
    setContentSize(_prefferedSize);
    SELF addChild(clipping);
    
    elementCont->setLayout(FlowLayout(SORT_VERTICAL, STACK_BOTTOM, 0, 0, false));
    elementCont->setConstraint(DependencyConstraint(this, TOP));
    elementCont->disableProcessToggleTree();

    scrollCont->setStatic();
    scrollCont->setContentSize(V2D(12, getContentSize().y));
    scrollCont->setConstraint(DependencyConstraint(this, RIGHT));
    scrollCont->setBorderLayoutAnchor(RIGHT);

    scrollKnob = Button::create();
    scrollKnob->DenyRescaling();
    scrollKnob->initIcon("slider_knob_hovered", ADVANCEDUI_SLIDER_CAP_INSETS, {-1, UINT16_MAX});
    scrollKnob->icon->setContentSize({ scrollKnob->icon->getContentSize().x, 16 });
    scrollCont->addChild(scrollKnob);

    SELF addChild(scrollCont);

    disableRebuildOnEnter();

    //upB = CUI::Button::create();
    //downB = CUI::Button::create();

    //upB->initIcon("editor_arrow_up", {2, 5});
    //upB->setUiPadding({ 2, 3 });
    ////rightB->_callback = [=](Button* target) {
    ////    float avg = 0.0f;
    ////    int count = 0;
    ////    for (auto& _ : elementCont->getChildren()) {
    ////        avg += _->getContentSize().x;
    ////        count++;
    ////    }
    ////    avg /= count;
    ////    V2D pos = ePos - V2D(avg / 2, 0);
    ////    pos.x = Math::clamp(pos.x, elementCont->getContentSize().x / -2 + getContentSize().x / 2 - scrollCont->getContentSize().x, getContentSize().x / -2);
    ////    if (ePos != pos) {
    ////        leftB->enable();
    ////        ePos = pos;
    ////        elementCont->stopAllActions();
    ////        elementCont->runAction(EaseExponentialOut::create(MoveTo::create(0.5, pos)));
    ////    }
    ////    else target->disable();
    ////};
    //auto cont = Container::create();
    //cont->addChild(upB);
    //scrollCont->setChildTop(cont);

    //downB->initIcon("editor_arrow_down", {2, 5});
    //downB->setUiPadding({ 2, 3 });
    ////leftB->_callback = [=](Button* target) {
    ////    float avg = 0.0f;
    ////    int count = 0;
    ////    for (auto& _ : elementCont->getChildren()) {
    ////        avg += _->getContentSize().x;
    ////        count++;
    ////    }
    ////    avg /= count;
    ////    V2D pos = ePos + V2D(avg / 2, 0);
    ////    pos.x = Math::clamp(pos.x, elementCont->getContentSize().x / -2 + getContentSize().x / 2 - scrollCont->getContentSize().x, getContentSize().x / -2);
    ////    if (ePos != pos) {
    ////        rightB->enable();
    ////        ePos = pos;
    ////        elementCont->stopAllActions();
    ////        elementCont->runAction(EaseExponentialOut::create(MoveTo::create(0.5, pos)));
    ////    } else target->disable();
    ////};
    //cont = Container::create();
    //cont->addChild(downB);
    //scrollCont->setChildBottom(cont);

    ePos = { 0, getContentSize().y / 2 };
}

CUI::List* CUI::List::create(V2D _prefferedSize, bool rescalingAllowed)
{
    List* ref = new List(_prefferedSize, rescalingAllowed);
    if (ref->init())
    {
        ref->autorelease();
    }
    else
    {
        AX_SAFE_DELETE(ref);
    }
    return ref;
}

void CUI::List::calculateContentBoundaries()
{
    Container::recalculateChildDimensions();
    auto& c = getContentSize();
    clipping->setClipRegion({ c.x / -2, c.y / -2, c.x, c.y});
}

void CUI::List::updateLayoutManagers(bool recursive)
{
    Container::updateLayoutManagers(true);
    elementCont->onFontScaleUpdate(1);
    elementCont->updateLayoutManagers(true);
    elementCont->setPosition(ePos);
    scrollCont->updateLayoutManagers(true);
    ePos = elementCont->getPosition();

    auto l = GameUtils::findNodesByTag(this, 9);
    if (l.size() != 0) l[0]->setVisible(!elements.size());
}

CUI::Label* CUI::List::setEmptyText(std::wstring _text)
{
    auto emptyText = CUI::Label::create();
    emptyText->setTag(9);
    emptyText->DenyRescaling();
    emptyText->hAlignment = TextHAlignment::CENTER;
    emptyText->init(_text, TTFFS);
    SELF addChild(emptyText);
    return emptyText;
}

void CUI::List::addElement(Container* container, int extendCoeff)
{
    container->_disregardGraph = true;
    auto y = container->getContentSize().y;
    container->setStatic();
    container->setContentSize(V2D(0, y) + container->getMargin(), false);
    container->setConstraint(ContentSizeConstraint(this, {-14, 0}, false, false, true));
    container->setPositionX(-7);
    elements.push_back(container);
    container->disableRebuildOnEnter();
    elementCont->addChild(container);
    GUI::DisableDynamicsRecursive(container);
    elemContPos = INVALID_LOCATION;
    if (extendCoeff != 0 && _prefferedSize.x < extendCoeff)
        setContentSize(V2D(extendCoeff, _prefferedSize.y), true);
    isListDirty = true;
}

void CUI::List::removeElement(U32 index)
{
    if (index < elements.size())
    {
        auto p = elements[index];
        elementCont->removeChild(p);
        elements.erase(elements.begin() + index);
        isListDirty = true;
    }
}

void CUI::List::moveElement(U32 index, U32 newIndex)
{
    if (index < elements.size() && newIndex <= elements.size() && index != newIndex)
    {
        std::swap(elements[index], elements[newIndex]);
        elementCont->getChildren().swap(index, newIndex);
        isListDirty = true;
    }
}

void CUI::List::update(F32 dt)
{
    //if (_pCurrentHeldItem == upB || _pCurrentHeldItem == downB) {
    //    ePos = ePos - V2D(0, vel * (_pCurrentHeldItem == upB ? dt : -dt));
    //    ePos.y = Math::clamp(ePos.y, getContentSize().y / 2, elementCont->getContentSize().y / 2 - getContentSize().y / 2);
    //    elementCont->setPosition(ePos);
    //    vel += 600 * dt;
    //}
    //else vel = 200;

    if (isListDirty)
    {
        updateLayoutManagers(true);

        if (elementCont->getContentSize().y / 2 < getContentSize().y)
            ePos.y = getContentSize().y / 2;

        ePos.y = Math::clamp(ePos.y, getContentSize().y / 2, elementCont->getContentSize().y / 2 - getContentSize().y / 2);
        elementCont->stopAllActions();
        elementCont->runAction(EaseCubicActionOut::create(MoveTo::create(0.4, ePos)));
        elemContPos = INVALID_LOCATION;

        int i = 0;
        for (auto& _ : elements)
        {
            if (i % 2 != 0)
            {
                _->setBackgroundSpriteDarken(V2D(UINT32_MAX, 0));
                _->recalculateChildDimensions();
                _->setBackgroundVisible(true);
            }
            else _->setBackgroundVisible(false);
            i++;
        }

        isListDirty = false;
    }
        //recalculateChildDimensions();

    if (_pCurrentHeldItem != scrollKnob) {
        float map = Math::map(elementCont->getPositionY(), getContentSize().y / 2, elementCont->getContentSize().y / 2 - getContentSize().y / 2, getContentSize().y / 2 - 16, getContentSize().y / -2 + 16);
        scrollKnob->setPositionY(map);
        dtScroll = UINT16_MAX;
    }
    else {
        auto ns = GameUtils::getNodeIgnoreDesignScale();
        if (dtScroll == UINT16_MAX)
            dtScroll = _savedLocationInView.y / (_rescalingAllowed ? 1 : ns.y);
        float v = _savedLocationInView.y / (_rescalingAllowed ? 1 : ns.y) - dtScroll;
        dtScroll = _savedLocationInView.y / (_rescalingAllowed ? 1 : ns.y);
        v /= getScale();
        scrollKnob->setPositionY(Math::clamp(scrollKnob->getPositionY() + v, getContentSize().y / -2 + 16, getContentSize().y / 2 - 16));
        float map = Math::map(scrollKnob->getPositionY(), getContentSize().y / 2 - 16, getContentSize().y / -2 + 16, getContentSize().y / 2, elementCont->getContentSize().y / 2 - getContentSize().y / 2);
        ePos.y = Math::clamp(map, getContentSize().y / 2, elementCont->getContentSize().y / 2 - getContentSize().y / 2);
        elementCont->setPosition(ePos);
        _pCurrentScrollControlItem = nullptr;
    }

    if (elementCont->getContentSize().y / 2 < getContentSize().y || elements.size() == 0)
        scrollEnableState.setValue(true); else scrollEnableState.setValue(false);

    if (scrollEnableState.isChanged())
        if (scrollEnableState.getValue())
            scrollCont->disable(true);
        else
            scrollCont->enable(true);

    // Ui Culling
    if (!elementCont->getPosition().equals(elemContPos)) {
        elemContPos = elementCont->getPosition();
        auto& p = elementCont->getPosition();
        for (auto& _ : elements) {
            auto c1 = _->getNodeToParentTransform() * Vec3(_->getContentSize().x, _->getContentSize().y, 0);
            if (c1 == ax::Vec3::ZERO) break;
            auto pos = _->getPosition() + p;
            auto b1 = Rect(0, pos.y, 0, c1.y);
            auto c2 = getNodeToParentTransform() * Vec3(getContentSize().x, getContentSize().y, 0) / getScale();
            auto b2 = Rect(0, c2.y / -2 + c1.y / 2, 0, c2.y);
            if (b1.intersectsRect(b2)) _->enableSelf(true); else _->disableSelf(true);
        }
    }
    deltaScroll2 = LERP(deltaScroll2, deltaScroll, 50 * dt);
}

void CUI::List::mouseScroll(EventMouse* event)
{
    if (elementCont->getContentSize().y / 2 < getContentSize().y) {
        V2D cp = elementCont->getPosition();
        elementCont->setPositionY(cp.y + 5 * event->getScrollY());
        elementCont->runAction(EaseBackOut::create(MoveTo::create(.4, cp)));
        return;
    }
    float avg = 0.0f;
    int count = 0;
    for (auto& _ : elementCont->getChildren()) {
        avg += _->getContentSize().y;
        count++;
    }
    avg /= count;
    ePos = ePos + V2D(0, avg * (event->getScrollY() < 0 ? -1 : 1));
    ePos.y = Math::clamp(ePos.y, getContentSize().y / 2, elementCont->getContentSize().y / 2 - getContentSize().y / 2);
    elementCont->stopAllActions();
    elementCont->runAction(EaseCubicActionOut::create(MoveTo::create(0.4, ePos)));
    elemContPos = INVALID_LOCATION;
    update(0);
}

bool CUI::List::hover(V2D mouseLocationInView, cocos2d::Camera* cam)
{
    if (_pCurrentScrollControlItem == this) {
        auto ns = GameUtils::getNodeIgnoreDesignScale();
        ePos.y -= deltaScroll - _savedLocationInView.y / ns.y;
        ePos.y = Math::clamp(ePos.y, getContentSize().y / 2, elementCont->getContentSize().y / 2 - getContentSize().y / 2);
        elementCont->setPositionY(ePos.y);
        deltaScroll = _savedLocationInView.y / ns.y;
        return Container::hover(INVALID_LOCATION, cam);
    }
    return Container::hover(mouseLocationInView, cam);
}

bool CUI::List::press(V2D mouseLocationInView, cocos2d::Camera* cam)
{
    if (_bgButton->hitTest(mouseLocationInView, cam, nullptr) &&
        elementCont->getContentSize().y / 2 > getContentSize().y) {
        _pCurrentScrollControlItem = this;
        auto ns = GameUtils::getNodeIgnoreDesignScale();
        deltaScroll = _savedLocationInView.y / ns.y;
        deltaScroll2 = deltaScroll;
        ePos.y = elementCont->getPositionY();
    }
    return Container::press(mouseLocationInView, cam);
}

bool CUI::List::release(V2D mouseLocationInView, cocos2d::Camera* cam)
{
    if (_pCurrentScrollControlItem == this)
        _pCurrentScrollControlItem = nullptr;
    auto ns = GameUtils::getNodeIgnoreDesignScale();
    ePos.y -= (deltaScroll2 - _savedLocationInView.y / ns.y) * 8;
    bool backOut = ePos.y < getContentSize().y / 2 || ePos.y > elementCont->getContentSize().y / 2 - getContentSize().y / 2;
    ePos.y = Math::clamp(ePos.y, getContentSize().y / 2, elementCont->getContentSize().y / 2 - getContentSize().y / 2);
    elementCont->stopAllActions();
    if (backOut)
        elementCont->runAction(EaseBackOut::create(MoveTo::create(0.5, ePos)));
    else
        elementCont->runAction(EaseCubicActionOut::create(MoveTo::create(1, ePos)));
    return Container::release(mouseLocationInView, cam);
}

CUI::List::~List()
{
    _pCurrentHoveredItem = nullptr;
    LOG_RELEASE;
}
