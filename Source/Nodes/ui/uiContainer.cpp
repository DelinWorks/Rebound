#include "uiContainer.h"

CUI::Container::Container() : _layout(LAYOUT_NONE), _constraint(CONSTRAINT_NONE),
                                   _borderLayout(BorderLayout::CENTER), _flowLayout(), _depConst()
{
    setAsContainer();
    setDynamic();
}

void CUI::Container::setBorderLayout(BorderLayout border, BorderContext context) {
    if (context == BorderContext::PARENT)
        _closestStaticBorder = true;
    auto rcomp = (UiRescaleComponent*)SELF getComponent("UiRescaleComponent");
    if (rcomp) {
        rcomp->setBorderLayout(_borderLayout = border);
        return;
    }
    addComponent((new UiRescaleComponent(Director::getInstance()->getVisibleSize()))
        ->setBorderLayout(_borderLayout = border));
}

CUI::Container* CUI::Container::create()
{
    Container* ref = new Container();
    if (ref->init())
    {
        ref->_isContainer = true;
        ref->autorelease();
    }
    else
    {
        AX_SAFE_DELETE(ref);
    }
    return ref;
}

void CUI::Container::setLayout(FlowLayout layout)
{
    _flowLayout = layout;
    _layout = Layout::LAYOUT_FLOW;
}

void CUI::Container::setConstraint(DependencyConstraint layout)
{
    _depConst = layout;
    _constraint = Constraint::CONSTRAINT_DEPENDENCY;
}

void CUI::Container::setConstraint(ContentSizeConstraint layout)
{
    _csConst = layout;
    _constraint = Constraint::CONSTRAINT_CONTENTSIZE;
}

CUI::Container* CUI::Container::createDenyScaling()
{
    Container* ref = new Container();
    if (ref->init())
    {
        ref->_isContainer = true;
        ref->DenyRescaling();
        ref->autorelease();
    }
    else
    {
        AX_SAFE_DELETE(ref);
    }
    return ref;
}

bool CUI::Container::hover(cocos2d::Vec2 mouseLocationInView, cocos2d::Camera* cam)
{
    if (!isEnabled()) return false;
    _isHitSwallowed = false;
    bool _isElementBlocked = true;
    if (_isElementBlocking && _bgButton && !_bgButton->hitTest(mouseLocationInView, cam, nullptr))
        _isElementBlocked = false;
    auto& list = getChildren();
    for (int i = list.size() - 1; i > -1; i--)
    {
        auto n = DCAST(GUI, list.at(i));
        if (n) {
            bool cond = !_isElementBlocked || (_isHitSwallowed && _pCurrentHeldItem != n && !n->isForceRawInput());
            if (n->hover(cond ? Vec2(UINT16_MAX, UINT16_MAX) : mouseLocationInView, cam))
                _isHitSwallowed = true;
        }
    }
    if (_bgButton && !_isHitSwallowed) {
        if (_bgButton->hitTest(mouseLocationInView, cam, nullptr))
            _isHitSwallowed = true;
    }
    if (_isSelfHover) {
        if (_isHitSwallowed) _pCurrentHoveredItem = this;
        else if (_pCurrentHoveredItem == this) _pCurrentHoveredItem = nullptr;
    }
    return _isHitSwallowed || _isBlocking;
}

bool CUI::Container::press(cocos2d::Vec2 mouseLocationInView, cocos2d::Camera* cam)
{
    if (!isEnabled()) return false;
    // reset the camera position so that hits are generated correctly.
    //cam->setPosition(Vec2::ZERO);
    bool isClickSwallowed = false;
    bool shouldDismiss = false;
    if (_isElementBlocking && _bgButton) {
        isClickSwallowed = !_bgButton->hitTest(mouseLocationInView, cam, nullptr);
        shouldDismiss = _isDismissible && isClickSwallowed;
    }
    auto& list = getChildren();
    for (int i = list.size() - 1; i > -1; i--)
    {
        auto n = DCAST(GUI, list.at(i));
        if (n)
            if (n->press(isClickSwallowed ? Vec2(UINT16_MAX, UINT16_MAX) : mouseLocationInView, cam))
                isClickSwallowed = true;
    }
    if (_bgButton && !isClickSwallowed) {
        if (_bgButton->hitTest(mouseLocationInView, cam, nullptr))
            isClickSwallowed = true;
    }
    if (_isDismissible && !isClickSwallowed || shouldDismiss) {
        _onContainerDismiss();
        removeFromParent();
    }
    if (_isElementBlocking && isClickSwallowed) isClickSwallowed = false;
    return isClickSwallowed || _isBlocking;
}

