#pragma once

#include "axmol.h"
#include "uiContainer.h"
#include "uiHoverEffect.h"
#include "Helper/ShapingEngine.hpp"
#include "Components/UiRescaleComponent.h"

USING_NS_CC;

#define BUTTON_P1_CONTENT_SIZE Size(380, 40)
#define BUTTON_P1_CLAMP_REGION Rect(380, 40, visibleSize.width + getWinDiff().width, 40)
#define BUTTON_P1_CLAMP_OFFSET Size(30, 0)

namespace CUI
{
    class Button;
    using ButtonCallback = std::function<void(Button* target)>;

    class Button : public GUI {
    public:
        static CUI::Button* create();

        UiFontDescriptor desc;
        ax::Label* field;
        ax::ui::Scale9Sprite* icon;
        ui::Scale9Sprite* sprite;
        ui::Button* button;
        std::string normal_sp;
        Color3B selected_color;
        cocos2d::Size field_size;
        cocos2d::Rect capinsets;
        cocos2d::Rect clampregion;
        cocos2d::Size clampoffset;
        cocos2d::Size hitboxpadding;
        ChangeValue<bool> hover_cv;
        bool adaptToWindowSize = false;
        bool extend = false;
        V2D preCompContentSize;
        V2D curdSizeContentSize;
        bool runActionOnIcon = true;

        ButtonCallback _callback;

        ~Button();
        
        void init(std::wstring _text, int _fontSize, S2D _size = S2D::ZERO, S2D _hitboxPadding = BUTTON_HITBOX_CORNER_TOLERANCE);
        void initIcon(std::string _frameName, S2D _hitboxPadding = BUTTON_HITBOX_CORNER_TOLERANCE);
        void initIcon(std::string _frameName, Rect _capInsets, S2D _hitboxPadding = BUTTON_HITBOX_CORNER_TOLERANCE);

        void init(std::wstring _text, std::string_view _fontname, I32 _fontsize,
            R2D _capinsets, S2D _contentsize, R2D _clampregion,
            S2D _clampoffset, std::string_view _normal_sp, bool _adaptToWindowSize,
            Color3B _selected_color, bool _allowExtend, bool _isIcon, S2D _hitboxpadding);

        void update(F32 dt) override;

        bool hover(V2D mouseLocationInView, Camera* cam) override;

        void focus();

        void defocus();

        void onEnable() override;

        void onDisable() override;

        bool press(V2D mouseLocationInView, Camera* cam);
        bool release(V2D mouseLocationInView, Camera* cam);

        Size getDynamicContentSize();

        Size getFitContentSize();

        void enableIconHighlight(Color3B highlight = Color3B(0, 237, 255));
        void disableIconHighlight();

        float preCalculatedHeight();

        void onFontScaleUpdate(float scale) override;

        void updateInternalObjects() override;

        void setString(std::wstring text);
    };
}
