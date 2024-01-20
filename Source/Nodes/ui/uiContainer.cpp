#include "uiContainer.h"

CUI::Container::Container() : _layout(LAYOUT_NONE), _constraint(CONSTRAINT_NONE),
                                   _borderLayout(BorderLayout::CENTER), _flowLayout(), _depConst()
{
    setAsContainer();
    setDynamic();
}

void CUI::Container::setBorderLayout(BorderLayout border, BorderContext context, bool designScaleIgnoring) {
    if (context == BorderContext::PARENT)
        _closestStaticBorder = true;
    auto rcomp = (UiRescaleComponent*)SELF getComponent("UiRescaleComponent");
    if (rcomp) {
        rcomp->setBorderLayout(_borderLayout = border);
        return;
    }
    rcomp = new UiRescaleComponent(Director::getInstance()->getVisibleSize());
    rcomp->setBorderLayout(_borderLayout = border);
    if (designScaleIgnoring)
    {
        rcomp->enableDesignScaleIgnoring();
        _ignoreDesignScale = true;
    }
    addComponent(rcomp);
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

bool CUI::Container::hover(V2D mouseLocationInView, Camera* cam)
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
            if (n->hover(cond ? V2D(UINT16_MAX, UINT16_MAX) : mouseLocationInView, cam))
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

bool CUI::Container::press(V2D mouseLocationInView, Camera* cam)
{
    if (!isEnabled()) return false;
    // reset the camera position so that hits are generated correctly.
    //cam->setPosition(V2D::ZERO);
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
            if (n->press(isClickSwallowed ? V2D(UINT16_MAX, UINT16_MAX) : mouseLocationInView, cam))
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

bool CUI::Container::release(V2D mouseLocationInView, Camera* cam)
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
            Rebound::destroyInstance();
    // // // // // // // // // // // // // // // // //

    for (auto& _ : getFocusSet()) {
        _->keyPress(keyCode);
        break;
    }
}

V2D CUI::Container::getScaledContentSize()
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

    _onContainerLayoutUpdate(this);
}

void CUI::Container::onEnter() {
    if (_rebuildOnEnter) {
        GUI::onEnter();
        onFontScaleUpdate(_UiScale / _UiScaleMul);
        updateLayoutManagers(true);
    }
    else Node::onEnter();
}

void CUI::Container::onEnable()
{}

void CUI::Container::onDisable()
{}

void CUI::Container::setBorderLayoutAnchor(V2D offset)
{
    switch (_borderLayout) {
    case BorderLayout::TOP:
        setAnchorPoint(V2D(0, 0.5) * offset);
        break;
    case BorderLayout::TOP_RIGHT:
        setAnchorPoint(V2D(0.5, 0.5) * offset);
        break;
    case BorderLayout::RIGHT:
        setAnchorPoint(V2D(0.5, 0) * offset);
        break;
    case BorderLayout::BOTTOM_RIGHT:
        setAnchorPoint(V2D(0.5, -0.5) * offset);
        break;
    case BorderLayout::BOTTOM:
        setAnchorPoint(V2D(0, -0.5) * offset);
        break;
    case BorderLayout::BOTTOM_LEFT:
        setAnchorPoint(V2D(-0.5, -0.5) * offset);
        break;
    case BorderLayout::LEFT:
        setAnchorPoint(V2D(-0.5, 0) * offset);
        break;
    case BorderLayout::TOP_LEFT:
        setAnchorPoint(V2D(-0.5, 0.5) * offset);
        break;
    default:
        setAnchorPoint(V2D(0, 0) * offset);
    }

    _onContainerLayoutUpdate(this);
}

void CUI::Container::setBorderLayoutAnchor(BorderLayout border, V2D offset)
{
    switch (border) {
    case BorderLayout::TOP:
        setAnchorPoint(V2D(0, 0.5) * offset);
        break;
    case BorderLayout::TOP_RIGHT:
        setAnchorPoint(V2D(0.5, 0.5) * offset);
        break;
    case BorderLayout::RIGHT:
        setAnchorPoint(V2D(0.5, 0) * offset);
        break;
    case BorderLayout::BOTTOM_RIGHT:
        setAnchorPoint(V2D(0.5, -0.5) * offset);
        break;
    case BorderLayout::BOTTOM:
        setAnchorPoint(V2D(0, -0.5) * offset);
        break;
    case BorderLayout::BOTTOM_LEFT:
        setAnchorPoint(V2D(-0.5, -0.5) * offset);
        break;
    case BorderLayout::LEFT:
        setAnchorPoint(V2D(-0.5, 0) * offset);
        break;
    case BorderLayout::TOP_LEFT:
        setAnchorPoint(V2D(-0.5, 0.5) * offset);
        break;
    default:
        setAnchorPoint(V2D(0, 0) * offset);
    }

    _onContainerLayoutUpdate(this);
}

