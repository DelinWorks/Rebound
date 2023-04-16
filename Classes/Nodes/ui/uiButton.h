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

namespace CustomUi
{
    class Button : public HoverEffectGUI {
    public:
        static CustomUi::Button* create();
        UiFontDescriptor desc;
        ax::Label* field;
        ui::Scale9Sprite* sprite;
        ui::Button* button;
        std::string normal_sp;
        Color3B selected_color;
        cocos2d::Rect capinsets;
        cocos2d::Rect clampregion;
        cocos2d::Size clampoffset;
        ChangeValue<bool> hover_cv;
        bool adaptToWindowSize = false;
        bool extend = false;

        std::function<void(Button* target)> _callback;

        void init(std::wstring _text, int _fontSize, Size _size);

        void init(std::wstring _text, std::string_view _fontname, i32 _fontsize,
            cocos2d::Rect _capinsets, cocos2d::Size _contentsize, cocos2d::Rect _clampregion,
            Size _clampoffset, std::string_view _normal_sp, bool _adaptToWindowSize,
            Color3B _selected_color, bool _allowExtend);

        void update(f32 dt) override;

        bool hover(cocos2d::Vec2 mouseLocationInView, Camera* cam) override;

        void focus();

        void defocus();

        void onEnable() override;

        void onDisable() override;

        bool press(cocos2d::Vec2 mouseLocationInView, Camera* cam);
        bool release(cocos2d::Vec2 mouseLocationInView, Camera* cam);

        Size getDynamicContentSize();

        void onFontScaleUpdate(float scale) override;
    };
}
