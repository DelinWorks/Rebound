#include "uiTabs.h"

CUI::Tabs::Tabs(V2D _prefferedSize)
{
    scheduleUpdate();
    //addComponent((new UiRescaleComponent(Director::getInstance()->getVisibleSize()))->enableDesignScaleIgnoring());

    elementCont = CUI::Container::create();
    scrollCont = CUI::Container::create();
    clipping = EventPassClippingNode::create(elementCont);

    setBackgroundBlocking();
    setElementBlocking();
    setSelfHover();

    setStatic();
    this->_prefferedSize = _prefferedSize;
    setContentSize(_prefferedSize);
    addChild(clipping);

    elementCont->setLayout(FlowLayout(SORT_HORIZONTAL, STACK_RIGHT, -48, 0, false));
    elementCont->setConstraint(DependencyConstraint(this, LEFT, V2D::ZERO, false, V2D(0, 0)));

    scrollCont->setStatic();
    scrollCont->setContentSize(V2D(18, _prefferedSize.y));
    scrollCont->setLayout(FlowLayout(SORT_HORIZONTAL, STACK_CENTER, 6));
    scrollCont->setConstraint(DependencyConstraint(this, RIGHT));
    scrollCont->setBorderLayoutAnchor(RIGHT);
    scrollCont->setBackgroundBlocking();
    addChild(scrollCont);

    rightB = CUI::Button::create();
    leftB = CUI::Button::create();

    rightB->initIcon("editor_arrow_right", {2, 5});
    //rightB->_callback = [=](Button* target) {
    //    float avg = 0.0f;
    //    int count = 0;
    //    for (auto& _ : elementCont->getChildren()) {
    //        avg += _->getContentSize().x;
    //        count++;
    //    }
    //    avg /= count;
    //    V2D pos = ePos - V2D(avg / 2, 0);
    //    pos.x = Math::clamp(pos.x, elementCont->getContentSize().x / -2 + getContentSize().x / 2 - scrollCont->getContentSize().x, getContentSize().x / -2);
    //    if (ePos != pos) {
    //        leftB->enable();
    //        ePos = pos;
    //        elementCont->stopAllActions();
    //        elementCont->runAction(EaseExponentialOut::create(MoveTo::create(0.5, pos)));
    //    }
    //    else target->disable();
    //};
    scrollCont->addChild(rightB);

    leftB->initIcon("editor_arrow_left", {2, 5});
    //leftB->_callback = [=](Button* target) {
    //    float avg = 0.0f;
    //    int count = 0;
    //    for (auto& _ : elementCont->getChildren()) {
    //        avg += _->getContentSize().x;
    //        count++;
    //    }
    //    avg /= count;
    //    V2D pos = ePos + V2D(avg / 2, 0);
    //    pos.x = Math::clamp(pos.x, elementCont->getContentSize().x / -2 + getContentSize().x / 2 - scrollCont->getContentSize().x, getContentSize().x / -2);
    //    if (ePos != pos) {
    //        rightB->enable();
    //        ePos = pos;
    //        elementCont->stopAllActions();
    //        elementCont->runAction(EaseExponentialOut::create(MoveTo::create(0.5, pos)));
    //    } else target->disable();
    //};
    scrollCont->addChild(leftB);
}