bool CUI::Container::release(cocos2d::Vec2 mouseLocationInView, cocos2d::Camera* cam)
{
    if (_pCurrentScrollControlItem) {
        _pCurrentScrollControlItem->release(mouseLocationInView, cam);
        _pCurrentScrollControlItem = nullptr;
    }
    if (_pCurrentHeldItem) {
        auto b = _pCurrentHeldItem->release(mouseLocationInView, cam);
        _pCurrentHeldItem = nullptr;
        return b;
    }
    return false;
}

void CUI::Container::keyPress(EventKeyboard::KeyCode keyCode)
{
    if (_pCurrentHeldItem) return;

    // REMOVE THIS CODE LATER // ONLY FOR DEBUGGING //
    if (getFocusSet().size() == 0)
        if (keyCode == EventKeyboard::KeyCode::KEY_ESCAPE)
            Darkness::destroyInstance();
    // // // // // // // // // // // // // // // // //

    for (auto& _ : getFocusSet()) {
        _->keyPress(keyCode);
        break;
    }
}

Vec2 CUI::Container::getScaledContentSize()
{
    auto ns = GameUtils::getNodeIgnoreDesignScale();
    return getContentSize() / ns;
}

void CUI::Container::keyRelease(EventKeyboard::KeyCode keyCode)
{
    if (_pCurrentHeldItem) return;

    for (auto& _ : getFocusSet()) {
        _->keyRelease(keyCode);
        break;
    }
}

void CUI::Container::setElementBlocking()
{
    _isElementBlocking = true;
}

void CUI::Container::setSelfHover()
{
    _isSelfHover = true;
}

void CUI::Container::mouseScroll(EventMouse* event) {
    if (_pCurrentHoveredItem)
        _pCurrentHoveredItem->mouseScroll(event);
}

void CUI::Container::updateLayoutManagers(bool recursive)
{
    if (recursive) {
        auto& list = getChildren();
        for (auto& _ : list) {
            auto ccast = DCAST(Container, _);
            auto gcast = DCAST(GUI, _);
            if (ccast)
                ccast->updateLayoutManagers(true);
            if (gcast) {
                gcast->update(0);
                gcast->updateInternalObjects();
            }
        }
    }

    switch (_layout) {
    case Layout::LAYOUT_FLOW: {
        _flowLayout.build(this);
        break;
    }
    default:
        break;
    }

    switch (_constraint) {
    case Constraint::CONSTRAINT_DEPENDENCY: {
        _depConst.build(this);
        break;
    }
    case Constraint::CONSTRAINT_CONTENTSIZE: {
        _csConst.build(this);
        break;
    }
    default:
        break;
    }

    calculateContentBoundaries();

    if (recursive) {
        auto& list = getChildren();
        for (auto& _ : list) {
            auto ccast = DCAST(Container, _);
            if (ccast)
                ccast->updateLayoutManagers(true);
        }
    }
}

void CUI::Container::onEnter() {
    if (_rebuildOnEnter) {
        GUI::onEnter();
        onFontScaleUpdate(_UiScale / _UiScaleMul);
        updateLayoutManagers(true);
    } else Node::onEnter();
}

void CUI::Container::onEnable()
{}

void CUI::Container::onDisable()
{}

