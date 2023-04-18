#include "uiContainer.h"

CustomUi::Container::Container() : _layout(Layout::NONE), _borderLayout(BorderLayout::CENTER), _flowLayout(), _contentSizeDebug(nullptr)
{
    _contentSizeDebug = DrawNode::create(1);
    _isContainer = true;
    setDynamic();
#ifdef DRAW_NODE_DEBUG
    addChild(_contentSizeDebug, 99);
    _contentSizeDebug->setTag(YOURE_NOT_WELCOME_HERE);
#endif
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
    _layout = Layout::FLOW;
}

bool CustomUi::Container::hover(cocos2d::Vec2 mouseLocationInView, cocos2d::Camera* cam)
{
    _isHitSwallowed = false;
    auto& list = getChildren();
    for (int i = list.size() - 1; i > -1; i--)
    {
        auto n = DCAST(GUI, list.at(i));
        if (n)
            if (n->hover(_isHitSwallowed ? Vec2(INFINITY, INFINITY) : mouseLocationInView, cam))
                _isHitSwallowed = true;
    }
    return _isHitSwallowed;
}

bool CustomUi::Container::press(cocos2d::Vec2 mouseLocationInView, cocos2d::Camera* cam)
{
    // reset the camera position so that hits are generated correctly.
    //cam->setPosition(Vec2::ZERO);
    auto& list = getChildren();
    bool isClickSwallowed = false;
    for (int i = list.size() - 1; i > -1; i--)
    {
        auto n = DCAST(GUI, list.at(i));
        if (n)
            if (n->press(isClickSwallowed ? Vec2(INFINITY, INFINITY) : mouseLocationInView, cam))
                isClickSwallowed = true;
    }
    return isClickSwallowed;
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
    for (auto& _ : _focusedElements)
        _->keyPress(keyCode);
}

void CustomUi::Container::keyRelease(EventKeyboard::KeyCode keyCode)
{
    for (auto& _ : _focusedElements)
        _->keyRelease(keyCode);
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
    case Layout::FLOW: {
        _flowLayout.build(this);
        break;
    }
    default:
        break;
    }

    calculateContentBoundaries();
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

void CustomUi::Container::setBorderLayoutAnchor()
{
    switch (_borderLayout) {
    case BorderLayout::TOP:
        setAnchorPoint({ 0, 0.5 });
        break;
    case BorderLayout::TOP_RIGHT:
        setAnchorPoint({ 0.5, 0.5 });
        break;
    case BorderLayout::RIGHT:
        setAnchorPoint({ 0.5, 0 });
        break;
    case BorderLayout::BOTTOM_RIGHT:
        setAnchorPoint({ 0.5, -0.5 });
        break;
    case BorderLayout::BOTTOM:
        setAnchorPoint({ 0, -0.5 });
        break;
    case BorderLayout::BOTTOM_LEFT:
        setAnchorPoint({ -0.5, -0.5 });
        break;
    case BorderLayout::LEFT:
        setAnchorPoint({ -0.5, 0 });
        break;
    case BorderLayout::TOP_LEFT:
        setAnchorPoint({ -0.5, 0.5 });
        break;
    default:
        setAnchorPoint({ 0, 0 });
    }
}

void CustomUi::Container::setBackgroundSprite(ax::Vec2 padding)
{
    _backgroundPadding = padding;
    _background = ax::ui::Scale9Sprite::createWithSpriteFrameName(ADVANCEDUI_TEXTURE, ADVANCEDUI_P1_CAP_INSETS);
    _background->setTag(YOURE_NOT_WELCOME_HERE);
    addChild(_background, -1);
}

void CustomUi::Container::setBackgroundDim()
{
    _bgDim = ax::LayerColor::create(Color4B(0, 0, 0, 100));
    auto visibleSize = Director::getInstance()->getVisibleSize();
    SET_POSITION_MINUS_HALF_SCREEN(_bgDim);
    _bgDim->setTag(YOURE_NOT_WELCOME_HERE);
    addChild(_bgDim, -2);
}

void CustomUi::Container::notifyLayout()
{
    updateLayoutManagers();
    GUI::notifyLayout();
}

