#include "uiTextField.h"
#include "2d/CCTweenFunction.h"

inline backend::ProgramState* createGPUProgram(std::string_view resources_frag_shader_path = ""sv, std::string_view resources_vertex_shader_path = ""sv)
{
    auto fileUtiles = FileUtils::getInstance();
    auto fragmentFullPath = fileUtiles->fullPathForFilename(resources_frag_shader_path);
    auto fragSource = fileUtiles->getStringFromFile(fragmentFullPath);
    auto vertexFullPath = fileUtiles->fullPathForFilename(resources_vertex_shader_path);
    auto vertexSource = fileUtiles->getStringFromFile(vertexFullPath);
    auto program = backend::Device::getInstance()->newProgram(vertexSource.length() == 0 ? positionTextureColor_vert : vertexSource.c_str(), fragSource.length() == 0 ? positionTextureColor_frag : fragSource.c_str());
    auto theFuckingProgramState = new backend::ProgramState(program);
    return theFuckingProgramState;
}

#define SET_UNIFORM(ps, name, value)  do {   \
decltype(value) __v = value;                           \
auto __loc = (ps)->getUniformLocation(name);  \
(ps)->setUniform(__loc, &__v, sizeof(__v));  \
} while(false)

#define SET_TEXTURE(ps, name, idx, value)  do {   \
auto * __v = value;                           \
auto __loc = (ps)->getUniformLocation(name);  \
(ps)->setTexture(__loc, idx, __v);  \
} while(false) 

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
        false,
        Color3B(117, 179, 255),
        true,
        maxLength,
        false,
        allowedChars
    );
}

void CustomUi::TextField::init(std::string_view _placeholder, std::string_view _fontname, i32 _fontsize, bool _password,
    cocos2d::Rect _capinsets, cocos2d::Size _contentsize, cocos2d::Rect _clampregion,
    Size _clampoffset, std::string_view _normal_sp, bool _adaptToWindowSize,
    Color3B _selected_color, bool _allowExtend, i32 length, bool _toUpper,
    std::string_view _allowedChars)
{
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
    cursor_control_parent = cocos2d::Node::create();
    field = cocos2d::ui::TextField::create(_placeholder, _fontname, _fontsize);
    if (length != -1) {
        field->setMaxLengthEnabled(true);
        field->setMaxLength(length);
    }
    field->setPasswordEnabled(_password);
    field->setEnabled(false);
    field->_textFieldRenderer->setVerticalAlignment(TextVAlignment::CENTER);
    field->_textFieldRenderer->setHorizontalAlignment(TextHAlignment::CENTER);
    sprite = cocos2d::ui::Scale9Sprite::createWithSpriteFrameName(normal_sp, capinsets);
    sprite->setContentSize(_contentsize);
    button = createPlaceholderButton();
    button->setEnabled(false);
    sprite_hover = Sprite::create("shared/unready/ptrn_bg_hover2.png");
    sprite_hover->setOpacity(0);
    sprite_hover_shader = createGPUProgram("shared/unready/ui_hover_shader/hover.frag", "shared/unready/ui_hover_shader/hover.vert");
    sprite_hover->setProgramState(sprite_hover_shader);
    SET_TEXTURE(sprite_hover_shader, "u_tex1", 1, Director::getInstance()->getTextureCache()->addImage("shared/unready/ptrn_bg_hover_shader.png")->getBackendTexture());
    cursor_control = Sprite::createWithSpriteFrameName("text_cursor.png");
    if (password) {
        password_control = Sprite::createWithSpriteFrameName("hide_password.png");
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
                    password_control->setSpriteFrame("show_password.png");
                    field->setPasswordEnabled(false);
                    field->setString(field->getString());
                }
                else if (show_password) {
                    show_password = false;
                    password_control->setSpriteFrame("hide_password.png");
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
    addChild(sprite_hover, 0);
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
    hover = ChangeValueBool();
    password_hover = ChangeValueBool();
}

bool CustomUi::TextField::update(cocos2d::Vec2 mouseLocationInView, Camera* cam)
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
        hover.setValue(button->hitTest(mouseLocationInView, cam, _NOTHING));
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

            str sString = TEXT(field->getString());
            std::size_t doubleSpace = sString.find("  ");
            while (doubleSpace != std::string::npos)
            {
                sString.erase(doubleSpace, 1);
                doubleSpace = sString.find("  ");
            }

            if (allowedChars.length() > 0) {
                str temp;
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

        if (hover.isChanged())
        {
            if (hover.getValue())
            {
                if (!_isFocused) {
                    SoundGlobals::playUiHoverSound();
                    sprite_hover->stopAllActions();
                    sprite_hover->runAction(
                        Sequence::create(
                            CallFunc::create([this]() { sprite_hover->setVisible(true); }),
                            FadeTo::create(0.1f, 100),
                            _NOTHING
                        )
                    );
                    hover_animation_time = 0.0;
                    hover_animation_step = 0.0;
                    SET_UNIFORM(sprite_hover_shader, "u_val", (float)0.0);
                }
            }
            else if (!_isFocused)
            {
            }
        }

        if (_isFocused || hover.getValue())
        {
            auto dSize = getDynamicContentSize();
            dSize.x += 30;
            dSize.y += 20;
            sprite_hover->setContentSize(dSize);
        }

        if (hover_animation_time != -1)
        {
            if (hover_animation_time >= 0.25 && (!_isFocused && !hover.getValue()))
            {
                hover_animation_time = -1;
                sprite_hover->stopAllActions();
                sprite_hover->runAction(
                    Sequence::create(
                        FadeTo::create(0.1f, 0),
                        CallFunc::create([this]() { sprite_hover->setVisible(false); }),
                        _NOTHING
                    )
                );
            }
        }

        if (hover_animation_time != -1)
        {
            hover_animation_time += Director::getInstance()->getDeltaTime();
            hover_animation_time = hover_animation_time > 1.0 ? 1.0 : hover_animation_time;

            if (hover_animation_step > 1.0F / 30.0)
            {
                hover_animation_step = 0;
                SET_UNIFORM(sprite_hover_shader, "u_val", (float)hover_animation_time);
            }
            else hover_animation_step += Director::getInstance()->getDeltaTime();
        }
    }

    if (password) {
        password_control->setPosition(Vec2(getDynamicContentSize().width / 2 + password_control->getContentSize().width - 8, 0));
    }

    return hover.getValue();
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
    sprite_hover->setVisible(false);
}

bool CustomUi::TextField::click(cocos2d::Vec2 mouseLocationInView, Camera* cam)
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
    update(mouseLocationInView, cam);
    return false;
}

Size CustomUi::TextField::getDynamicContentSize()
{
    return sprite->getContentSize();
}
