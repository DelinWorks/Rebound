#include "uiButton.h"

CustomUi::Button* CustomUi::Button::create()
{
    CustomUi::Button* ret = new CustomUi::Button();
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

CustomUi::Button::~Button() {
    if (CustomUi::_pCurrentHeldItem == this)
        CustomUi::_pCurrentHeldItem = nullptr;
}

void CustomUi::Button::init(std::wstring _text, int _fontSize, Size _size, Size _hitboxPadding)
{
    init(
        _text,
        "fonts/bitsy-font-with-arabic.ttf"sv,
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

void CustomUi::Button::initIcon(std::string _frameName, Size _hitboxPadding)
{
    init(
        L"",
        "fonts/bitsy-font-with-arabic.ttf"sv,
        0,
        ADVANCEDUI_P1_CAP_INSETS,
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

void CustomUi::Button::init(std::wstring _text, std::string_view _fontname, i32 _fontsize,
    cocos2d::Rect _capinsets, cocos2d::Size _contentsize, cocos2d::Rect _clampregion,
    Size _clampoffset, std::string_view _normal_sp, bool _adaptToWindowSize,
    Color3B _selected_color, bool _allowExtend, bool _isIcon , ax::Size _hitboxpadding)
{
    desc.fontName = _fontname;
    desc.fontSize = _fontsize;
    addComponent((new UiRescaleComponent(Director::getInstance()->getVisibleSize()))->enableDesignScaleIgnoring());
    scheduleUpdate();
    adaptToWindowSize = _adaptToWindowSize;
    extend = _allowExtend;
    normal_sp = _normal_sp;
    clampregion = _clampregion;
    clampoffset = _clampoffset;
    capinsets = _capinsets;
    hitboxpadding = _hitboxpadding;
    selected_color = _selected_color;
    setContentSize(_contentsize);
    if (_isIcon) {
        icon = ax::Sprite::createWithSpriteFrameName(_normal_sp);
        addChild(icon, 0);
    }
    else {
        field = ax::Label::createWithTTF(ShapingEngine::render(_text), _fontname, _fontsize * _UiScale);
        sprite = ax::ui::Scale9Sprite::createWithSpriteFrameName(normal_sp, capinsets);
        sprite->setContentSize(_contentsize);
        sprite->setColor(selected_color);
        sprite->setVisible(false);
        addChild(sprite, 0);
        addChild(field, 1);
    }
    button = createPlaceholderButton();
    button->setEnabled(false);
    button->ignoreContentAdaptWithSize(false);
    addChild(button);
    _callback = [] (Button*) {};
}

void CustomUi::Button::update(f32 dt) {
    auto dSize = getDynamicContentSize();
    setContentSize(dSize + getUiPadding());
    HoverEffectGUI::update(dt);
}

bool CustomUi::Button::hover(ax::Vec2 mouseLocationInView, Camera* cam)
{
    if (field /* !isIcon */) {
        if (!adaptToWindowSize && field->getContentSize().width / _UiScale > sprite->getContentSize().width)
            field->setScale(sprite->getContentSize().width / (field->getContentSize().width / _UiScale + capinsets.origin.x * 2) / _UiScale);
        else if (adaptToWindowSize)
            field->setScale(1 / _UiScale);

        sprite->setContentSize(Size(extend ? Math::clamp(field->getContentSize().width / _UiScale + clampoffset.width, clampregion.origin.x, adaptToWindowSize ? Darkness::getInstance()->gameWindow.windowSize.width : clampregion.size.width) : clampregion.size.width,
            Math::clamp(field->getContentSize().height / _UiScale + clampoffset.height, clampregion.origin.y, adaptToWindowSize ? Darkness::getInstance()->gameWindow.windowSize.height : clampregion.size.height)));
        button->setContentSize(sprite->getContentSize() + getUiPadding() / 2 + hitboxpadding);
    }
    else
        button->setContentSize((icon->getContentSize() + getUiPadding() / 2 + hitboxpadding) * _PxArtMultiplier);

    if (isEnabled())
    {
#if 1
        if (!_pCurrentHeldItem) {
            setUiHovered(button->hitTest(mouseLocationInView, cam, _NOTHING));
            hover_cv.setValue(isUiHovered());
        }

        if (hover_cv.isChanged())
            HoverEffectGUI::hover();
#else
        hover->setValue(false);
        password_hover->setValue(false);
#endif
    }

    return hover_cv.getValue();
}

void CustomUi::Button::focus()
{
}

void CustomUi::Button::defocus()
{
}

void CustomUi::Button::onEnable()
{
    auto fade = FadeTo::create(0.1f, 255);
    auto tint = TintTo::create(0.1f, Color3B::WHITE);
    if (field) {
        field->runAction(fade);
        field->runAction(tint);
    }
    else icon->runAction(fade);
}

void CustomUi::Button::onDisable()
{
    auto fade = FadeTo::create(0.1f, 100);
    auto tint = TintTo::create(0.1f, Color3B::GRAY);
    if (field) {
        field->runAction(fade);
        field->runAction(tint);
    }
    else icon->runAction(fade);

    setUiHovered(false);
    hover_cv.setValue(false);
    HoverEffectGUI::hover();
    defocus();
}

bool CustomUi::Button::press(ax::Vec2 mouseLocationInView, Camera* cam)
{
    if (!isEnabled())
        return false;
    if (button->hitTest(mouseLocationInView, cam, _NOTHING)) {
        if (_pCurrentHeldItem) _pCurrentHeldItem->release({ INFINITY, INFINITY }, cam);
        _pCurrentHeldItem = this;
        auto fade = FadeTo::create(0, 100);
        auto tint = TintTo::create(0, Color3B::GRAY);
        if (field) {
            field->runAction(fade);
            field->runAction(tint);
        }
        else icon->runAction(fade);
        return true;
    }
    hover(mouseLocationInView, cam);
    return false;
}

bool CustomUi::Button::release(cocos2d::Vec2 mouseLocationInView, Camera* cam)
{
    auto fade = FadeTo::create(0.1f, 255);
    auto tint = TintTo::create(0.1f, Color3B::WHITE);
    if (field) {
        field->runAction(fade);
        field->runAction(tint);
    }
    else icon->runAction(fade);
    if (button->hitTest(mouseLocationInView, cam, _NOTHING)) {
        _callback(this);
        SoundGlobals::playUiHoverSound();
        return true;
    }
}

Size CustomUi::Button::getDynamicContentSize()
{
    return sprite ? sprite->getContentSize() : (icon->getContentSize() * _PxArtMultiplier + clampoffset);
}

void CustomUi::Button::onFontScaleUpdate(float scale)
{
    if (field) {
        field->initWithTTF(field->getString(), desc.fontName, desc.fontSize * _PmtFontScale * scale);
        //field->enableShadow(Color4B(selected_color.r, selected_color.g, selected_color.b, 100), {1,-1}, 1);
        //field->enableUnderline();
        //field->enableOutline(Color4B(selected_color.r, selected_color.g, selected_color.b, 35), 4);
        field->getFontAtlas()->setAliasTexParameters();
    }
    else icon->setScale(/*1.0 / scale * */_PxArtMultiplier);
}
