#pragma once

#include "axmol.h"
#include "uiContainer.h"
#include "uiHoverEffect.h"
#include "Helper/ShapingEngine.hpp"
#include "Components/UiRescaleComponent.h"

#define EXCLUDE_EXTENSIONS
#include "include_ui.h"

USING_NS_CC;

namespace CustomUi
{
    class DiscardPanel : public Container {
    public:
        static CustomUi::DiscardPanel* create(BorderLayout border = BorderLayout::CENTER, BorderContext context = BorderContext::SCREEN_SPACE);

        void init(const std::wstring& header, const std::wstring& placeholder);

        bool hover(cocos2d::Vec2 mouseLocationInView, cocos2d::Camera* cam);
        bool press(cocos2d::Vec2 mouseLocationInView, cocos2d::Camera* cam);

        void update(f32 dt) override;

        Size getDynamicContentSize();
    };
}