CUI::Tabs* CUI::Tabs::create(V2D _prefferedSize)
{
    Tabs* ref = new Tabs(_prefferedSize);
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

void CUI::Tabs::calculateContentBoundaries()
{
    auto ns = GameUtils::getNodeIgnoreDesignScale();
    scrollCont->setContentSize(V2D(28 * ns.x, _prefferedSize.y));
    setContentSize(V2D(getContentSize().x, 20 * ns.y));
    Container::recalculateChildDimensions();
    auto& c = getContentSize();
    clipping->setClipRegion(Rect(c.x / -2 - 3 * ns.x, c.y * 8 / -2, c.x - 18 * ns.x, c.y * 8));
}

void CUI::Tabs::updateLayoutManagers(bool recursive)
{
    scrollCont->updateLayoutManagers(true);
    elementCont->updateLayoutManagers();
    ePos = elementCont->getPosition();
    Container::updateLayoutManagers();
}

void CUI::Tabs::addElement(std::wstring e, GUI* container)
{
    if (elementCont->getChildrenCount() > 0) {
        auto sep = CUI::Button::create();
        sep->init(L"|", TTFFS);
        sep->disable();
        elementCont->addChild(sep);
    }

    auto b = CUI::Button::create();
    b->init(e, TTFFS, V2D::ZERO, { -6, 3 });
    elementCont->addChild(b);
    tabIndices.push_back({ b, container });

    b->_callback = [=](Button* target) {
        for (int i = 0; i < tabIndices.size(); i++) {
            if (tabIndices[i].button == target) {
                int idx = i;
                target->enableIconHighlight();
                if (tabIndices[i].cont) {
                    tabIndices[i].cont->enableSelf(true);
                    auto c = DCAST(Container, tabIndices[i].cont);
                    if (c) c->_onContainerTabSelected(c);
                }
                tabIndex = idx;
            }
            else {
                tabIndices[i].button->disableIconHighlight();
                if (tabIndices[i].cont)
                    tabIndices[i].cont->disableSelf(true);
            }
        }
    };
}

void CUI::Tabs::update(F32 dt)
{
    if (_pCurrentHeldItem == rightB || _pCurrentHeldItem == leftB) {
        ePos = ePos - V2D(vel * (_pCurrentHeldItem == rightB ? dt : -dt), 0);
        ePos.x = Math::clamp(ePos.x, elementCont->getContentSize().x / -2 + getContentSize().x / 2 - scrollCont->getContentSize().x, getContentSize().x / -2 - 8);
        elementCont->setPosition(ePos);
        vel += 600 * dt;
    }
    else vel = 200;

    // Ui Culling
    if (!elementCont->getPosition().equals(elemContPos)) {
        auto& p = elementCont->getPosition();
        auto ns = GameUtils::getNodeIgnoreDesignScale();
        for (auto& _ : elementCont->getChildren()) {
            auto c1 = _->getNodeToParentTransform() * Vec3(_->getContentSize().x, _->getContentSize().y, 0);
            if (c1 == ax::Vec3::ZERO) break;
            auto pos = _->getPosition() + p;
            auto b1 = Rect(pos.x, 0, c1.x, 0);
            auto c2 = getNodeToParentTransform() * Vec3(getContentSize().x, getContentSize().y, 0);
            auto b2 = Rect(c2.x / -2 + c1.x / 2, 0, c2.x - 30 * ns.x, 0);
            _->setVisible(b1.intersectsRect(b2));
            elemContPos = elementCont->getPosition();
        }
        if (elementCont->getContentSize().x / 2 < _prefferedSize.x)
            scrollCont->disable(true); else scrollCont->enable(true);
    }
}

void CUI::Tabs::mouseScroll(EventMouse* event)
{
    if (elementCont->getContentSize().x / 2 < _prefferedSize.x) {
        V2D cp = elementCont->getPosition();
        elementCont->setPositionX(cp.x + 5);
        elementCont->runAction(EaseElasticOut::create(MoveTo::create(2, cp), 0.1));
        return;
    }
    float avg = 0.0f;
    int count = 0;
    for (auto& _ : elementCont->getChildren()) {
        avg += _->getContentSize().x;
        count++;
    }
    avg /= count;
    ePos = ePos + V2D(avg / 3 * (event->getScrollY() < 0 ? 1 : -1), 0);
    ePos.x = Math::clamp(ePos.x, elementCont->getContentSize().x / -2 + getContentSize().x / 2 - scrollCont->getContentSize().x, getContentSize().x / -2 - 8);
    elementCont->stopAllActions();
    elementCont->runAction(EaseCubicActionOut::create(MoveTo::create(0.4, ePos)));
    elemContPos = INVALID_LOCATION;
    update(0);
}

void CUI::Tabs::setSelection(int idx)
{
    if (tabIndices.size() > 0)
        tabIndices[idx].button->_callback(tabIndices[idx].button);
}

CUI::Tabs::~Tabs()
{
    LOG_RELEASE;
}
