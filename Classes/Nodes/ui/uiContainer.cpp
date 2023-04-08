#include "uiContainer.h"

CustomUi::Container* CustomUi::Container::create()
{
    Container* ref = new Container();
    if (ref->init())
    {
        ref->contentSizeDebug = DrawNode::create(1);
        ref->_isContainer = true;
        ref->setDynamic();
        ref->addChild(ref->contentSizeDebug, 99);
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
        if (context == BorderContext::CLOSEST_STATIC)
            ref->_closestStaticBorder = true;
        ref->addComponent((new UiRescaleComponent(Director::getInstance()->getVisibleSize()))
            ->setBorderLayout(ref->borderLayout = border));
        return ref;
    }
    else return nullptr;
}

void CustomUi::Container::setLayout(FlowLayout _layout)
{
    flowLayout = _layout;
    layout = Layout::FLOW;
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
    //std::cout << "Updated layout of container: " << this << "\n";
    calculateContentBoundaries();
    switch (layout) {
    case Layout::FLOW: {
        flowLayout.build(this);
        break;
    }
    default:
        break;
    }

    if (recursive) {
        auto& list = getChildren();
        for (auto& _ : list) {
            auto cast = DCAST(Container, _);
            if (cast)
                cast->updateLayoutManagers(true);
        }
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
    switch (borderLayout) {
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
    }
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
        float eq = _->getPositionX();
        if (eq > highestX) {
            highestX = eq;
            highestSize.x = _->getContentSize().x * n->getScaleX();
        }

        eq = _->getPositionY();
        if (eq > highestY) {
            highestY = eq;
            highestSize.y = _->getContentSize().y * n->getScaleY();
        }

        if (_->getContentSize().x > dominantSize.x) {
            highestSize.x = _->getContentSize().x * n->getScaleX();
            dominantSize.x = _->getContentSize().x;
        }

        if (_->getContentSize().y > dominantSize.y) {
            highestSize.y = _->getContentSize().y * n->getScaleY();
            dominantSize.y = _->getContentSize().y;
        }
    }

    if (_isDynamic)
        setContentSize(Vec2(highestX * 2 + highestSize.x, highestY * 2 + highestSize.y));
    contentSizeDebug->clear();
    contentSizeDebug->drawRect(-getContentSize() / 2, getContentSize() / 2, Color4B(Color3B::ORANGE, 50));
}

void CustomUi::FlowLayout::build(CustomUi::Container* container, u16 start)
{
    auto list = container->getChildren();

    f32 sumSize = 0;
    for (auto& _ : list) {
        auto cSize = _->getContentSize();
        cSize.x += spacing.x * 2;
        cSize.y += spacing.y * 2;
        sumSize += sort == SORT_HORIZONTAL ? cSize.x : cSize.y;
    }

    auto n = Node::create();
    GameUtils::setNodeIgnoreDesignScale(n);

    if (direction == STACK_BOTTOM || direction == STACK_LEFT)
        std::reverse(list.begin(), list.end());

    f32 cumSize = 0;
    if (direction == STACK_CENTER)
        cumSize = sumSize / -2;
    for (auto& _ : list) {
        if (DCAST(DrawNode, _)) continue;
        auto cSize = _->getContentSize();
        if (DCAST(Container, _)) cSize *= 1.0 / n->getScaleX();
        if (_) {
            if (sort == SORT_HORIZONTAL) {
                cumSize += cSize.x / (direction == STACK_LEFT ? -2 : 2);
                cSize.x += spacing.x * 2;
                cSize.y += spacing.y * 2;
                _->setPositionX(cumSize * n->getScaleX());
                cumSize += cSize.x / (direction == STACK_LEFT ? -2 : 2);
            } else if (sort == SORT_VERTICAL) {
                cumSize += cSize.y / (direction == STACK_BOTTOM ? -2 : 2);
                cSize.x += spacing.x * 2;
                cSize.y += spacing.y * 2;
                _->setPositionY(cumSize * n->getScaleY());
                cumSize += cSize.y / (direction == STACK_BOTTOM ? -2 : 2);
            }
        }
        /*else continue;*/
    }
}