void CUI::Container::setBackgroundSprite(V2D padding, BgSpriteType type)
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
    //    ->enableDesignScaleIgnoring(V2D(2, 2)));
    _background->_roundRenderMatrix = false;
    Node::addChild(_background, -1);
}

void CUI::Container::setBackgroundSpriteCramped(V2D padding, V2D scale)
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

void CUI::Container::setBackgroundSpriteCramped2(V2D padding, V2D scale)
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

void CUI::Container::setBackgroundSpriteCramped3(V2D padding, V2D scale)
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

void CUI::Container::setBackgroundSpriteDarken(V2D padding) {
    if (_background) return;
    _backgroundPadding = padding;
    _background = ax::ui::Scale9Sprite::create("pixel.png");
    _background->setTag(YOURE_NOT_WELCOME_HERE);
    //_background->setProgramState(_backgroundShader);
    _background->setColor({ 128, 128, 128 });
    _background->setOpacity(20);
    //_background->addComponent((new UiRescaleComponent(Director::getInstance()->getVisibleSize()))
    //    ->enableDesignScaleIgnoring(V2D(2, 2)));
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
    _onContainerLayoutUpdate(this);
}

void CUI::Container::addSpecialChild(CUI::GUI* gui)
{
    _allButtons.push_back(gui);
    _onContainerLayoutUpdate(this);
}

CUI::Container* CUI::Container::addChildAsContainer(CUI::GUI* gui) {
    auto cont = CUI::Container::create();
    cont->setLayout(FlowLayout());
    cont->addChild(gui);
    cont->updateLayoutManagers();
    Node::addChild(cont);
    _onContainerLayoutUpdate(this);
    return cont;
}

void CUI::Container::recalculateChildDimensions()
{
    float s = Rebound::getInstance()->gameWindow.guiScale / 2;

    V2D scale = V2D(getScaleX() == 0 ? 1 : getScaleX(), getScaleY() == 0 ? 1 : getScaleY());
    scale *= 1.0 / s;
    if (_background) {
        _background->setContentSize(Math::getEven(getContentSize() * scale.x + _backgroundPadding * s));
        _background->setScaleX(1.0 / scale.x * (_background->getScaleX() / abs(_background->getScaleX())));
        _background->setScaleY(1.0 / scale.y * (_background->getScaleY() / abs(_background->getScaleY())));
    }

    if (_bgButton)
        _bgButton->setContentSize(getContentSize() + _backgroundPadding + BUTTON_HITBOX_CORNER_TOLERANCE);
}

void CUI::Container::setBackgroundVisible(bool visible)
{
    if (_background)
        _background->setVisible(visible);
}

void CUI::Container::calculateContentBoundaries()
{
    auto& list = getChildren();

    float highestX = -FLT_MAX;
    float highestY = -FLT_MAX;

    auto ns = _rescalingAllowed ? GameUtils::getNodeIgnoreDesignScale() : V2D::ONE;

    V2D highestSize = V2D::ZERO;

    float prevX = INVALID_LOCATION.x;
    float prevY = INVALID_LOCATION.y;

    for (auto& n : list) {
        auto _ = DCAST(GUI, n);
        if (!_ || _->getTag() <= YOURE_NOT_WELCOME_HERE) continue;
        //if (!isContainerDynamic()) continue;
        float eqX = abs(_->getPositionX());
        float eqY = abs(_->getPositionY());
        auto size = _->getScaledContentSize();

        if (eqX > highestX) {
            highestX = eqX;
            highestSize.x = size.x * ns.x;
        }

        if (eqY > highestY) {
            highestY = eqY;
            highestSize.y = size.y * ns.y;
        }

        if (prevX == eqX || !_isTightBoundaries)
            if (size.x * ns.y > highestSize.x)
                highestSize.x = size.x * ns.x;

        if (prevY == eqY || !_isTightBoundaries)
            if (size.y * ns.y > highestSize.y)
                highestSize.y = size.y * ns.y;

        prevX = eqX;
        prevY = eqY;
    }

    auto scaledMargin = V2D(
        _margin.x * 2 * ns.x,
        _margin.y * 2 * ns.y
    );

    setContentSize((V2D(isDynamicX() ? abs(highestX * 2 + highestSize.x + scaledMargin.x) : getContentSize().x,
        isDynamicY() ? abs(highestY * 2 + highestSize.y + scaledMargin.y) : getContentSize().y)), false);

    Container::recalculateChildDimensions();

    _onContainerLayoutUpdate(this);
}