void CUI::Container::setBorderLayoutAnchor(ax::Vec2 offset)
{
    switch (_borderLayout) {
    case BorderLayout::TOP:
        setAnchorPoint(Vec2(0, 0.5) * offset);
        break;
    case BorderLayout::TOP_RIGHT:
        setAnchorPoint(Vec2(0.5, 0.5) * offset);
        break;
    case BorderLayout::RIGHT:
        setAnchorPoint(Vec2(0.5, 0) * offset);
        break;
    case BorderLayout::BOTTOM_RIGHT:
        setAnchorPoint(Vec2(0.5, -0.5) * offset);
        break;
    case BorderLayout::BOTTOM:
        setAnchorPoint(Vec2(0, -0.5) * offset);
        break;
    case BorderLayout::BOTTOM_LEFT:
        setAnchorPoint(Vec2(-0.5, -0.5) * offset);
        break;
    case BorderLayout::LEFT:
        setAnchorPoint(Vec2(-0.5, 0) * offset);
        break;
    case BorderLayout::TOP_LEFT:
        setAnchorPoint(Vec2(-0.5, 0.5) * offset);
        break;
    default:
        setAnchorPoint(Vec2(0, 0) * offset);
    }
}

void CUI::Container::setBorderLayoutAnchor(BorderLayout border, ax::Vec2 offset)
{
    switch (border) {
    case BorderLayout::TOP:
        setAnchorPoint(Vec2(0, 0.5) * offset);
        break;
    case BorderLayout::TOP_RIGHT:
        setAnchorPoint(Vec2(0.5, 0.5) * offset);
        break;
    case BorderLayout::RIGHT:
        setAnchorPoint(Vec2(0.5, 0) * offset);
        break;
    case BorderLayout::BOTTOM_RIGHT:
        setAnchorPoint(Vec2(0.5, -0.5) * offset);
        break;
    case BorderLayout::BOTTOM:
        setAnchorPoint(Vec2(0, -0.5) * offset);
        break;
    case BorderLayout::BOTTOM_LEFT:
        setAnchorPoint(Vec2(-0.5, -0.5) * offset);
        break;
    case BorderLayout::LEFT:
        setAnchorPoint(Vec2(-0.5, 0) * offset);
        break;
    case BorderLayout::TOP_LEFT:
        setAnchorPoint(Vec2(-0.5, 0.5) * offset);
        break;
    default:
        setAnchorPoint(Vec2(0, 0) * offset);
    }
}

void CUI::Container::setBackgroundSprite(ax::Vec2 padding, BgSpriteType type)
{
    _backgroundPadding = padding;
    _background = ax::ui::Scale9Sprite::create();
    switch (type) {
    case BgSpriteType::BG_NORMAL:
        _background->setSpriteFrame(ADVANCEDUI_TEXTURE);
        break;
    case BgSpriteType::BG_INVERTED:
        _background->setSpriteFrame(ADVANCEDUI_TEXTURE_INV);
        break;
    case BgSpriteType::BG_GRAY:
        _background->setSpriteFrame(ADVANCEDUI_TEXTURE_GRAY);
        break;
    }
    _background->setCapInsets(ADVANCEDUI_P1_CAP_INSETS);
    _background->setTag(YOURE_NOT_WELCOME_HERE);
    //if (type != BgSpriteType::BG_GRAY)
    _background->setProgramState(_backgroundShader);
    //_background->addComponent((new UiRescaleComponent(Director::getInstance()->getVisibleSize()))
    //    ->enableDesignScaleIgnoring(Vec2(2, 2)));
    Node::addChild(_background, -1);
}

void CUI::Container::setBackgroundSpriteCramped(ax::Vec2 padding, ax::Vec2 scale)
{
    _backgroundPadding = padding;
    _background = ax::ui::Scale9Sprite::createWithSpriteFrameName(ADVANCEDUI_TEXTURE_CRAMPED, ADVANCEDUI_P1_CAP_INSETS);
    _background->setTag(YOURE_NOT_WELCOME_HERE);
    _background->setProgramState(_backgroundShader);
    _background->setScale(scale.x, scale.y);
    //_background->addComponent((new UiRescaleComponent(Director::getInstance()->getVisibleSize()))
    //    ->enableDesignScaleIgnoring(scale * _PxArtMultiplier));
    Node::addChild(_background, -1);
}

