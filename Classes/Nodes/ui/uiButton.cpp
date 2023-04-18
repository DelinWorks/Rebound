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

void CustomUi::Button::init(std::wstring _text, int _fontSize, Size _size)
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
        true
    );
}

void CustomUi::Button::init(std::wstring _text, std::string_view _fontname, i32 _fontsize,
    cocos2d::Rect _capinsets, cocos2d::Size _contentsize, cocos2d::Rect _clampregion,
    Size _clampoffset, std::string_view _normal_sp, bool _adaptToWindowSize,
    Color3B _selected_color, bool _allowExtend)
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
    selected_color = _selected_color;
    field = ax::Label::createWithTTF(ShapingEngine::render(_text), _fontname, _fontsize * _UiScale);
    sprite = ax::ui::Scale9Sprite::createWithSpriteFrameName(normal_sp, capinsets);
    sprite->setContentSize(_contentsize);
    sprite->setColor(selected_color);
    setContentSize(_contentsize);
    button = createPlaceholderButton();
    button->setEnabled(false);
    button->ignoreContentAdaptWithSize(false);
    addChild(sprite, 0);
    addChild(button);
    addChild(field);
    _callback = [] (Button*) {};
}

void CustomUi::Button::update(f32 dt) {
    auto dSize = getDynamicContentSize();
    setContentSize(dSize + _padding);
    HoverEffectGUI::update(dt);
}

bool CustomUi::Button::hover(ax::Vec2 mouseLocationInView, Camera* cam)
{
    if (!adaptToWindowSize && field->getContentSize().width / _UiScale > sprite->getContentSize().width)
        field->setScale(sprite->getContentSize().width / (field->getContentSize().width / _UiScale + capinsets.origin.x * 2) / _UiScale);
    else if (adaptToWindowSize)
        field->setScale(1 / _UiScale);

    sprite->setContentSize(Size(extend ? Math::clamp(field->getContentSize().width / _UiScale + clampoffset.width, clampregion.origin.x, adaptToWindowSize ?  Darkness::getInstance()->gameWindow.windowSize.width : clampregion.size.width) : clampregion.size.width,
        Math::clamp(field->getContentSize().height / _UiScale + clampoffset.height, clampregion.origin.y, adaptToWindowSize ? Darkness::getInstance()->gameWindow.windowSize.height : clampregion.size.height)));
    button->setContentSize(sprite->getContentSize());

    if (isEnabled())
    {
#if 1
        if (!_pCurrentHeldItem)
            hover_cv.setValue(_isHovered = button->hitTest(mouseLocationInView, cam, _NOTHING));

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
    sprite->runAction(fade);
    field->runAction(tint);
}

void CustomUi::Button::onDisable()
{
    defocus();
    auto fade = FadeTo::create(0.1f, 100);
    auto tint = TintTo::create(0.1f, Color3B::GRAY);
    sprite->runAction(fade);
    field->runAction(tint);
}

bool CustomUi::Button::press(ax::Vec2 mouseLocationInView, Camera* cam)
{
    if (!isEnabled())
        return false;
    if (button->hitTest(mouseLocationInView, cam, _NOTHING)) {
        _pCurrentHeldItem = this;
        onDisable();
        return true;
    }
    hover(mouseLocationInView, cam);
    return false;
}

bool CustomUi::Button::release(cocos2d::Vec2 mouseLocationInView, Camera* cam)
{
    onEnable();
    if (button->hitTest(mouseLocationInView, cam, _NOTHING)) {
        _callback(this);
        SoundGlobals::playUiHoverSound();
        return true;
    }
}

Size CustomUi::Button::getDynamicContentSize()
{
    return sprite->getContentSize();
}

void CustomUi::Button::onFontScaleUpdate(float scale)
{
    field->initWithTTF(field->getString(), desc.fontName, desc.fontSize * _PmtFontScale * scale);
    //field->enableShadow(Color4B(selected_color.r, selected_color.g, selected_color.b, 100), {1,-1}, 1);
    field->enableUnderline();
    //field->enableOutline(Color4B(selected_color.r, selected_color.g, selected_color.b, 35), 4);
    field->getFontAtlas()->setAliasTexParameters();
}
