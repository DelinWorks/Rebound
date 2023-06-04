#pragma once

#include "axmol.h"
#include "uiContainer.h"
#include "uiHoverEffect.h"
#include "Helper/ShapingEngine.hpp"
#include "Components/UiRescaleComponent.h"

#define EXCLUDE_EXTENSIONS
#include "include_ui.h"

USING_NS_CC;

namespace CUI
{
    enum DiscardButtons : u8 {
        OKAY_ABORT = 0,
        YES_NO = 1,
        OKAY = 2,
        SUBMIT_CANCEL = 3
    };

    enum DiscardType : u8 {
        MESSAGE = 0,
        INPUT = 1
    };

    class DiscardPanel : public Modal {
    public:
        static CUI::DiscardPanel* create(BorderLayout border = BorderLayout::CENTER, BorderContext context = BorderContext::SCREEN_SPACE);

        void init(const std::wstring& header, const std::wstring& placeholder_or_text, DiscardButtons buttons = DiscardButtons::OKAY_ABORT, DiscardType type = DiscardType::INPUT);

        bool hover(cocos2d::Vec2 mouseLocationInView, cocos2d::Camera* cam);
        bool press(cocos2d::Vec2 mouseLocationInView, cocos2d::Camera* cam);
        void keyPress(EventKeyboard::KeyCode keyCode);
        void keyRelease(EventKeyboard::KeyCode keyCode);

        void update(f32 dt) override;

        Size getDynamicContentSize();

        DiscardButtons buttons;
        ButtonCallback discardCallback;
        Button* eventButton;

        CUI::Container* stack;
    };
}
