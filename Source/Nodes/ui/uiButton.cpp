#include "uiButton.h"

CUI::Button* CUI::Button::create()
{
    CUI::Button* ret = new CUI::Button();
    if (((Node*)ret)->init())
    {
        ret->autorelease();
    }
    else
    {
        AX_SAFE_DELETE(ret);
    }
    return ret;
}

void CUI::Button::init(std::wstring _text, int _fontSize, Size _size, Size _hitboxPadding)
{
    init(
        _text,
        _fontName,
        _fontSize,
        ADVANCEDUI_P1_CAP_INSETS,
        _size,
        Rect(_size.x, _size.y, _size.x, _size.y),
        BUTTON_P1_CLAMP_OFFSET,
        ADVANCEDUI_TEXTURE,
        true,
        Color3B(117, 179, 255),
        true,
        false,
        _hitboxPadding
    );
}

void CUI::Button::initIcon(std::string _frameName, Size _hitboxPadding)
{
    init(
        L"",
        "",
        0,
        Rect::ZERO,
        Size(0, 0),
        Rect(0, 0, 0, 0),
        Size(0, 0),
        _frameName,
        true,
        Color3B(117, 179, 255),
        true,
        true,
        _hitboxPadding
    );
}

void CUI::Button::initIcon(std::string _frameName, Rect _capInsets, Size _hitboxPadding)
{
    init(
        L"",
        "",
        0,
        _capInsets,
        Size(0, 0),
        Rect(0, 0, 0, 0),
        Size(0, 0),
        _frameName,
        true,
        Color3B(117, 179, 255),
        true,
        true,
        _hitboxPadding
    );
}

void CUI::Button::init(std::wstring _text, std::string_view _fontname, i32 _fontsize,
    cocos2d::Rect _capinsets, cocos2d::Size _contentsize, cocos2d::Rect _clampregion,
    Size _clampoffset, std::string_view _normal_sp, bool _adaptToWindowSize,
    Color3B _selected_color, bool _allowExtend, bool _isIcon , ax::Size _hitboxpadding)
{
    if (_rescalingAllowed)
        addComponent((new UiRescaleComponent(Director::getInstance()->getVisibleSize()))->enableDesignScaleIgnoring());
    desc.fontName = _fontname;
    desc.fontSize = _fontsize;
    adaptToWindowSize = _adaptToWindowSize;
    extend = _allowExtend;
    normal_sp = _normal_sp;
    clampregion = _clampregion;
    clampoffset = _clampoffset;
    capinsets = _capinsets;
    hitboxpadding = _hitboxpadding;
    selected_color = _selected_color;
    field_size = _contentsize;
    if (_isIcon) {
        if (_capinsets.equals(Rect::ZERO)) {
            icon = ax::ui::Scale9Sprite::createWithSpriteFrameName(_normal_sp);
            icon->setScale9Enabled(false);
            addChild(icon, 0);
        } else {
            icon = ax::ui::Scale9Sprite::createWithSpriteFrameName(_normal_sp);
            icon->setCapInsets(_capinsets);
            addChild(icon, 0);
        }
        icon->_roundRenderMatrix = true;
        preCompContentSize = (icon->getContentSize() * (_iconArtMulEnabled ? _PxArtMultiplier : _pretextIconScaling) + clampoffset);
        icon->setScale(_iconArtMulEnabled ? _PxArtMultiplier : _pretextIconScaling);
    }
    else {
        field = ax::Label::createWithBMFont(_fontname, ShapingEngine::render(_text));
        field->setAdditionalKerning(1);
        sprite = ax::ui::Scale9Sprite::createWithSpriteFrameName(normal_sp, capinsets);
        sprite->setContentSize(_contentsize);
        sprite->setColor(selected_color);
        sprite->setVisible(false);
        addChild(sprite, 0);
        addChild(field, 1);
        preCompContentSize = _contentsize;
    }
    button = createPlaceholderButton();
    addChild(button);
    _callback = [] (Button*) {};
    onFontScaleUpdate(1);
    update(0);
}

void CUI::Button::update(f32 dt) {
    auto dSize = getDynamicContentSize();
    if (setContentSize(dSize + getUiPadding(), _isContentSizeDynamic))
        updateInternalObjects();
    //HoverEffectGUI::update(dt, getContentSize() + Vec2(50 * (field_size.x != 0 || field_size.y != 0), 0));
}

bool CUI::Button::hover(ax::Vec2 mouseLocationInView, Camera* cam)
{
    if (!isVisible()) return false;
    if (isEnabled())
    {
#if 1
        if (!_pCurrentHeldItem) {
            setUiHovered(button->hitTest(mouseLocationInView, cam, _NOTHING));
            hover_cv.setValue(isUiHovered());
            _pCurrentHoveredTooltipItem = isUiHovered() ? this : (_pCurrentHoveredTooltipItem == this ? nullptr : _pCurrentHoveredTooltipItem);
            if (field) {
                if (isUiHovered())
                    field->enableUnderline();
                else
                    field->disableEffect(ax::LabelEffect::UNDERLINE);
            }
        }

        if (icon) {
            if (isUiHovered())
                icon->setPositionY(1);
            else
                icon->setPositionY(0);
        }

        //if (hover_cv.isChanged())
        //    HoverEffectGUI::hover();
#else
        hover->setValue(false);
        password_hover->setValue(false);
#endif
    }

    return hover_cv.getValue();
}

void CUI::Button::focus()
{
}

void CUI::Button::defocus()
{
}

