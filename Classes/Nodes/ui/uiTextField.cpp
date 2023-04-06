#include "uiTextField.h"
#include "2d/CCTweenFunction.h"

CustomUi::TextField* CustomUi::TextField::create()
{
    CustomUi::TextField* ret = new CustomUi::TextField();
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

void CustomUi::TextField::init(std::string_view _placeholder, int _fontSize, Size _size, int maxLength, std::string_view allowedChars)
{
    init(
        _placeholder,
        "fonts/arial.ttf"sv,
        _fontSize,
        false,
        ADVANCEDUI_P1_CAP_INSETS,
        _size,
        Rect(150, 40, 150, 40),
        TEXTFIELD_P1_CLAMP_OFFSET,
        ADVANCEDUI_TEXTURE,
        true,
        Color3B(117, 179, 255),
        true,
        maxLength,
        false,
        allowedChars
    );
}

void CustomUi::TextField::init(std::string_view _placeholder, std::string_view _fontname, i32 _fontsize, bool _password,
    ax::Rect _capinsets, ax::Size _contentsize, ax::Rect _clampregion,
    Size _clampoffset, std::string_view _normal_sp, bool _adaptToWindowSize,
    Color3B _selected_color, bool _allowExtend, i32 length, bool _toUpper,
    std::string_view _allowedChars)
{
    scheduleUpdate();
    adaptToWindowSize = _adaptToWindowSize;
    extend = _allowExtend;
    toUpper = _toUpper;
    allowedChars = _allowedChars;
    normal_sp = _normal_sp;
    clampregion = _clampregion;
    clampoffset = _clampoffset;
    capinsets = _capinsets;
    selected_color = _selected_color;
    password = _password;
    cursor_control_parent = ax::Node::create();
    field = ax::ui::TextField::create(_placeholder, _fontname, _fontsize);
    if (length != -1) {
        field->setMaxLengthEnabled(true);
        field->setMaxLength(length);
    }
    field->setPasswordEnabled(_password);
    field->setEnabled(false);
    field->_textFieldRenderer->setVerticalAlignment(TextVAlignment::CENTER);
    field->_textFieldRenderer->setHorizontalAlignment(TextHAlignment::CENTER);
    sprite = ax::ui::Scale9Sprite::createWithSpriteFrameName(normal_sp, capinsets);
    sprite->setContentSize(_contentsize);
    button = createPlaceholderButton();
    button->setEnabled(false);
    cursor_control = Sprite::createWithSpriteFrameName("text_cursor");
    if (password) {
        password_control = Sprite::createWithSpriteFrameName("hide_password");
        addChild(password_control, 2);
        password_control_button = createPlaceholderButton();
        //password_control_button->ignoreContentAdaptWithSize(false);
        //password_control_button->setContentSize(Size(password_control->getContentSize().width + 15, password_control->getContentSize().height + 15));
        //password_control->addChild(password_control_button);
        hookPlaceholderButtonToNode(password_control, password_control_button, Size(15, 10), true);
        //password_control_button->setPosition(Vec2(password_control->getBoundingBox().size.width / 2, password_control->getBoundingBox().size.height / 2));
        password_control_button->addTouchEventListener([&](Ref* sender, ui::Widget::TouchEventType type) {
            switch (type)
            {
            case ui::Widget::TouchEventType::BEGAN:
            {
                password_control->setScale(1.1f);
                auto scale = ScaleTo::create(0.2f, 1);
                auto ease = EaseCubicActionOut::create(scale);
                password_control->runAction(ease);
                if (!show_password) {
                    show_password = true;
                    password_control->setSpriteFrame("show_password");
                    field->setPasswordEnabled(false);
                    field->setString(field->getString());
                }
                else if (show_password) {
                    show_password = false;
                    password_control->setSpriteFrame("hide_password");
                    field->setPasswordEnabled(true);
                    field->setString(field->getString());
                }
                break;
            }
            default:
                break;
            }
            });
    }
    button->ignoreContentAdaptWithSize(false);
    addChild(sprite, 0);
    addChild(field, 1);
    addChild(cursor_control_parent, 2);
    {
        f32 duration = 0.1F;
        auto hide = FadeTo::create(duration, 0);
        auto show = FadeTo::create(duration, 255);
        auto delay = DelayTime::create((1.0 - duration) / 2.0);
        auto seq = Sequence::create(hide, delay, show, delay, NULL);
        auto repeat = RepeatForever::create(seq);
        cursor_control_parent->setCascadeOpacityEnabled(true);
        cursor_control_parent->runAction(repeat);
    }
    cursor_control->setVisible(false);
    cursor_control_parent->addChild(cursor_control);
    addChild(button);
    hover_cv = ChangeValueBool();
    password_hover = ChangeValueBool();
}

void CustomUi::TextField::update(f32 dt) {
    HoverEffectGUI::update(dt);
}

bool CustomUi::TextField::hover(ax::Vec2 mouseLocationInView, Camera* cam)
{
    if (!adaptToWindowSize && field->getContentSize().width > sprite->getContentSize().width)
        field->_textFieldRenderer->setScale(sprite->getContentSize().width / (field->getContentSize().width + capinsets.origin.x * 2));
    else if (adaptToWindowSize)
        field->_textFieldRenderer->setScale(1);

    sprite->setContentSize(Size(extend ? Math::clamp(field->getContentSize().width + clampoffset.width, clampregion.origin.x, adaptToWindowSize ? (password ? Darkness::getInstance()->gameWindow.windowSize.width - (password_control->getContentSize().width * 2 + 30) : Darkness::getInstance()->gameWindow.windowSize.width) : (password ? clampregion.size.width - (password_control->getContentSize().width * 2 + 30) : clampregion.size.width)) : clampregion.size.width,
        Math::clamp(field->getContentSize().height + clampoffset.height, clampregion.origin.y, adaptToWindowSize ? Darkness::getInstance()->gameWindow.windowSize.height : clampregion.size.height)));
    button->setContentSize(sprite->getContentSize());

    if (password_control_button != _NOTHING)
        password_hover.setValue(password_control_button->hitTest(mouseLocationInView, cam, _NOTHING));

    if (password_hover.isChanged() && password_hover.getValue())
        SoundGlobals::playUiHoverSound();

    if (isEnabled())
    {
#if 1
        hover_cv.setValue(button->hitTest(mouseLocationInView, cam, _NOTHING));
#else
        hover->setValue(false);
        password_hover->setValue(false);
#endif

        if (_isFocused)
        {
            cursor_control->setVisible(true);
            if (field->getString().length() > 0) {
                cursor_control->setPosition(Vec2((field->getContentSize().width / 2 + 4) * field->_textFieldRenderer->getScale(), 0));
                cursor_control->setScale(field->_textFieldRenderer->getScale());
            }
            else {
                cursor_control->setPosition(Vec2(0, 0));
                cursor_control->setScale(1);
            }

            std::string  sString = TEXT(field->getString());
            std::size_t doubleSpace = sString.find("  ");
            while (doubleSpace != std::string::npos)
            {
                sString.erase(doubleSpace, 1);
                doubleSpace = sString.find("  ");
            }

            if (allowedChars.length() > 0) {
                std::string temp;
                temp.reserve(sString.length());
                for (size_t i = 0; i < sString.length(); i++)
                {
                    if (allowedChars.find(sString[i]) != std::string::npos)
                        temp += sString[i];
                }
                sString = temp;
            }

            if (toUpper)
                std::transform(sString.begin(), sString.end(), sString.begin(), ::toupper);

            field->setString(sString);
        }
        else cursor_control->setVisible(false);

        if (hover_cv.isChanged())
        {
            if (hover_cv.getValue())
            {
                if (!_isFocused) {
                    HoverEffectGUI::reset();
                }
            }
            else if (!_isFocused)
            {
            }
        }

        if (_isFocused || hover_cv.getValue())
        {
            auto dSize = getDynamicContentSize();
            dSize.x += 50;
            dSize.y += 40;
            HoverEffectGUI::hover(true, dSize);
        }
    }

    if (password) {
        password_control->setPosition(Vec2(getDynamicContentSize().width / 2 + password_control->getContentSize().width - 8, 0));
    }

    return hover_cv.getValue();
}

void CustomUi::TextField::focus()
{
    sprite->stopAllActions();
    auto fade = FadeTo::create(0.1f, 255);
    //auto scale = ScaleTo::create(0.1f, 1);
    //auto ease = EaseCubicActionIn::create(scale);
    //parent->runAction(ease);
    //sprite->runAction(fade);
    //auto tint = TintTo::create(0.1f, selected_color);
    //sprite->runAction(tint);
    field->attachWithIME();
    if (field->getString().length() > 0)
        cursor_control->setVisible(true);
    notifyFocused(_isFocused = true);
}

void CustomUi::TextField::defocus()
{
    auto tint = TintTo::create(0.1f, Color3B::WHITE);
    sprite->runAction(tint);
    field->detachWithIME();
    cursor_control->setVisible(false);
    notifyFocused(_isFocused = false);
}

void CustomUi::TextField::onEnable()
{
    auto fade = FadeTo::create(0.1f, 255);
    auto tint = TintTo::create(0.1f, Color3B::WHITE);
    sprite->runAction(tint);
    sprite->runAction(fade);
    field->runAction(tint);
}

void CustomUi::TextField::onDisable()
{
    defocus();
    auto fade = FadeTo::create(0.1f, 100);
    auto tint = TintTo::create(0.1f, Color3B::GRAY);
    sprite->runAction(fade);
    sprite->runAction(tint);
    field->runAction(tint);
}

bool CustomUi::TextField::click(ax::Vec2 mouseLocationInView, Camera* cam)
{
    if (!isEnabled())
        return false;
    if (button->hitTest(mouseLocationInView, cam, _NOTHING)) {
        focus();
        SoundGlobals::playUiHoverSound();
        return true;
    }
    if (!button->hitTest(mouseLocationInView, cam, _NOTHING))
        defocus();
    hover(mouseLocationInView, cam);
    return false;
}

Size CustomUi::TextField::getDynamicContentSize()
{
    return sprite->getContentSize();
}