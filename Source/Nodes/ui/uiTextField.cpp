#include "uiTextField.h"
#include "2d/TweenFunction.h"
#include <regex>

CUI::TextField* CUI::TextField::create()
{
    CUI::TextField* ret = new CUI::TextField();
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

void CUI::TextField::init(const std::wstring& _placeholder, int _fontSize, Size _size, int maxLength, std::string_view allowedChars, std::string_view _fontname)
{
    init(
        _placeholder,
        _fontname,
        _fontSize,
        false,
        ADVANCEDUI_P1_CAP_INSETS,
        _size,
        Rect(_size.x, _size.y, _size.x, _size.y),
        TEXTFIELD_P1_CLAMP_OFFSET,
        ADVANCEDUI_TEXTURE,
        true,
        Color3B::WHITE,
        true,
        maxLength,
        false,
        allowedChars
    );
}

void CUI::TextField::init(const std::wstring& _placeholder, std::string_view _fontname, I32 _fontsize, bool _password,
    R2D _capinsets, ax::Size _contentsize, R2D _clampregion,
    S2D _clampoffset, std::string_view _normal_sp, bool _adaptToWindowSize,
    Color3B _selected_color, bool _allowExtend, I32 length, bool _toUpper,
    std::string_view _allowedChars)
{
    if (_rescalingAllowed)
        addComponent((new UiRescaleComponent(Director::getInstance()->getVisibleSize()))->enableDesignScaleIgnoring());
    setHoverOffset({ 10, 10 });
    desc.fontName = _fontname;
    desc.fontSize = _fontsize;
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
    field = ax::ui::TextField::create(Strings::narrow(_placeholder), _fontname, 0);
    field->setMaxLengthEnabled(true);
    field->setMaxLength(length == -1 ? 256 : length);
    field->setPasswordEnabled(_password);
    field->setEnabled(false);
    field->getTextFieldRenderer()->setVerticalAlignment(TextVAlignment::CENTER);
    field->getTextFieldRenderer()->setHorizontalAlignment(TextHAlignment::CENTER);
    sprite = ax::ui::Scale9Sprite::createWithSpriteFrameName(normal_sp, capinsets);
    sprite->setContentSize(_contentsize);
    sprite->setColor(selected_color);
    sprite->setProgramState(CUI::_backgroundShader);
    setContentSize(_contentsize);
    button = createPlaceholderButton();
    cursor_control = Sprite::createWithSpriteFrameName("text_cursor");
    cursor_control->setScaleY(_fontsize / TTFFS);
    if (password) {
        password_control = Sprite::createWithSpriteFrameName("hide_password");
        addChild(password_control, 2);
        password_control_button = createPlaceholderButton();
        //password_control_button->ignoreContentAdaptWithSize(false);
        //password_control_button->setContentSize(Size(password_control->getContentSize().width + 15, password_control->getContentSize().height + 15));
        //password_control->addChild(password_control_button);
        hookPlaceholderButtonToNode(password_control, password_control_button, Size(15, 10), true);
        //password_control_button->setPosition(V2D(password_control->getBoundingBox().size.width / 2, password_control->getBoundingBox().size.height / 2));
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
        F32 duration = 0.1F;
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
    _callback = [](TextField* target) {};
    _onFocusCallback = []() {};
    //hover_cv = ChangeValue<bool>();
    //password_hover = ChangeValue<bool>();
}

void CUI::TextField::update(F32 dt) {
    auto dSize = getDynamicContentSize();
    setContentSize(dSize + getUiPadding());
    if (sprite->isVisible())
        HoverEffectGUI::update(dt, getContentSize());
}

bool CUI::TextField::hover(V2D mouseLocationInView, Camera* cam)
{
    if (!adaptToWindowSize && field->getContentSize().width / _UiScale > sprite->getContentSize().width)
        field->getTextFieldRenderer()->setScale(sprite->getContentSize().width / (field->getContentSize().width / _UiScale + capinsets.origin.x * 2) / _UiScale);
    else if (adaptToWindowSize)
        field->getTextFieldRenderer()->setScale(1 / _UiScale);

    sprite->setContentSize(Size(extend ? Math::clamp(field->getContentSize().width / _UiScale + clampoffset.width, clampregion.origin.x, adaptToWindowSize ? (password ? Rebound::getInstance()->gameWindow.windowSize.width - (password_control->getContentSize().width * 2 + 30) :
        Rebound::getInstance()->gameWindow.windowSize.width) : (password ? clampregion.size.width - (password_control->getContentSize().width * 2 + 30) : clampregion.size.width)) : clampregion.size.width,
        Math::clamp((field->getContentSize().height - (_ForceOutline ? _PmtFontOutline * 2 * _UiScale : 0)) / _UiScale + clampoffset.height, clampregion.origin.y, adaptToWindowSize ? Rebound::getInstance()->gameWindow.windowSize.height : clampregion.size.height)));
    button->setContentSize(sprite->getContentSize());

    if (password_control_button != _NOTHING)
        password_hover.setValue(password_control_button->hitTest(mouseLocationInView, cam, _NOTHING));

    if (isEnabled())
    {
#if 1
        setUiHovered(button->hitTest(mouseLocationInView, cam, _NOTHING));
        hover_cv.setValue(isUiHovered());
        _pCurrentHoveredTooltipItem = isUiHovered() ? this : (_pCurrentHoveredTooltipItem == this ? nullptr : _pCurrentHoveredTooltipItem);
        if (hover_cv.isChanged() && hover_cv.getValue())
            SoundGlobals::playUiHoverSound();
#else
        hover->setValue(false);
        password_hover->setValue(false);
#endif

        if (isUiFocused())
        {
            cursor_control->setVisible(true);
            if (field->getString().length() > 0) {
                cursor_control->setPosition(V2D((field->getContentSize().width / 2 + 4) * field->getTextFieldRenderer()->getScale(), 0));
                //cursor_control->setScale(field->getTextFieldRenderer()->getScale() * _UiScale);
            }
            else {
                cursor_control->setPosition(V2D(0, 0));
                //cursor_control->setScale(1);
            }

            std::string sString = TEXT(field->getString());
            sString = std::regex_replace(sString, std::regex("[ ]{2,}"), " ");
            sString = std::regex_replace(sString, std::regex("[\n]"), "");
            /* REMOVE LEADING ZEROS */ if (remove_zeros && sString.length() > 1) {
                int i = 0;
                while (sString[i] == '0') i++;
                sString.erase(0, i);
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

            try {
                auto asString = ShapingEngine::Helper::widen(sString);
                if (!asString._Equal(cachedString)) {
                    cachedString = asString;
                    _callback(this);
                }
                field->setString(sString);
            }
            catch (std::exception& e) { field->setString(e.what()); };
        }
        else cursor_control->setVisible(false);
    }

    if (password) {
        password_control->setPosition(V2D(getDynamicContentSize().width / 2 + password_control->getContentSize().width - 8, 0));
    }

    return hover_cv.getValue();
}

void CUI::TextField::focus()
{
    if (!isUiFocused())
        _onFocusCallback();
    field->attachWithIME();
    if (isUiFocused()) return;
    field->setString(ShapingEngine::Helper::narrow(cachedString));
    sprite->stopAllActions();
    auto fade = FadeTo::create(0.1f, 255);
    if (field->getString().length() > 0)
        cursor_control->setVisible(true);
    SoundGlobals::playUiHoldSound();
    notifyFocused(this, true);
}

void CUI::TextField::defocus()
{
    if (!isUiFocused()) return;
    field->setString(ShapingEngine::render(cachedString, true));
    field->detachWithIME();
    cursor_control->setVisible(false);
    notifyFocused(this, false);
    SoundGlobals::playUiClickSound();
    HoverEffectGUI::hover();
}

void CUI::TextField::onEnable()
{
    auto fade = FadeTo::create(0.1f, 255);
    auto tint = TintTo::create(0.1f, Color3B::WHITE);
    sprite->runAction(fade);
    field->runAction(tint);
}

void CUI::TextField::onDisable()
{
    defocus();
    auto fade = FadeTo::create(0.1f, 100);
    auto tint = TintTo::create(0.1f, Color3B::GRAY);
    sprite->runAction(fade);
    field->runAction(tint);
}

bool CUI::TextField::press(V2D mouseLocationInView, Camera* cam)
{
    if (!isEnabled())
        return false;
    if (button->hitTest(mouseLocationInView, cam, _NOTHING)) {
        focus();
        return true;
    }
    else if (!button->hitTest(mouseLocationInView, cam, _NOTHING) && isUiFocused()) {
        defocus();
        return true;
    }
    hover(mouseLocationInView, cam);
    return false;
}

bool CUI::TextField::release(V2D mouseLocationInView, Camera* cam)
{
    return false;
}

void CUI::TextField::keyPress(EventKeyboard::KeyCode keyCode)
{
#ifdef WIN32 // clipboard implementation works with windows only.
    if (keyCode == EventKeyboard::KeyCode::KEY_LEFT_CTRL)
        _isLeftCtrlPressed = true;
    if (_isLeftCtrlPressed && (keyCode == EventKeyboard::KeyCode::KEY_X || keyCode == EventKeyboard::KeyCode::KEY_C)) {
        toClipboard(std::string(field->getString()));
        if (keyCode == EventKeyboard::KeyCode::KEY_X)
            field->setString("");
    }
    if (_isLeftCtrlPressed && keyCode == EventKeyboard::KeyCode::KEY_V) {
        field->setString(std::string(field->getString()) + fromClipboard());
    }
#endif
    if (ANY_ENTER(keyCode) || keyCode == EventKeyboard::KeyCode::KEY_ESCAPE)
        defocus();
}

void CUI::TextField::keyRelease(EventKeyboard::KeyCode keyCode)
{
    if (keyCode == EventKeyboard::KeyCode::KEY_LEFT_CTRL)
        _isLeftCtrlPressed = false;
}

Size CUI::TextField::getDynamicContentSize()
{
    return sprite->getContentSize();
}

void CUI::TextField::onFontScaleUpdate(float scale)
{
    //field->getTextFieldRenderer()->initWithTTF(field->getString(), desc.fontName, desc.fontSize * _PmtFontScale * scale);
    //field->getTextFieldRenderer()->getFontAtlas()->setAliasTexParameters();
    //if (_ForceOutline)
    //    field->getTextFieldRenderer()->enableOutline(Color4B(0, 0, 0, 255), _PmtFontOutline * _UiScale);
    field->getTextFieldRenderer()->setBMFontSize(UINT16_MAX * _BMFontScale * (desc.fontSize / 16));
    field->updateSizeAndPosition();
}

void CUI::TextField::setStyleDotted()
{
    sprite->setVisible(false);
    field->getTextFieldRenderer()->enableUnderline();
}

void CUI::TextField::setString(std::string text)
{
    cachedString = Strings::widen(text);
    field->setString(ShapingEngine::render(cachedString));
}

void CUI::TextField::setString(std::wstring text)
{
    cachedString = text;
    field->setString(ShapingEngine::render(cachedString));
}

CUI::TextField::~TextField()
{
    if (CUI::_pCurrentHeldItem == this)
        CUI::_pCurrentHeldItem = nullptr;
    if (CUI::_pCurrentHoveredTooltipItem == this)
        CUI::_pCurrentHoveredTooltipItem = nullptr;
    LOG_RELEASE;
}
