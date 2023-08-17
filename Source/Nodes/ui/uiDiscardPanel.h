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
    enum DiscardButtons : U8 {
        OKAY_ABORT = 0,
        YES_NO = 1,
        OKAY = 2,
        SUBMIT_CANCEL = 3
    };

    enum DiscardType : U8 {
        MESSAGE = 0,
        INPUT = 1
    };

    class DiscardPanel : public Modal {
    public:
        static CUI::DiscardPanel* create(BorderLayout border = BorderLayout::CENTER, BorderContext context = BorderContext::SCREEN_SPACE);

        ~DiscardPanel();

        void init(const std::wstring& header, const std::wstring& placeholder_or_text, DiscardButtons buttons = DiscardButtons::OKAY_ABORT, DiscardType type = DiscardType::INPUT);

        bool hover(V2D mouseLocationInView, Camera* cam);
        bool press(V2D mouseLocationInView, Camera* cam);
        void keyPress(EventKeyboard::KeyCode keyCode);
        void keyRelease(EventKeyboard::KeyCode keyCode);

        void update(F32 dt) override;

        Size getDynamicContentSize();

        DiscardButtons buttons;
        ButtonCallback discardCallback;
        Button* eventButton;

        CUI::Container* stack;
    };
}
