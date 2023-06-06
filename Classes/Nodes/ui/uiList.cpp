#include "uiList.h"

CUI::List::List(Vec2 _prefferedSize)
{
    scheduleUpdate();

    elementCont = CUI::Container::create();
    scrollCont = CUI::Container4Edge::create(ax::Vec2::ZERO);
    clipping = EventPassClippingNode::create(elementCont);

    setBackgroundBlocking();
    setElementBlocking();
    setSelfHover();

    setStatic();
    SELF _prefferedSize = _prefferedSize;
    setContentSize(_prefferedSize);
    SELF addChild(clipping);
    
    elementCont->setLayout(FlowLayout(SORT_VERTICAL, STACK_BOTTOM, 0, 0, false));
    elementCont->setConstraint(DependencyConstraint(this, TOP));

    scrollCont->setStatic();
    scrollCont->setContentSize(Vec2(12, _prefferedSize.y));
    scrollCont->setConstraint(DependencyConstraint(this, RIGHT));
    scrollCont->setBorderLayoutAnchor(RIGHT);

    scrollKnob = Button::create();
    scrollKnob->initIcon("slider_knob", ADVANCEDUI_SLIDER_CAP_INSETS, {3, UINT16_MAX});
    scrollKnob->icon->setContentSize({ scrollKnob->icon->getContentSize().x, 16 });
    scrollCont->addChild(scrollKnob);

    SELF addChild(scrollCont);

    upB = CUI::Button::create();
    downB = CUI::Button::create();

    upB->initIcon("editor_arrow_up", {2, 5});
    upB->setUiPadding({ 2, 3 });
    //rightB->_callback = [=](Button* target) {
    //    float avg = 0.0f;
    //    int count = 0;
    //    for (auto& _ : elementCont->getChildren()) {
    //        avg += _->getContentSize().x;
    //        count++;
    //    }
    //    avg /= count;
    //    Vec2 pos = ePos - Vec2(avg / 2, 0);
    //    pos.x = Math::clamp(pos.x, elementCont->getContentSize().x / -2 + getContentSize().x / 2 - scrollCont->getContentSize().x, getContentSize().x / -2);
    //    if (ePos != pos) {
    //        leftB->enable();
    //        ePos = pos;
    //        elementCont->stopAllActions();
    //        elementCont->runAction(EaseExponentialOut::create(MoveTo::create(0.5, pos)));
    //    }
    //    else target->disable();
    //};
    auto cont = Container::create();
    cont->addChild(upB);
    scrollCont->setChildTop(cont);

    downB->initIcon("editor_arrow_down", {2, 5});
    downB->setUiPadding({ 2, 3 });
    //leftB->_callback = [=](Button* target) {
    //    float avg = 0.0f;
    //    int count = 0;
    //    for (auto& _ : elementCont->getChildren()) {
    //        avg += _->getContentSize().x;
    //        count++;
    //    }
    //    avg /= count;
    //    Vec2 pos = ePos + Vec2(avg / 2, 0);
    //    pos.x = Math::clamp(pos.x, elementCont->getContentSize().x / -2 + getContentSize().x / 2 - scrollCont->getContentSize().x, getContentSize().x / -2);
    //    if (ePos != pos) {
    //        rightB->enable();
    //        ePos = pos;
    //        elementCont->stopAllActions();
    //        elementCont->runAction(EaseExponentialOut::create(MoveTo::create(0.5, pos)));
    //    } else target->disable();
    //};
    cont = Container::create();
    cont->addChild(downB);
    scrollCont->setChildBottom(cont);

    ePos = INVALID_LOCATION;
}

CUI::List* CUI::List::create(Vec2 _prefferedSize)
{
    List* ref = new List(_prefferedSize);
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
    scrollCont->setContentSize(Vec2(12, _prefferedSize.y), false);
    elementCont->updateLayoutManagers();
    ePos = elementCont->getPosition();
    Container::updateLayoutManagers();
}

