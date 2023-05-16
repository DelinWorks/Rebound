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

#define BUTTON_HITBOX_CORNER_TOLERANCE Size(3, 3)

namespace CustomUi
{
    class Slider;
    using SliderCallback = std::function<void(Slider* target)>;

    class Slider : public HoverEffectGUI {
    public:
        static CustomUi::Slider* create();

        ui::Button* button;
        ui::Slider* slider;
        ui::Scale9Sprite* sprite;
        cocos2d::Size bar_size;
        cocos2d::Rect capinsets;
        ChangeValue<bool> hover_cv;

        SliderCallback _callback;
        
        void init(Size _contentsize = ax::Size::ZERO);

        void init(std::string_view barFrame, std::string_view progressFrame, std::string_view knob, ax::Rect _capinsets, cocos2d::Size _contentsize);

        void update(f32 dt) override;

        bool hover(cocos2d::Vec2 mouseLocationInView, Camera* cam) override;

        void focus();

        void defocus();

        void onEnable() override;

        void onDisable() override;

        Size getDynamicContentSize();
    };
}
