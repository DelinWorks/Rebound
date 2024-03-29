#include "uiContainer4Edge.h"

CUI::Container4Edge::Container4Edge()
{
}

CUI::Container4Edge* CUI::Container4Edge::create(V2D _prefferedSize)
{
    Container4Edge* ref = new Container4Edge();
    if (ref->init())
    {
        ref->_prefferedSize = _prefferedSize;
        ref->setContentSize(_prefferedSize);
        ref->autorelease();
    }
    else
    {
        AX_SAFE_DELETE(ref);
    }
    return ref;
}

void CUI::Container4Edge::setChildTop(CUI::GUI* gui)
{
    if (top) top->removeFromParent();
    addChild(gui);
    top = (Container*)gui;
    top->setBorderLayoutAnchor(BOTTOM);
    top->setConstraint(DependencyConstraint(this, TOP));
}

void CUI::Container4Edge::setChildBottom(CUI::GUI* gui)
{
    if (bottom) bottom->removeFromParent();
    addChild(gui);
    bottom = (Container*)gui;
    bottom->setBorderLayoutAnchor(TOP);
    bottom->setConstraint(DependencyConstraint(this, BOTTOM));
}

void CUI::Container4Edge::setChildRight(CUI::GUI* gui)
{
    if (right) right->removeFromParent();
    addChild(gui);
    right = (Container*)gui;
    right->setBorderLayoutAnchor(LEFT);
    right->setConstraint(DependencyConstraint(this, RIGHT));
}

void CUI::Container4Edge::setChildLeft(CUI::GUI* gui)
{
    if (left) left->removeFromParent();
    addChild(gui);
    left = (Container*)gui;
    left->setBorderLayoutAnchor(RIGHT);
    left->setConstraint(DependencyConstraint(this, LEFT));
}

void CUI::Container4Edge::calculateContentBoundaries()
{
    V2D size = _prefferedSize;

    auto ns = GameUtils::getNodeIgnoreDesignScale();

    size.x -= left ? left->getContentSize().x * ns.x : 0;
    size.x -= right ? right->getContentSize().x * ns.x : 0;
    size.y -= top ? top->getContentSize().y * ns.y : 0;
    size.y -= bottom ? bottom->getContentSize().y * ns.y : 0;

    if (top && size.x < top->getContentSize().x) size.x = top->getContentSize().x;
    if (bottom && size.x < bottom->getContentSize().x) size.x = bottom->getContentSize().x;

    if (right && size.y < right->getContentSize().y) size.y = right->getContentSize().y;
    if (left && size.y < left->getContentSize().y) size.y = left->getContentSize().y;

    setContentSize(size, false);
    Container::recalculateChildDimensions();
}

void CUI::Container4Edge::updateLayoutManagers(bool recursive)
{
    Container::updateLayoutManagers(recursive);
    if (top) top->updateLayoutManagers(recursive);
    if (bottom) bottom->updateLayoutManagers(recursive);
    if (right) right->updateLayoutManagers(recursive);
    if (left) left->updateLayoutManagers(recursive);
}

CUI::Container4Edge::~Container4Edge()
{
    LOG_RELEASE;
}
