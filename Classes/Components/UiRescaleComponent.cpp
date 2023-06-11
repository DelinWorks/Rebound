#include "UiRescaleComponent.h"

USING_NS_GAMEUTILS;

UiRescaleComponent::UiRescaleComponent(Size _visibleSize) {
    autorelease();
    setName(__func__);
    setEnabled(true);
    _resizeHintsRect = Rect(2, 0, 2, 0);
}

UiRescaleComponent::UiRescaleComponent() {
    setName(__func__);
    setEnabled(true);
    _resizeHintsRect = Rect(2, 0, 2, 0);
}

void UiRescaleComponent::onAdd() {
    if (_isUiElemDirty) {
        windowSizeChange(Director::getInstance()->getOpenGLView()->getFrameSize());
        _isUiElemDirty = false;
    }
}

void UiRescaleComponent::onRemove()
{
}

UiRescaleComponent* UiRescaleComponent::enableLayerResizing() {
    _setLayerColor = true;
    _isUiElemDirty = true;
    return this;
}

UiRescaleComponent* UiRescaleComponent::enableDesignScaleIgnoring(ax::Vec2 identity) {
    _ignore = true;
    _isUiElemDirty = true;
    _identityScale = identity;
    return this;
}

UiRescaleComponent* UiRescaleComponent::setVisibleSizeHints(f32 widthDiv, f32 widthOffset, f32 heightDiv, f32 heightOffset) {
    _resizeHints = true;
    _resizeHintsRect = Rect(widthDiv, heightDiv, widthOffset, heightOffset);
    _isUiElemDirty = true;
    return this;
}

UiRescaleComponent* UiRescaleComponent::setBorderLayout(BorderLayout border) {
    switch (border) {
    case BorderLayout::TOP: {
        setVisibleSizeHints(0);
        break;
    }
    case BorderLayout::TOP_RIGHT: {
        setVisibleSizeHints();
        break;
    }
    case BorderLayout::RIGHT: {
        setVisibleSizeHints(2, 0, 0, 0);
        break;
    }
    case BorderLayout::BOTTOM_RIGHT: {
        setVisibleSizeHints(2, 0, -2, 0);
        break;
    }
    case BorderLayout::BOTTOM: {
        setVisibleSizeHints(0, 0, -2, 0);
        break;
    }
    case BorderLayout::BOTTOM_LEFT: {
        setVisibleSizeHints(-2, 0, -2, 0);
        break;
    }
    case BorderLayout::LEFT: {
        setVisibleSizeHints(-2, 0, 0, 0);
        break;
    }
    case BorderLayout::TOP_LEFT: {
        setVisibleSizeHints(-2, 0, 2, 0);
        break;
    }
    default: {
        setVisibleSizeHints(0, 1, 0, 0);
    }
    }
    return this;
}

UiRescaleComponent* UiRescaleComponent::enableSizeFitting(Size _sizeInPixels) {
    _fitting = true;
    _fittingSize = _sizeInPixels;
    _isUiElemDirty = true;
    return this;
}

void UiRescaleComponent::windowSizeChange(Size newVisibleSize) {

    auto repositionNode = [&](Node* target) {
        auto newPos = Vec2(_resizeHintsRect.origin.x == 0 ? 0 : newVisibleSize.width / _resizeHintsRect.origin.x + _resizeHintsRect.size.width,
            _resizeHintsRect.origin.y == 0 ? 0 : newVisibleSize.height / _resizeHintsRect.origin.y + _resizeHintsRect.size.height);
        target->setPosition(Vec2(newPos.x, newPos.y));
    };

    if (_setLayerColor)
    {
        //auto parent = layer->getParent();
        //auto order = layer->getLocalZOrder();
        //layer->removeFromParentAndCleanup(true);
        //layer = LayerColor::create(layerColor);
        //parent->addChild(layer, order);
        ((LayerColor*)_owner)->changeWidthAndHeight(newVisibleSize.width, newVisibleSize.height);
        if (_resizeHints) repositionNode(_owner);

        return;
    }
    if (_ignore && !_fitting)
    {
        setNodeIgnoreDesignScale(_owner);
        setNodeScaleFHD(_owner);
        _owner->setScaleX(_owner->getScaleX() * _identityScale.x);
        _owner->setScaleY(_owner->getScaleY() * _identityScale.y);
        //_owner->setScale(_owner->getScaleX() * 1);
    }
    //if (fitting && !ignore)
    //{
    //    auto winsize = Darkness::getInstance()->windowSize;

    //    setNodeIgnoreDesignScale(_owner);

    //    float finalScale = 0;

    //    //if (winsize.width < fittingSize.width && winsize.width <= winsize.height)
    //    //    finalScale = winsize.width / (fittingSize.width > fittingSize.height ? fittingSize.width : fittingSize.height);

    //    if (winsize.height < fittingSize.height && winsize.width > winsize.height)
    //        finalScale = winsize.height / (fittingSize.width > fittingSize.height ? fittingSize.width : fittingSize.height);
    //    
    //    _owner->setScale(_owner->getScale() - finalScale);

    //    if (winsize.width > fittingSize.width && winsize.height > fittingSize.height)
    //        setNodeIgnoreDesignScale(_owner);
    //}
    if (_resizeHints) repositionNode(_owner);
}
