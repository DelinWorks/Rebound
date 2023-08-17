#pragma once

#include "axmol.h"
#include "uiContainer.h"
#include "uiHoverEffect.h"
#include "Helper/ShapingEngine.hpp"
#include "Components/UiRescaleComponent.h"
#include "uiSlider.h"
#include "ZEditorToolbox/ColorChannelManager.h"

USING_NS_CC;

namespace CUI
{
    class HSVWheel;
    using HSVWheelCallback = std::function<void(const ax::HSV& hsv, HSVWheel* target)>;

    class HSVWheel : public GUI {
    public:
        static CUI::HSVWheel* create(float scale = 1.0);

        ~HSVWheel();

        ui::Button* button;
        ui::Button* resetButton;

        ax::Node* lp;
        ui::Button* wButton;
        ui::Button* sqButton;
        ax::Sprite* wheel;
        ax::Sprite* wheelHandle;
        ax::Sprite* square;
        ax::Sprite* squareHandle;

        ax::Sprite* oldColor;
        ax::Sprite* newColor;

        ax::HSV hsv;
        ax::HSV sqHsv;

        Slider* opacity;

        std::function<void()> _onClickCallback;
        HSVWheelCallback _callback;

        bool isPickingWheel = false;
        bool isPickingSquare = false;

        bool doNotPushStateOnce = false;
        EditorToolbox::ColorChannelManager* channelMgr;
        U16 currentChannel;

        bool init(float scale);

        void update(F32 dt) override;

        void updateColorValues();
        void updateColorValues(Color4F color, bool _doNotPushStateOnce = false);

        bool hover(V2D mouseLocationInView, Camera* cam) override;
        bool press(V2D mouseLocationInView, Camera* cam);
        bool release(V2D mouseLocationInView, Camera* cam);
    };
}
