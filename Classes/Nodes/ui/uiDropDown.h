#pragma once

#include "axmol.h"
#include "uiContainer.h"
#include "uiHoverEffect.h"
#include "Helper/ShapingEngine.hpp"
#include "Components/UiRescaleComponent.h"
#include "uiList.h"
#include "uiButton.h"
#include "uiLabel.h"

USING_NS_CC;

#define BUTTON_P1_CONTENT_SIZE Size(380, 40)
#define BUTTON_P1_CLAMP_REGION Rect(380, 40, visibleSize.width + getWinDiff().width, 40)
#define BUTTON_P1_CLAMP_OFFSET Size(30, 0)

#define SLIDER_HITBOX_CORNER_TOLERANCE Size(12, 20)

namespace CUI
{
    class DropDown;
    using DropDownCallback = std::function<void(DropDown* target)>;

    class DropDown : public HoverEffectGUI {
    public:
        static CUI::DropDown* create();

        ~DropDown();

        Container* cont;
        ui::Button* button;
        Button* knob;
        Label* label;
        ChangeValue<bool> hover_cv;

        std::vector<std::wstring> _items;
        int selectedIndex = 0;
        DropDownCallback _callback;
        
        void init(std::vector<std::wstring>& _items, Size _contentsize = ax::Size::ZERO);

        void update(f32 dt) override;

        bool hover(cocos2d::Vec2 mouseLocationInView, Camera* cam) override;

        void focus();

        void defocus();

        void onEnable() override;

        void onDisable() override;

        bool isHeld = false;
        bool press(cocos2d::Vec2 mouseLocationInView, Camera* cam);
        bool release(cocos2d::Vec2 mouseLocationInView, Camera* cam);

        Size getDynamicContentSize();

        Size getFitContentSize();

        void showMenu(GUI* parent, BorderLayout b1, BorderLayout b2, Vec2 offset);

        void setSelection(int idx = 0);
    };
}