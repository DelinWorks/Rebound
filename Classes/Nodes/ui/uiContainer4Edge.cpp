#include "uiContainer4Edge.h"

CustomUi::Container4Edge::Container4Edge()
{
}

CustomUi::Container4Edge* CustomUi::Container4Edge::create(Vec2 _prefferedSize)
{
    Container4Edge* ref = new Container4Edge();
    if (ref->init())
    {
        ref->prefferedSize = _prefferedSize;
        ref->setContentSize(_prefferedSize);
        ref->autorelease();
    }
    else
    {
        AX_SAFE_DELETE(ref);
    }
    return ref;
}

void CustomUi::Container4Edge::setChildTop(CustomUi::GUI* gui)
{
    if (top) top->removeFromParent();
    addChild(gui);
    top = (Container*)gui;
    top->setBorderLayoutAnchor(BOTTOM);
    top->setConstraint(DependencyConstraint(this, TOP));
}

void CustomUi::Container4Edge::setChildBottom(CustomUi::GUI* gui)
{
    if (bottom) bottom->removeFromParent();
    addChild(gui);
    bottom = (Container*)gui;
    bottom->setBorderLayoutAnchor(TOP);
    bottom->setConstraint(DependencyConstraint(this, BOTTOM));
}

void CustomUi::Container4Edge::setChildRight(CustomUi::GUI* gui)
{
    if (right) right->removeFromParent();
    addChild(gui);
    right = (Container*)gui;
    right->setBorderLayoutAnchor(RIGHT);
    right->setConstraint(DependencyConstraint(this, RIGHT));
}

void CustomUi::Container4Edge::setChildLeft(CustomUi::GUI* gui)
{
    if (left) left->removeFromParent();
    addChild(gui);
    left = (Container*)gui;
    left->setBorderLayoutAnchor(LEFT);
    left->setConstraint(DependencyConstraint(this, LEFT));
}

void CustomUi::Container4Edge::calculateContentBoundaries()
{
    Vec2 size = prefferedSize;

    size.x -= left ? left->getContentSize().x : 0;
    size.x -= right ? right->getContentSize().x : 0;
    size.y -= top ? top->getContentSize().y : 0;
    size.y -= bottom ? bottom->getContentSize().y : 0;

    if (top && size.x < top->getContentSize().x) size.x = top->getContentSize().x;
    if (bottom && size.x < bottom->getContentSize().x) size.x = bottom->getContentSize().x;

    if (right && size.y < right->getContentSize().y) size.y = right->getContentSize().y;
    if (left && size.y < left->getContentSize().y) size.y = left->getContentSize().y;

    setContentSize(size);
}

void CustomUi::Container4Edge::updateLayoutManagers(bool recursive)
{
    calculateContentBoundaries();
    if (top) top->updateLayoutManagers();
    if (bottom) bottom->updateLayoutManagers();
    if (right) right->updateLayoutManagers();
    if (left) left->updateLayoutManagers();
}
