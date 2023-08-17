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

#define SLIDER_HITBOX_CORNER_TOLERANCE Size(12, 20)

namespace CUI
{
    class Slider;
    using SliderCallback = std::function<void(float p, Slider* target)>;

    class Slider : public HoverEffectGUI {
    public:
        static CUI::Slider* create();

        ~Slider();

        ui::Button* button;
        ui::Slider* slider;
        ui::Scale9Sprite* sprite;
        S2D bar_size;
        R2D capinsets;
        ChangeValue<bool> hover_cv;

        float currentValue;
        std::function<void()> _onClickCallback;
        SliderCallback _callback;
        
        void setValue(float v, bool call = true);

        void init(Size _contentsize = ax::Size::ZERO);

        void init(std::string_view barFrame, std::string_view progressFrame, std::string_view knob, ax::Rect _capinsets, S2D _contentsize);

        void update(F32 dt) override;

        bool hover(V2D mouseLocationInView, Camera* cam) override;

        void focus();

        void defocus();

        void onEnable() override;

        void onDisable() override;

        bool isHeld = false;
        bool press(V2D mouseLocationInView, Camera* cam);
        bool release(V2D mouseLocationInView, Camera* cam);

        Size getDynamicContentSize();

        Size getFitContentSize();
    };
}
