#include "uiImageView.h"

CUI::ImageView* CUI::ImageView::create(Size _contentsize, ax::Texture2D* texture, bool rescalingAllowed)
{
    CUI::ImageView* ret = new CUI::ImageView();
    if (ret->init(_contentsize, texture, rescalingAllowed))
    {
        ret->autorelease();
    }
    else
    {
        AX_SAFE_DELETE(ret);
    }
    return ret;
}

bool CUI::ImageView::init(Size _contentsize, ax::Texture2D* texture, bool rescalingAllowed) {
    if (_rescalingAllowed = rescalingAllowed)
        addComponent((new UiRescaleComponent(Director::getInstance()->getVisibleSize()))->enableDesignScaleIgnoring());
    button = createPlaceholderButton();
    auxButton = createPlaceholderButton();
    //button->setOpacity(0);
    //auxButton->setColor(Color3B::RED);
    //auxButton->setOpacity(0);
    auxButton->setAnchorPoint({ 0, 0 });
    imageP = ax::Node::create();
    button->setContentSize(_contentsize);
    scissor = ClippingRectangleNode::create(
        Rect(-_contentsize.x / 2, -_contentsize.y / 2, _contentsize.x, _contentsize.y));
    texture->setAliasTexParameters();
    image = Sprite::createWithTexture(texture);
    textureSize = Size(texture->getPixelsWide(), texture->getPixelsHigh());
    bg = Sprite::create("pixel.png");
    bg->setContentSize(textureSize);
    grid = DrawNode::create(1);
    selection = DrawNode::create(1);
    auxButton->setContentSize(image->getContentSize());
    image->setAnchorPoint({ 0, 1 });
    bg->setAnchorPoint({ 0, 1 });
    bg->setColor({ 128,128,128 });
    image->addChild(auxButton);
    imageP->addChild(bg);
    imageP->addChild(image);
    image->addChild(grid);
    image->addChild(selection);
    grid->setPosition(0, textureSize.y);
    selection->setPosition(0, textureSize.y);
    scissor->addChild(imageP);
    addChild(scissor);
    addChild(button);
    setContentSize(_contentsize);

    return Node::init();
}

bool CUI::ImageView::hover(V2D mouseLocationInView, Camera* cam)
{
    if (isEnabled())
    {
        if (_pCurrentHeldItem == this && !IS_LOCATION_INVALID(mouseLocationInView)) {
            auto oldMousePos = mousePos;
            mousePos = _savedLocationInView;
            image->setPosition(image->getPositionX() + (mousePos.x - oldMousePos.x) * (1.0 / (imageP->getScaleX() * getScaleX())),
                image->getPositionY() + (mousePos.y - oldMousePos.y) * (1.0 / (imageP->getScaleX() * getScaleX())));
            image->setPositionX(Math::clamp(image->getPositionX(), -textureSize.x, 0));
            image->setPositionY(Math::clamp(image->getPositionY(), 0, textureSize.y));
            bg->setPosition(image->getPosition());
            if (!pressLocation.fuzzyEquals(mouseLocationInView, 4))
                pressLocation = INVALID_LOCATION;
        }
        bool b = button->hitTest(mouseLocationInView, cam, nullptr);
        hover_cv.setValue(b);
        if (b) {
            _pCurrentHoveredItem = this;
            if (hover_cv.isChanged() && hover_cv.getValue())
                SoundGlobals::playUiHoverSound();
        }
        else if (_pCurrentHoveredItem == this)
            _pCurrentHoveredItem = nullptr;
        return isUiEnabled() && b;
    }
}

bool CUI::ImageView::press(V2D mouseLocationInView, Camera* cam)
{
    if (isEnabled() && button->hitTest(mouseLocationInView, cam, nullptr)) {
        _pCurrentHeldItem = this;
        mousePos = mouseLocationInView;
        pressLocation = mousePos;
        SoundGlobals::playUiHoldSound();
    }
    return false;
}

bool CUI::ImageView::release(V2D mouseLocationInView, Camera* cam)
{
    if (_pCurrentHeldItem == this) {
        if (pressLocation.fuzzyEquals(mouseLocationInView, 3)) {
            _pCurrentHeldItem = nullptr;
            V3D hitP;
            auxButton->hitTest(mouseLocationInView, cam, &hitP);
            if (hitP.x > 0.0 && textureSize.y - hitP.y > 0 && hitP.x < textureSize.x && textureSize.y - hitP.y < textureSize.y) {
                auto pos = V2D(floor(hitP.x / gridSize.y), floor((textureSize.y - hitP.y) / gridSize.y));
                selection->clear();
                selection->drawSolidRect(V2D(pos.x * gridSize.x, -(pos.y * gridSize.y)),
                    V2D(pos.x * gridSize.x + gridSize.x, -(pos.y * gridSize.y + gridSize.y)),
                    Color4F(0, 0.58f, 1.0f, 0.5f));
                selectedIndex = pos.y * (textureSize.x / gridSize.x) + pos.x;
                SoundGlobals::playUiClickSound();
            }
        }
    }
    return false;
}

void CUI::ImageView::mouseScroll(ax::EventMouse* event)
{
    imageP->setScale(imageP->getScaleX() + (event->getScrollY() * -0.25));
    imageP->setScale(Math::clamp(imageP->getScaleX(), 0.25, 5));
}

void CUI::ImageView::enableGridSelection(S2D _gridsize)
{
    gridSize = _gridsize;
    textureSize = Size(CMF(textureSize.x, gridSize.x), CMF(textureSize.y, gridSize.y));
    image->setTextureRect(Rect(0, 0, textureSize.x, textureSize.y));
    grid->setPosition(0, textureSize.y);
    selection->setPosition(0, textureSize.y);

    grid->clear();
    for (int x = 0; x <= textureSize.x / gridSize.x; x++)
        grid->drawLine(V2D(x * gridSize.x, 0), V2D(x * gridSize.x, -textureSize.y), Color4B::BLACK);
    for (int y = -textureSize.y / gridSize.y; y <= 0; y++)
        grid->drawLine(V2D(0, y * gridSize.y), V2D(textureSize.x, y * gridSize.y), Color4B::BLACK);
}

void CUI::ImageView::onEnable()
{
}

void CUI::ImageView::onDisable()
{
}

V2D CUI::ImageView::getScaledContentSize()
{
    return _contentSize / (_rescalingAllowed ? V2D::ONE : GameUtils::getNodeIgnoreDesignScale());
}

CUI::ImageView::~ImageView()
{
    LOG_RELEASE;
}
