#pragma once

#include "axmol.h"
#include "uiContainer.h"
#include "uiHoverEffect.h"
#include "Helper/ShapingEngine.hpp"
#include "Components/UiRescaleComponent.h"
#include "uiButton.h"
#include "uiLabel.h"

USING_NS_CC;

#define BUTTON_P1_CONTENT_SIZE Size(380, 40)
#define BUTTON_P1_CLAMP_REGION Rect(380, 40, visibleSize.width + getWinDiff().width, 40)
#define BUTTON_P1_CLAMP_OFFSET Size(30, 0)

#define SLIDER_HITBOX_CORNER_TOLERANCE Size(12, 20)

namespace CUI
{
    class Toggle;
    using ToggleCallback = std::function<void(bool t, Toggle* target)>;

    class RadioGroup : public ax::Ref {
    public:
        RadioGroup();
        ~RadioGroup();
        void addChild(Toggle* t);
        void select(Toggle* t);
    private:
        std::vector<Toggle*> radios;
    };

    class Toggle : public HoverEffectGUI {
    public:
        static CUI::Toggle* create();

        ~Toggle();

        Container* cont;
        ui::Button* button;
        Button* knob;
        Label* label;
        ChangeValue<bool> hover_cv;

        bool isToggled = false;
        ToggleCallback _callback;
        RadioGroup* group = nullptr;
        
        void init(std::wstring _text, S2D _contentsize = S2D::ZERO);

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

        void toggle(bool state);
    };
}