void CustomUi::Container::calculateContentBoundaries()
{
    auto& list = getChildren();

    float highestX = -FLT_MAX;
    float highestY = -FLT_MAX;

    auto n = Node::create();
    GameUtils::setNodeIgnoreDesignScale(n);

    Vec2 highestSize = Vec2::ZERO;
    Vec2 dominantSize = Vec2::ZERO;

    for (auto& _ : list) {
        if (_->getTag() == YOURE_NOT_WELCOME_HERE)
            continue;
        auto c = DCAST(Container, _);
        if (c) c->calculateContentBoundaries();
        if (!_isDynamic) continue;
        auto size = _->getContentSize();
        float eq = _->getPositionX();
        if (eq > highestX) {
            highestX = eq;
            highestSize.x = size.x * (_->getScaleX() == 1 ? 1 : n->getScaleX());
        }

        eq = _->getPositionY();
        if (eq > highestY) {
            highestY = eq;
            highestSize.y = size.y * (_->getScaleY() == 1 ? 1 : n->getScaleY());
        }

        if (size.x > dominantSize.x) {
            highestSize.x = size.x * (_->getScaleX() == 1 ? 1 : n->getScaleX());
            dominantSize.x = size.x;
        }

        if (size.y > dominantSize.y) {
            highestSize.y = size.y * (_->getScaleY() == 1 ? 1 : n->getScaleY());
            dominantSize.y = size.y;
        }
    }

    auto scaledMargin = ax::Vec2(
        _margin.x * 2 * n->getScaleX(),
        _margin.y * 2 * n->getScaleY()
    );

    if (_isDynamic)
        Node::setContentSize(Vec2(highestX * 2 + highestSize.x + scaledMargin.x, highestY * 2 + highestSize.y + scaledMargin.y));

#ifdef DRAW_NODE_DEBUG
    _contentSizeDebug->clear();
    _contentSizeDebug->drawRect(-getContentSize() / 2, getContentSize() / 2, Color4B(Color3B::ORANGE, 50));
#endif

    if (_background)
        _background->setContentSize(getContentSize() + _backgroundPadding);
}

void CustomUi::FlowLayout::build(CustomUi::Container* container)
{
    // WARNING: copying is intended because of list reversing
    auto list = container->getChildren();

    auto n = Node::create();
    GameUtils::setNodeIgnoreDesignScale(n);

    auto _spacing = Vec2(spacing, spacing);
    _spacing.x *= n->getScaleX();
    _spacing.y *= n->getScaleY();

    f32 sumSize = 0;
    u16 listSize = 0;
    for (auto& _ : list) {
        if (!_ || _->getTag() == YOURE_NOT_WELCOME_HERE) continue;
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

    if (direction == STACK_BOTTOM || direction == STACK_LEFT || direction == STACK_CENTER)
        std::reverse(list.begin(), list.end());

    f32 cumSize = 0;
    if (direction == STACK_CENTER)
        cumSize = (sumSize - (_spacing.x * 1.5 * listSize) - _spacing.x / 2) / -2;
    for (auto& _ : list) {
        if (!_ || _->getTag() == YOURE_NOT_WELCOME_HERE) continue;
        auto cSize = _->getContentSize();
        if (cSize.x == 0 || cSize.y == 0)
            continue;
        if (_) {
            if (sort == SORT_HORIZONTAL) {
                cumSize += cSize.x / (direction == STACK_LEFT ? -2 : 2);
                cSize.x += _spacing.x;
                _->setPositionX(cumSize * n->getScaleX() + marginF);
                cumSize += cSize.x / (direction == STACK_LEFT ? -2 : 2);
            }
            else if (sort == SORT_VERTICAL) {
                cumSize += cSize.y / (direction == STACK_BOTTOM ? -2 : 2);
                cSize.y += _spacing.y;
                _->setPositionY(cumSize * n->getScaleY() + marginF);
                cumSize += cSize.y / (direction == STACK_BOTTOM ? -2 : 2);
            }
        }
        /*else continue;*/
    }
}