void CUI::List::addElement(Container* container)
{
    container->calculateContentBoundaries();
    auto y = container->getContentSize().y;
    container->setStatic();
    container->setContentSize(Vec2(0, y) + container->getMargin());
    container->setConstraint(ContentSizeConstraint(this, {-12, 0}, false, false, true));
    container->setPositionX(-6);
    if (elements.size() % 2 == 0)
        container->setBackgroundSpriteDarken();
    elements.push_back(container);
    elementCont->addChild(container);
}

void CUI::List::update(f32 dt)
{
    if (_pCurrentHeldItem == upB || _pCurrentHeldItem == downB) {
        ePos = ePos - Vec2(0, vel * (_pCurrentHeldItem == upB ? dt : -dt));
        ePos.y = Math::clamp(ePos.y, getContentSize().y / 2, elementCont->getContentSize().y / 2 - getContentSize().y / 2);
        elementCont->setPosition(ePos);
        vel += 600 * dt;
    }
    else vel = 200;

    if (_pCurrentHeldItem != scrollKnob) {
        float map = Math::map(elementCont->getPositionY(), getContentSize().y / 2, elementCont->getContentSize().y / 2 - getContentSize().y / 2, getContentSize().y / 2 - 16, getContentSize().y / -2 + 16);
        scrollKnob->setPositionY(map);
        dtScroll = UINT16_MAX;
    }
    else {
        if (dtScroll == UINT16_MAX)
            dtScroll = _savedLocationInView.y;
        float v = _savedLocationInView.y - dtScroll;
        dtScroll = _savedLocationInView.y;
        scrollKnob->setPositionY(Math::clamp(scrollKnob->getPositionY() + v, getContentSize().y / -2 + 16, getContentSize().y / 2 - 16));
        float map = Math::map(scrollKnob->getPositionY(), getContentSize().y / 2 - 16, getContentSize().y / -2 + 16, getContentSize().y / 2, elementCont->getContentSize().y / 2 - getContentSize().y / 2);
        ePos.y = Math::clamp(map, getContentSize().y / 2, elementCont->getContentSize().y / 2 - getContentSize().y / 2);
        elementCont->setPosition(ePos);
    }

    // Ui Culling
    if (!elementCont->getPosition().equals(elemContPos)) {
        auto& p = elementCont->getPosition();
        for (auto& _ : elementCont->getChildren()) {
            auto c1 = _->getNodeToParentTransform() * Vec3(_->getContentSize().x, _->getContentSize().y, 0);
            if (c1 == ax::Vec3::ZERO) break;
            auto pos = _->getPosition() + p;
            auto b1 = Rect(0, pos.y, 0, c1.y);
            auto c2 = getNodeToParentTransform() * Vec3(getContentSize().x, getContentSize().y, 0);
            auto b2 = Rect(0, c2.y / -2 + c1.y / 2, 0, c2.y);
            _->setVisible(b1.intersectsRect(b2));
            elemContPos = elementCont->getPosition();
        }
        if (elementCont->getContentSize().y / 2 < _prefferedSize.y)
            scrollCont->disable(true); else scrollCont->enable(true);
    }
}

void CUI::List::mouseScroll(EventMouse* event)
{
    if (elementCont->getContentSize().y / 2 < _prefferedSize.y) {
        Vec2 cp = elementCont->getPosition();
        elementCont->setPositionY(cp.y + 5);
        elementCont->runAction(EaseBackOut::create(MoveTo::create(1, cp)));
        return;
    }
    float avg = 0.0f;
    int count = 0;
    for (auto& _ : elementCont->getChildren()) {
        avg += _->getContentSize().y;
        count++;
    }
    avg /= count;
    ePos = ePos + Vec2(0, avg * (event->getScrollY() < 0 ? -1 : 1));
    ePos.y = Math::clamp(ePos.y, getContentSize().y / 2, elementCont->getContentSize().y / 2 - getContentSize().y / 2);
    elementCont->stopAllActions();
    elementCont->runAction(EaseCubicActionOut::create(MoveTo::create(0.4, ePos)));
    elemContPos = INVALID_LOCATION;
    update(0);
}

CUI::List::~List()
{
    LOG_RELEASE;
}
