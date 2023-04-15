#include "uiContainer.h"

CustomUi::Container* CustomUi::Container::create()
{
    Container* ref = new Container();
    if (ref->init())
    {
        ref->_contentSizeDebug = DrawNode::create(1);
        ref->_isContainer = true;
        ref->setDynamic();
        ref->addChild(ref->_contentSizeDebug, 99);
        ref->autorelease();
    }
    else
    {
        AX_SAFE_DELETE(ref);
    }
    return ref;
}

CustomUi::Container* CustomUi::Container::create(BorderLayout border, BorderContext context)
{
    auto ref = create();
    if (ref) {
        if (context == BorderContext::PARENT)
            ref->_closestStaticBorder = true;
        ref->addComponent((new UiRescaleComponent(Director::getInstance()->getVisibleSize()))
            ->setBorderLayout(ref->_borderLayout = border));
        return ref;
    }
    else return nullptr;
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

bool CustomUi::Container::click(cocos2d::Vec2 mouseLocationInView, cocos2d::Camera* cam)
{
    // reset the camera position so that hits are generated correctly.
    //cam->setPosition(Vec2::ZERO);
    auto& list = getChildren();
    bool isClickSwallowed = false;
    for (int i = list.size() - 1; i > -1; i--)
    {
        auto n = DCAST(GUI, list.at(i));
        if (n)
            if (n->click(isClickSwallowed ? Vec2(INFINITY, INFINITY) : mouseLocationInView, cam))
                isClickSwallowed = true;
    }
    return isClickSwallowed;
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

    calculateContentBoundaries();
    switch (_layout) {
    case Layout::FLOW: {
        _flowLayout.build(this);
        break;
    }
    default:
        break;
    }
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
        if (DCAST(DrawNode, _)) continue;
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
    _contentSizeDebug->clear();
    _contentSizeDebug->drawRect(-getContentSize() / 2, getContentSize() / 2, Color4B(Color3B::ORANGE, 50));
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
    for (auto& _ : list) {
        auto cSize = _->getContentSize();
        cSize.x += _spacing.x * 2;
        cSize.y += _spacing.y * 2;
        sumSize += sort == SORT_HORIZONTAL ? cSize.x : cSize.y;
    }

    float marginF = direction == STACK_RIGHT || direction == STACK_TOP ? margin : -margin;

    if (direction == STACK_BOTTOM || direction == STACK_LEFT)
        std::reverse(list.begin(), list.end());

    f32 cumSize = 0;
    if (direction == STACK_CENTER)
        cumSize = (sumSize - (_spacing.x * 1.5 * list.size()) - _spacing.x) / -2;
    for (auto& _ : list) {
        if (!_ || DCAST(DrawNode, _)) continue;
        auto cont = DCAST(Container, _);
        if (cont) cont->calculateContentBoundaries();
        auto cSize = _->getContentSize();
        if (_) {
            if (sort == SORT_HORIZONTAL) {
                cumSize += cSize.x / (direction == STACK_LEFT ? -2 : 2);
                cSize.x += _spacing.x;
                _->setPositionX(cumSize * (_->getScaleX() == 1 ? 1 : n->getScaleX()) + marginF);
                cumSize += cSize.x / (direction == STACK_LEFT ? -2 : 2);
            } else if (sort == SORT_VERTICAL) {
                cumSize += cSize.y / (direction == STACK_BOTTOM ? -2 : 2);
                cSize.y += _spacing.y;
                _->setPositionY(cumSize * (_->getScaleY() == 1 ? 1 : n->getScaleY()) + marginF);
                cumSize += cSize.y / (direction == STACK_BOTTOM ? -2 : 2);
            }
        }
        /*else continue;*/
    }
}
