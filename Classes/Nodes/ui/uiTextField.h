#ifndef __ADVANCED_UI_TEXTFIELD_H__
#define __ADVANCED_UI_TEXTFIELD_H__

#include "axmol.h"
#include "uiContainer.h"
#include "uiHoverEffect.h"
#include "Helper/ShapingEngine.hpp"
#include "Components/UiRescaleComponent.h"

USING_NS_CC;

#define TEXTFIELD_P1_CONTENT_SIZE Size(380, 40)
#define TEXTFIELD_P1_CLAMP_REGION Rect(380, 40, visibleSize.width + getWinDiff().width, 40)
#define TEXTFIELD_P1_CLAMP_OFFSET Size(30, 0)

namespace CUI
{
    class TextField;
    using TextFieldCallback = std::function<void(TextField* target)>;

    class TextField : public HoverEffectGUI {
    public:
        static CUI::TextField* create();

        ~TextField();

        UiFontDescriptor desc;
        ui::TextField* field;
        std::wstring cachedString;
        ui::Scale9Sprite* sprite;
        Sprite* password_control;
        Node* cursor_control_parent;
        Sprite* cursor_control;
        ui::Button* password_control_button;
        ui::Button* button;
        std::string normal_sp;
        Color3B selected_color;
        cocos2d::Rect capinsets;
        cocos2d::Rect clampregion;
        cocos2d::Size clampoffset;
        ChangeValue<bool> hover_cv;
        ChangeValue<bool> password_hover;
        bool adaptToWindowSize = false;
        bool extend = false;
        bool toUpper = false;
        std::string allowedChars = "";
        bool password = false;
        bool show_password = false;
        bool remove_zeros = true;

        TextFieldCallback _callback;

        void init(const std::wstring& _placeholder, int _fontSize, Size _size, int maxLength = -1, std::string_view allowedChars = ""sv);

        void init(const std::wstring& _placeholder, std::string_view _fontname, i32 _fontsize, bool _password,
            cocos2d::Rect _capinsets, cocos2d::Size _contentsize, cocos2d::Rect _clampregion,
            Size _clampoffset, std::string_view _normal_sp, bool _adaptToWindowSize,
            Color3B _selected_color, bool _allowExtend, i32 length, bool _toUpper,
            std::string_view _allowedChars);

        void update(f32 dt) override;

        bool hover(cocos2d::Vec2 mouseLocationInView, Camera* cam) override;

        void focus();

        void defocus();

        void onEnable() override;

        void onDisable() override;

        bool press(cocos2d::Vec2 mouseLocationInView, Camera* cam);
        bool release(cocos2d::Vec2 mouseLocationInView, Camera* cam);

        bool _isLeftCtrlPressed = false;
        void keyPress(EventKeyboard::KeyCode keyCode);
        void keyRelease(EventKeyboard::KeyCode keyCode);

        Size getDynamicContentSize();

        void onFontScaleUpdate(float scale) override;

        void setStyleDotted();

        void setString(std::string _text);
        void setString(std::wstring _text);
    };
}

#endif
