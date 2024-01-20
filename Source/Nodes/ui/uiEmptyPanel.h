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
    using EnterCallback = std::function<void(Button* target, std::wstring)>;

    class EmptyPanel : public Modal {
    public:
        static CUI::EmptyPanel* create(BorderLayout border = BorderLayout::CENTER, BorderContext context = BorderContext::SCREEN_SPACE);

        ~EmptyPanel();

        void init(const std::wstring& header);

        bool hover(V2D mouseLocationInView, Camera* cam);
        bool press(V2D mouseLocationInView, Camera* cam);
        void keyPress(EventKeyboard::KeyCode keyCode);
        void keyRelease(EventKeyboard::KeyCode keyCode);

        void update(F32 dt) override;

        Size getDynamicContentSize();

        ButtonCallback discardCallback;

        CUI::Container* stack;
    };
}
