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
        ChangeValueBool hover_cv;
        ChangeValueBool password_hover;
        bool adaptToWindowSize = false;
        bool extend = false;
        bool toUpper = false;
        str allowedChars = "";
        bool password = false;
        bool show_password = false;

        void init(std::string_view _placeholder, int _fontSize, Size _size, int maxLength = -1, std::string_view allowedChars = ""sv);

        void init(std::string_view _placeholder, std::string_view _fontname, i32 _fontsize, bool _password,
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

        bool click(cocos2d::Vec2 mouseLocationInView, Camera* cam);

        Size getDynamicContentSize();
    };
}

#endif
