#ifndef __ADVANCED_UI_TEXTFIELD_H__
#define __ADVANCED_UI_TEXTFIELD_H__

#include "cocos2d.h"
#include "ui/CocosGUI.h"
#include "uiContainer.h"

USING_NS_CC;
USING_NS_CC::ui;

#define TEXTFIELD_P1_CONTENT_SIZE Size(380, 40)
#define TEXTFIELD_P1_CLAMP_REGION Rect(380, 40, visibleSize.width + getWinDiff().width, 40)
#define TEXTFIELD_P1_CLAMP_OFFSET Size(30, 0)

namespace CustomUi
{
    class TextField : public GUI {
    public:
        static CustomUi::TextField* create();

        ui::TextField* field;
        ui::Scale9Sprite* sprite;
        Sprite* sprite_hover;
        cocos2d::backend::ProgramState* sprite_hover_shader;
        float hover_animation_time;
        float hover_animation_step;
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
        ChangeValueBool hover;
        ChangeValueBool password_hover;
        bool adaptToWindowSize = false;
        bool extend = false;
        bool toUpper = false;
        str allowedChars = "";
        bool password = false;
        bool show_password = false;
        bool isFocused = false;

        void init(std::string _placeholder, std::string _fontname, i32 _fontsize,
            bool _password, cocos2d::Rect _capinsets, cocos2d::Size _contentsize,
            cocos2d::Rect _clampregion, Size _clampoffset, std::string _normal_sp,
            bool _adaptToWindowSize = false, Color3B _selected_color = Color3B(117, 179, 255),
            bool _allowExtend = true, i32 length = 128, bool _toUpper = false, str _allowedChars = "");

        bool update(cocos2d::Vec2 mouseLocationInView, Camera* cam);

        void focus();

        void defocus();

        void onEnable() override;

        void onDisable() override;

        bool click(cocos2d::Vec2 mouseLocationInView, Camera* cam);

        Size getDynamicContentSize();
    };
}

#endif