void CUI::Container::setBackgroundSpriteCramped2(ax::Vec2 padding, ax::Vec2 scale)
{
    _backgroundPadding = padding;
    _background = ax::ui::Scale9Sprite::createWithSpriteFrameName(ADVANCEDUI_TEXTURE_CRAMPED2, ADVANCEDUI_P1_CAP_INSETS);
    _background->setTag(YOURE_NOT_WELCOME_HERE);
    _background->setProgramState(_backgroundShader);
    _background->setScale(scale.x, scale.y);
    //_background->addComponent((new UiRescaleComponent(Director::getInstance()->getVisibleSize()))
    //    ->enableDesignScaleIgnoring(scale * _PxArtMultiplier));
    Node::addChild(_background, -1);
}

void CUI::Container::setBackgroundSpriteCramped3(ax::Vec2 padding, ax::Vec2 scale)
{
    _backgroundPadding = padding;
    _background = ax::ui::Scale9Sprite::createWithSpriteFrameName(ADVANCEDUI_TEXTURE_CRAMPED3, ADVANCEDUI_P1_CAP_INSETS);
    _background->setTag(YOURE_NOT_WELCOME_HERE);
    _background->setProgramState(_backgroundShader);
    _background->setScale(scale.x, scale.y);
    //_background->addComponent((new UiRescaleComponent(Director::getInstance()->getVisibleSize()))
    //    ->enableDesignScaleIgnoring(scale * _PxArtMultiplier));
    Node::addChild(_background, -1);
}

void CUI::Container::setBackgroundSpriteDarken(ax::Vec2 padding) {
    _backgroundPadding = padding;
    _background = ax::ui::Scale9Sprite::create("pixel.png");
    _background->setTag(YOURE_NOT_WELCOME_HERE);
    //_background->setProgramState(_backgroundShader);
    _background->setColor({ 128, 128, 128 });
    _background->setOpacity(20);
    //_background->addComponent((new UiRescaleComponent(Director::getInstance()->getVisibleSize()))
    //    ->enableDesignScaleIgnoring(Vec2(2, 2)));
    Node::addChild(_background, -1);
}

void CUI::Container::setBackgroundDim()
{
    _bgDim = ax::LayerColor::create(Color4B(0, 0, 0, 100));
    auto visibleSize = Director::getInstance()->getVisibleSize();
    SET_POSITION_MINUS_HALF_SCREEN(_bgDim);
    _bgDim->setTag(YOURE_NOT_WELCOME_HERE);
    _bgDim->setScale(10);
    Node::addChild(_bgDim, -2);
}

void CUI::Container::setBlocking()
{
    _isBlocking = true;
}

void CUI::Container::setDismissible()
{
    _isDismissible = true;
}

void CUI::Container::setBackgroundBlocking()
{
    _bgButton = createPlaceholderButton();
    _bgButton->setContentSize(getContentSize() + BUTTON_HITBOX_CORNER_TOLERANCE);
    _bgButton->setTag(GUI_ELEMENT_EXCLUDE);
    addChild(_bgButton);
}

void CUI::Container::notifyLayout()
{
    updateLayoutManagers();
    GUI::notifyLayout();
}

void CUI::Container::addSpecialChild(CUI::GUI* gui)
{
    _allButtons.push_back(gui);
}

CUI::Container* CUI::Container::addChildAsContainer(CUI::GUI* gui) {
    auto cont = CUI::Container::create();
    cont->setLayout(FlowLayout());
    cont->addChild(gui);
    cont->updateLayoutManagers();
    Node::addChild(cont);
    return cont;
}

void CUI::Container::recalculateChildDimensions()
{
    if (_background)
        _background->setContentSize(getContentSize() + _backgroundPadding);

    if (_bgButton)
        _bgButton->setContentSize(getContentSize() + _backgroundPadding + BUTTON_HITBOX_CORNER_TOLERANCE);
}

