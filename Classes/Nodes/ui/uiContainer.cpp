#include "uiContainer.h"

CustomUi::Container::Container() : _layout(LAYOUT_NONE), _constraint(CONSTRAINT_NONE),
                                   _borderLayout(BorderLayout::CENTER), _flowLayout(), _depConst()
{
    setCascadeOpacityEnabled(true);
    setAsContainer();
    setDynamic();
}

CustomUi::Container::~Container() {
}

void CustomUi::Container::setBorderLayout(BorderLayout border, BorderContext context) {
    if (context == BorderContext::PARENT)
        _closestStaticBorder = true;
    addComponent((new UiRescaleComponent(Director::getInstance()->getVisibleSize()))
        ->setBorderLayout(_borderLayout = border));
}

CustomUi::Container* CustomUi::Container::create()
{
    Container* ref = new Container();
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

void CustomUi::Container::setLayout(FlowLayout layout)
{
    _flowLayout = layout;
    _layout = Layout::LAYOUT_FLOW;
}

void CustomUi::Container::setConstraint(DependencyConstraint layout)
{
    _depConst = layout;
    _constraint = Constraint::CONSTRAINT_DEPENDENCY;
}

bool CustomUi::Container::hover(cocos2d::Vec2 mouseLocationInView, cocos2d::Camera* cam)
{
    _isHitSwallowed = false;
    auto& list = getChildren();
    for (int i = list.size() - 1; i > -1; i--)
    {
        auto n = DCAST(GUI, list.at(i));
        if (n)
            if (n->hover((_isHitSwallowed && _pCurrentHeldItem != n) ? Vec2(UINT16_MAX, UINT16_MAX) : mouseLocationInView, cam))
                _isHitSwallowed = true;
    }
    if (_bgButton && !_isHitSwallowed) {
        if (_bgButton->hitTest(mouseLocationInView, cam, nullptr))
            _isHitSwallowed = true;
    }
    return _isHitSwallowed || _isBlocking;
}

bool CustomUi::Container::press(cocos2d::Vec2 mouseLocationInView, cocos2d::Camera* cam)
{
    // reset the camera position so that hits are generated correctly.
    //cam->setPosition(Vec2::ZERO);
    bool isClickSwallowed = false;
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
    if (_isDismissible && !isClickSwallowed)
        removeFromParent();
    return isClickSwallowed || _isBlocking;
}

bool CustomUi::Container::release(cocos2d::Vec2 mouseLocationInView, cocos2d::Camera* cam)
{
    if (_pCurrentHeldItem) {
        auto b = _pCurrentHeldItem->release(mouseLocationInView, cam);
        _pCurrentHeldItem = nullptr;
        return b;
    }
    return false;
}

void CustomUi::Container::keyPress(EventKeyboard::KeyCode keyCode)
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

void CustomUi::Container::keyRelease(EventKeyboard::KeyCode keyCode)
{
    if (_pCurrentHeldItem) return;

    for (auto& _ : getFocusSet()) {
        _->keyRelease(keyCode);
        break;
    }
}

void CustomUi::Container::mouseScroll(EventMouse* event) {
    if (_pCurrentHoveredItem)
        _pCurrentHoveredItem->mouseScroll(event);
}

void CustomUi::Container::updateLayoutManagers(bool recursive)
{
    if (recursive) {
        auto& list = getChildren();
        for (auto& _ : list) {
            auto cast = DCAST(Container, _);
            if (cast)
                cast->updateLayoutManagers(true);
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
    default:
        break;
    }

    calculateContentBoundaries();

    if (recursive) {
        auto& list = getChildren();
        for (auto& _ : list) {
            auto cast = DCAST(Container, _);
            if (cast)
                cast->updateLayoutManagers(true);
        }
    }
}

void CustomUi::Container::onEnter() {
    GUI::onEnter();
    onFontScaleUpdate(_UiScale / _UiScaleMul);
    updateLayoutManagers(true);
}

void CustomUi::Container::onEnable()
{
}

void CustomUi::Container::onDisable()
{
}

void CustomUi::Container::setBorderLayoutAnchor(ax::Vec2 offset)
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

void CustomUi::Container::setBorderLayoutAnchor(BorderLayout border, ax::Vec2 offset)
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

void CustomUi::Container::setBackgroundSprite(ax::Vec2 padding, BgSpriteType type)
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
    Node::addChild(_background, -1);
}

void CustomUi::Container::setBackgroundSpriteCramped(ax::Vec2 padding, ax::Vec2 scale)
{
    _backgroundPadding = padding;
    _background = ax::ui::Scale9Sprite::createWithSpriteFrameName(ADVANCEDUI_TEXTURE_CRAMPED, ADVANCEDUI_P1_CAP_INSETS);
    _background->setTag(YOURE_NOT_WELCOME_HERE);
    _background->setScale(scale.x, scale.y);
    _background->setProgramState(_backgroundShader);
    Node::addChild(_background, -1);
}

void CustomUi::Container::setBackgroundDim()
{
    _bgDim = ax::LayerColor::create(Color4B(0, 0, 0, 100));
    auto visibleSize = Director::getInstance()->getVisibleSize();
    SET_POSITION_MINUS_HALF_SCREEN(_bgDim);
    _bgDim->setTag(YOURE_NOT_WELCOME_HERE);
    _bgDim->setScale(10);
    Node::addChild(_bgDim, -2);
}

void CustomUi::Container::setBlocking()
{
    _isBlocking = true;
}

void CustomUi::Container::setDismissible()
{
    _isDismissible = true;
}

void CustomUi::Container::setBackgroundBlocking()
{
    _bgButton = createPlaceholderButton();
    _bgButton->setContentSize(getContentSize());
    _bgButton->setTag(GUI_ELEMENT_EXCLUDE);
    addChild(_bgButton);
}

void CustomUi::Container::notifyLayout()
{
    updateLayoutManagers();
    GUI::notifyLayout();
}

void CustomUi::Container::addSpecialChild(CustomUi::GUI* gui)
{
    _allButtons.push_back(gui);
}

void CustomUi::Container::addChildAsContainer(CustomUi::GUI* gui) {
    auto cont = CustomUi::Container::create();
    cont->setTag(CONTAINER_FLOW_TAG);
    cont->addChild(gui);
    Node::addChild(cont);
}

void CustomUi::Container::calculateContentBoundaries()
{
    auto& list = getChildren();

    float highestX = -FLT_MAX;
    float highestY = -FLT_MAX;

    auto ns = GameUtils::getNodeIgnoreDesignScale();

    Vec2 highestSize = Vec2::ZERO;
    Vec2 dominantSize = Vec2::ZERO;

    for (auto& _ : list) {
        if (_->getTag() <= YOURE_NOT_WELCOME_HERE)
            continue;
        auto c = DCAST(Container, _);
        if (c) c->calculateContentBoundaries();
        if (!isContainerDynamic()) continue;
        auto size = _->getContentSize();
        float eq = _->getPositionX();
        if (eq > highestX) {
            highestX = eq;
            highestSize.x = size.x * (c ? 1 : ns.x);
        }

        eq = _->getPositionY();
        if (eq > highestY) {
            highestY = eq;
            highestSize.y = size.y * (c ? 1 : ns.y);
        }

        if (size.x > dominantSize.x) {
            highestSize.x = size.x * (c ? 1 : ns.x);
            dominantSize.x = size.x;
        }

        if (size.y > dominantSize.y) {
            highestSize.y = size.y * (c ? 1 : ns.y);
            dominantSize.y = size.y;
        }
    }

    auto scaledMargin = ax::Vec2(
        _margin.x * 2 * ns.x,
        _margin.y * 2 * ns.y
    );

    if (isContainerDynamic())
        setContentSize(Math::getEven(Vec2(highestX * 2 + highestSize.x + scaledMargin.x, highestY * 2 + highestSize.y + scaledMargin.y)), false);

    if (_background)
        _background->setContentSize(getContentSize() + _backgroundPadding);

    if (_bgButton)
        _bgButton->setContentSize(getContentSize());
}

void CustomUi::FlowLayout::build(CustomUi::Container* container)
{
    // WARNING: copying is intended because of list reversing
    auto list = container->getChildren();

    auto ns = GameUtils::getNodeIgnoreDesignScale();

    auto _spacing = Vec2(spacing, spacing);

    f32 sumSize = 0;
    u16 listSize = 0;
    for (auto& _ : list) {
        if (!_ || _->getTag() <= YOURE_NOT_WELCOME_HERE) continue;
        auto cont = DCAST(Container, _);
        if (cont) cont->calculateContentBoundaries();
        auto cSize = _->getContentSize();
        if (cSize.x != 0 && cSize.y != 0) {
            cSize.x += _spacing.x * 2;
            cSize.y += _spacing.y * 2;
            sumSize += sort == SORT_HORIZONTAL ? cSize.x : cSize.y;
            listSize++;
        }
    }

    float marginF = direction == STACK_RIGHT || direction == STACK_TOP ? margin : -margin;

    if (reverseStack)
        std::reverse(list.begin(), list.end());

    f32 cumSize = 0;

    if (direction == STACK_CENTER)
        cumSize = (sumSize - (_spacing.x * 1.5 * listSize) - _spacing.x / 2) / -2;
    for (auto& _ : list) {
        if (!_ || _->getTag() <= YOURE_NOT_WELCOME_HERE) continue;
        auto cSize = _->getContentSize();
        if (cSize.x == 0 || cSize.y == 0)
            continue;
        if (_) {
            if (sort == SORT_HORIZONTAL) {
                cumSize += cSize.x / (direction == STACK_LEFT ? -2 : 2);
                cSize.x += _spacing.x;
                _->setPositionX(Math::getEven(cumSize * (_->getTag() == CONTAINER_FLOW_TAG ? 1 : ns.x) + marginF));
                cumSize += cSize.x / (direction == STACK_LEFT ? -2 : 2);
            }
            else if (sort == SORT_VERTICAL) {
                cumSize += cSize.y / (direction == STACK_BOTTOM ? -2 : 2);
                cSize.y += _spacing.y;
                _->setPositionY(Math::getEven(cumSize * (_->getTag() == CONTAINER_FLOW_TAG ? 1 : ns.y) + marginF));
                cumSize += cSize.y / (direction == STACK_BOTTOM ? -2 : 2);
            }
        }
        /*else continue;*/
    }
}

void CustomUi::DependencyConstraint::build(CustomUi::GUI* element)
{
    auto container = DCAST(Container, parent);
    if (container) container->updateLayoutManagers();

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

    if (parent) anchor *= parent->getScale();

    if (worldPos)
        element->setPosition((parent->getWorldPosition() + worldPosOffset) - parent->getContentSize() * anchor);
    else
        element->setPosition(parent->getContentSize() * anchor);
}

CustomUi::Separator* CustomUi::Separator::create(Vec2 size)
{
    CustomUi::Separator* ref = new CustomUi::Separator();
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