void CUI::Button::onEnable()
{
    if (!_actionOnDisable) return;

    auto fade = FadeTo::create(1, 255);
    auto tint = TintTo::create(1, Color3B::WHITE);
    if (field) {
        field->runAction(fade);
        field->runAction(tint);
    }
    else if (runActionOnIcon) {
        icon->stopAllActions();
        icon->runAction(fade);
    }
}

void CUI::Button::onDisable()
{
    if (_actionOnDisable) {
        auto fade = FadeTo::create(0, 100);
        auto tint = TintTo::create(0, Color3B::GRAY);
        if (field) {
            field->runAction(fade);
            field->runAction(tint);
        }
        else if (runActionOnIcon) {
            icon->stopAllActions();
            icon->runAction(fade);
        }

        setUiHovered(false);
        hover_cv.setValue(false);
        //HoverEffectGUI::hover();
        defocus();
    }
    if (icon) icon->setPositionY(0);
}

bool CUI::Button::press(ax::Vec2 mouseLocationInView, Camera* cam)
{
    if (!isEnabled() || !isVisible()) return false;
    if (button->hitTest(mouseLocationInView, cam, _NOTHING)) {
        if (_pCurrentHeldItem) _pCurrentHeldItem->release(INVALID_LOCATION, cam);
        _pCurrentHeldItem = this;
        _currentHeldItemLocationInView = mouseLocationInView;
        auto fade = FadeTo::create(0, 100);
        if (field) {
            field->stopAllActions();
            field->runAction(fade);
        }
        else if (runActionOnIcon) {
            icon->stopAllActions();
            icon->runAction(fade);
        }
        return true;
    }
    hover(mouseLocationInView, cam);
    return false;
}

bool CUI::Button::release(cocos2d::Vec2 mouseLocationInView, Camera* cam)
{
    auto fade = FadeTo::create(0.1f, 255);
    if (field) {
        field->runAction(fade);
    }
    else if (runActionOnIcon) icon->runAction(fade);
    if (button->hitTest(mouseLocationInView, cam, _NOTHING)) {
        _callback(this);
        SoundGlobals::playUiHoverSound();
        return true;
    }
    return false;
}

Size CUI::Button::getDynamicContentSize()
{
    return preCompContentSize;
}

Size CUI::Button::getFitContentSize()
{
    if (!field) return Size();
    return field->getContentSize() + Vec2((_ForceOutline ? _PmtFontOutline : 1), 0);
}

void CUI::Button::enableIconHighlight()
{
    selected_color = Color3B(0, 237, 255);
    if (field) field->setColor(selected_color);
    if (icon && runActionOnIcon) icon->setColor(selected_color);
}

void CUI::Button::disableIconHighlight()
{
    if (field) field->setColor(Color3B::WHITE);
    if (icon && runActionOnIcon) icon->setColor(Color3B::WHITE);
}

float CUI::Button::preCalculatedHeight()
{
    return field->getContentSize().y * getScaleY() / _UiScale;
}

void CUI::Button::onFontScaleUpdate(float scale)
{
    if (field) {
        //field->initWithTTF(field->getString(), desc.fontName, desc.fontSize * _PmtFontScale * scale);
        if (field_size.x != 0 || field_size.y != 0) {
            field->setDimensions(field_size.x * scale * _PmtFontScale, field_size.y * scale * _PmtFontScale);
            field->setHorizontalAlignment(ax::TextHAlignment::LEFT);
            //_prtcl->setAngleVar(0);
            //_prtcl->setSpeed(40);
        }
        //if (_ForceOutline)
        //    field->enableOutline(Color4B(0, 0, 0, 255), _PmtFontOutline * scale);
        field->setBMFontSize(UINT16_MAX * _BMFontScale);
        field->getFontAtlas()->setAliasTexParameters();
    }
}

void CUI::Button::updateInternalObjects()
{
    if (field /* !isIcon */) {
        if (!adaptToWindowSize && field->getContentSize().width / GET_UI_SCALE_MUL > sprite->getContentSize().width)
            field->setScale(sprite->getContentSize().width / (field->getContentSize().width / GET_UI_SCALE_MUL + capinsets.origin.x * 2) / GET_UI_SCALE_MUL);
        else if (adaptToWindowSize)
            field->setScale(1 / GET_UI_SCALE_MUL);

        sprite->setContentSize(Size(extend ? Math::clamp(field->getContentSize().width / GET_UI_SCALE_MUL + clampoffset.width, clampregion.origin.x, adaptToWindowSize ? Darkness::getInstance()->gameWindow.windowSize.width : clampregion.size.width) : clampregion.size.width,
            Math::clamp((field->getContentSize().height - (_ForceOutline ? _PmtFontOutline * 2 * GET_UI_SCALE_MUL : 0)) / GET_UI_SCALE_MUL + clampoffset.height, clampregion.origin.y, adaptToWindowSize ? Darkness::getInstance()->gameWindow.windowSize.height : clampregion.size.height)));
        button->setContentSize(sprite->getContentSize() + getUiPadding() / 2 + hitboxpadding);
        preCompContentSize = sprite->getContentSize();
    }
    else
        button->setContentSize((icon->getContentSize() + getUiPadding() / 2 + hitboxpadding) * (_iconArtMulEnabled ? _PxArtMultiplier : _pretextIconScaling));
}

CUI::Button::~Button() {
    if (CUI::_pCurrentHeldItem == this)
        CUI::_pCurrentHeldItem = nullptr;
    if (CUI::_pCurrentHoveredTooltipItem == this)
        CUI::_pCurrentHoveredTooltipItem = nullptr;
    LOG_RELEASE;
}