void CUI::Container::calculateContentBoundaries()
{
    auto& list = getChildren();

    float highestX = -FLT_MAX;
    float highestY = -FLT_MAX;

    auto ns = _rescalingAllowed ? GameUtils::getNodeIgnoreDesignScale() : Vec2::ONE;

    Vec2 highestSize = Vec2::ZERO;
    Vec2 dominantSize = Vec2::ZERO;

    for (auto& n : list) {
        auto _ = DCAST(GUI, n);
        if (!_ || _->getTag() <= YOURE_NOT_WELCOME_HERE) continue;
        if (!isContainerDynamic()) continue;
        auto size = _->getScaledContentSize();
        float eq = abs(_->getPositionX());
        if (eq > highestX) {
            highestX = eq;
            highestSize.x = size.x * ns.x;
        }

        eq = abs(_->getPositionY());
        if (eq > highestY) {
            highestY = eq;
            highestSize.y = size.y * ns.y;
        }

        if (size.x > dominantSize.x) {
            highestSize.x = size.x * ns.x;
            dominantSize.x = size.x;
        }

        if (size.y > dominantSize.y) {
            highestSize.y = size.y * ns.y;
            dominantSize.y = size.y;
        }
    }

    auto scaledMargin = ax::Vec2(
        _margin.x * 2 * ns.x,
        _margin.y * 2 * ns.y
    );

    if (isContainerDynamic())
        setContentSize((Vec2(abs(highestX * 2 + highestSize.x + scaledMargin.x),
                             abs(highestY * 2 + highestSize.y + scaledMargin.y))), false);

    Container::recalculateChildDimensions();
}

void CUI::FlowLayout::build(CUI::Container* container)
{
    auto list = container->getChildren();
    auto ns = container->_rescalingAllowed ? GameUtils::getNodeIgnoreDesignScale() : Vec2::ONE;
    auto _spacing = Vec2(spacing, spacing);
    f32 sumSize = 0;
    u16 listSize = 0;
    for (auto& n : list) {
        auto _ = DCAST(GUI, n);
        if (!_ || _->getTag() <= YOURE_NOT_WELCOME_HERE) continue;
        auto cSize = constSize ? constSizeV : _->getScaledContentSize();
        if (cSize.x == 0 || cSize.y == 0)
            continue;
        cSize.x += _spacing.x * 2;
        cSize.y += _spacing.y * 2;
        sumSize += sort == SORT_HORIZONTAL ? cSize.x : cSize.y;
        listSize++;
    }
    float marginF = direction == STACK_RIGHT || direction == STACK_TOP ? margin : -margin;
    marginF *= ns.x;
    if (reverseStack)
        std::reverse(list.begin(), list.end());
    f32 cumSize = 0;
    if (direction == STACK_CENTER)
        cumSize = (sumSize - (_spacing.x * 1.5 * listSize) - _spacing.x / 2) / -2;
    for (auto& n : list) {
        auto _ = DCAST(GUI, n);
        if (!_ || _->getTag() <= YOURE_NOT_WELCOME_HERE) continue;
        auto cSize = constSize ? constSizeV : _->getScaledContentSize();
        if (cSize.x == 0 || cSize.y == 0)
            continue;
        if (_) {
            if (sort == SORT_HORIZONTAL) {
                cumSize += cSize.x / (direction == STACK_LEFT ? -2 : 2);
                cSize.x += _spacing.x;
                auto r = cumSize * ns.x + marginF;
                _->setPositionX(!_->_iconArtMulEnabled ? Math::getEven(r) : r);
                cumSize += cSize.x / (direction == STACK_LEFT ? -2 : 2);
            }
            else if (sort == SORT_VERTICAL) {
                cumSize += cSize.y / (direction == STACK_BOTTOM ? -2 : 2);
                cSize.y += _spacing.y;
                auto r = cumSize * ns.y + marginF;
                _->setPositionY(!_->_iconArtMulEnabled ? Math::getEven(r) : r);
                cumSize += cSize.y / (direction == STACK_BOTTOM ? -2 : 2);
            }
        }
    }
}