void CUI::FlowLayout::build(CUI::Container* container)
{
    auto list = container->getChildren();
    auto ns = container->_rescalingAllowed ? GameUtils::getNodeIgnoreDesignScale() : V2D::ONE;
    //if (container->_ignoreDesignScale)
    //    ns = V2D::ONE;
    auto _spacing = V2D(spacing, spacing);
    F32 sumSize = 0;
    U16 listSize = 0;
    for (auto& n : list) {
        auto _ = DCAST(GUI, n);
        if (!_ || _->getTag() <= YOURE_NOT_WELCOME_HERE) continue;
        auto cSize = constSize ? constSizeV : (_->getScaledContentSize() / (!_->_flowLayoutRescalingAllowed ? V2D::ONE : ns));
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
    F32 cumSize = 0;
    if (direction == STACK_CENTER)
        cumSize = (sumSize - (_spacing.x * 1.5 * listSize) - _spacing.x / 2) / -2;
    for (auto& n : list) {
        auto _ = DCAST(GUI, n);
        if (!_ || _->getTag() <= YOURE_NOT_WELCOME_HERE) continue;
        auto cSize = constSize ? constSizeV : (_->getScaledContentSize() / (!_->_flowLayoutRescalingAllowed ? V2D::ONE : ns));
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

    V2D anchor = V2D::ZERO;

    switch (position) {
    case BorderLayout::TOP:
        anchor = (V2D(0, 0.5) + offset);
        break;
    case BorderLayout::TOP_RIGHT:
        anchor = (V2D(0.5, 0.5) + offset);
        break;
    case BorderLayout::RIGHT:
        anchor = (V2D(0.5, 0) + offset);
        break;
    case BorderLayout::BOTTOM_RIGHT:
        anchor = (V2D(0.5, -0.5) + offset);
        break;
    case BorderLayout::BOTTOM:
        anchor = (V2D(0, -0.5) + offset);
        break;
    case BorderLayout::BOTTOM_LEFT:
        anchor = (V2D(-0.5, -0.5) + offset);
        break;
    case BorderLayout::LEFT:
        anchor = (V2D(-0.5, 0) + offset);
        break;
    case BorderLayout::TOP_LEFT:
        anchor = (V2D(-0.5, 0.5) + offset);
        break;
    default:
        anchor = (V2D(0, 0) + offset);
    }

    //if (parent) anchor *= parent->getScale();

    if (worldPos)
        element->setPosition((parent->getWorldPosition() + worldPosOffset) - parent->getContentSize() * anchor);
    else
    {
        Size dScale = { 1920, 1080 };
        Size wFrame = Director::getInstance()->getOpenGLView()->getFrameSize();
        element->setPosition((parent->getContentSize() * anchor) + worldPosOffset * (wFrame / dScale));
    }
}

void CUI::ContentSizeConstraint::build(CUI::GUI* element) {
    V2D s = parent->getContentSize() + offset;
    auto ns = V2D::ONE;
    if (scaled && element->_rescalingAllowed)
        ns = GameUtils::getNodeIgnoreDesignScale();
    if (!element->isDynamicX()) s.x = lockX ? element->getPrefferedContentSize().x : MAX(minSize.x * Rebound::getInstance()->gameWindow.guiScale, s.x) * ns.x;
    if (!element->isDynamicY()) s.y = lockY ? element->getPrefferedContentSize().y : MAX(minSize.y * Rebound::getInstance()->gameWindow.guiScale, s.y) * ns.y;
    element->setContentSize(s);
}

CUI::Separator* CUI::Separator::create(V2D size)
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

bool CUI::EventPassClippingNode::hover(V2D mouseLocationInView, Camera* cam)
{
    if (mouseLocationInView == INVALID_LOCATION) return false;
    return child->hover(mouseLocationInView, cam);
}

bool CUI::EventPassClippingNode::press(V2D mouseLocationInView, Camera* cam)
{
    return child->press(mouseLocationInView, cam);
}

bool CUI::EventPassClippingNode::release(V2D mouseLocationInView, Camera* cam)
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
