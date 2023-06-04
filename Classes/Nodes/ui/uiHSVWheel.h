#pragma once

#include "axmol.h"
#include "uiContainer.h"
#include "uiHoverEffect.h"
#include "Helper/ShapingEngine.hpp"
#include "Components/UiRescaleComponent.h"
#include "uiSlider.h"

USING_NS_CC;

namespace CUI
{
    class HSVWheel;
    using HSVWheelCallback = std::function<void(const ax::HSV& hsv, HSVWheel* target)>;

    class HSVWheel : public GUI {
    public:
        static CUI::HSVWheel* create(float scale = 1.0);

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

        HSVWheelCallback _callback;

        bool isPickingWheel = false;
        bool isPickingSquare = false;

        bool init(float scale);

        void update(f32 dt) override;

        void updateColorValues();
        void updateColorValues(Color4F color);

        bool hover(cocos2d::Vec2 mouseLocationInView, Camera* cam) override;
        bool press(cocos2d::Vec2 mouseLocationInView, Camera* cam);
        bool release(cocos2d::Vec2 mouseLocationInView, Camera* cam);
    };
}