void CUI::DependencyConstraint::build(CUI::GUI* element)
{
    // TODO: FIX STACK OVERFLOW
    //auto container = DCAST(Container, parent);
    //if (container) container->updateLayoutManagers();

    Vec2 anchor = ax::Vec2::ZERO;

    switch (position) {
    case BorderLayout::TOP:
        anchor = (Vec2(0, 0.5) + offset);
        break;
    case BorderLayout::TOP_RIGHT:
        anchor = (Vec2(0.5, 0.5) + offset);
        break;
    case BorderLayout::RIGHT:
        anchor = (Vec2(0.5, 0) + offset);
        break;
    case BorderLayout::BOTTOM_RIGHT:
        anchor = (Vec2(0.5, -0.5) + offset);
        break;
    case BorderLayout::BOTTOM:
        anchor = (Vec2(0, -0.5) + offset);
        break;
    case BorderLayout::BOTTOM_LEFT:
        anchor = (Vec2(-0.5, -0.5) + offset);
        break;
    case BorderLayout::LEFT:
        anchor = (Vec2(-0.5, 0) + offset);
        break;
    case BorderLayout::TOP_LEFT:
        anchor = (Vec2(-0.5, 0.5) + offset);
        break;
    default:
        anchor = (Vec2(0, 0) + offset);
    }

    //if (parent) anchor *= parent->getScale();

    if (worldPos)
        element->setPosition((parent->getWorldPosition() + worldPosOffset) - parent->getContentSize() * anchor);
    else
        element->setPosition((parent->getContentSize() * anchor) + worldPosOffset);
}

void CUI::ContentSizeConstraint::build(CUI::GUI* element) {
    if (element->isContainerDynamic()) return;
    Vec2 s = parent->getContentSize() + offset;
    auto ns = ax::Vec2::ONE;
    if (scale && element->_rescalingAllowed)
        ns = GameUtils::getNodeIgnoreDesignScale();
    s.x = lockX ? element->getPrefferedContentSize().x : s.x * ns.x;
    s.y = lockY ? element->getPrefferedContentSize().y : s.y * ns.y;
    element->setContentSize(s);
}

CUI::Separator* CUI::Separator::create(Vec2 size)
{
    CUI::Separator* ref = new CUI::Separator();
    if (ref->init())
    {
        ref->setContentSize(size);
        ref->autorelease();
    }
    else
    {
        AX_SAFE_DELETE(ref);
    }
    return ref;
}

CUI::EventPassClippingNode* CUI::EventPassClippingNode::create(Container* _child)
{
    CUI::EventPassClippingNode* ref = new CUI::EventPassClippingNode();
    if (ref->init())
    {
        ref->autorelease();
        ref->setClipRegion(ax::Rect::ZERO);
        ref->child = _child;
        ref->clip->addChild(_child);
    }
    else
    {
        AX_SAFE_DELETE(ref);
    }
    return ref;
}

void CUI::EventPassClippingNode::setClipRegion(ax::Rect r)
{
    if (!clip) {
        clip = ClippingRectangleNode::create(r);
        addChild(clip);
    }
    else clip->setClippingRegion(r);
}

bool CUI::EventPassClippingNode::hover(Vec2 mouseLocationInView, Camera* cam)
{
    if (mouseLocationInView == INVALID_LOCATION) return false;
    return child->hover(mouseLocationInView, cam);
}

bool CUI::EventPassClippingNode::press(Vec2 mouseLocationInView, Camera* cam)
{
    return child->press(mouseLocationInView, cam);
}

bool CUI::EventPassClippingNode::release(Vec2 mouseLocationInView, Camera* cam)
{
    return child->release(mouseLocationInView, cam);
}

void CUI::EventPassClippingNode::keyPress(EventKeyboard::KeyCode keyCode)
{
    return child->keyPress(keyCode);
}

void CUI::EventPassClippingNode::keyRelease(EventKeyboard::KeyCode keyCode)
{
    return child->keyRelease(keyCode);
}

void CUI::EventPassClippingNode::mouseScroll(EventMouse* event)
{
    return child->mouseScroll(event);
}

CUI::Container::~Container()
{
    LOG_RELEASE;
}
