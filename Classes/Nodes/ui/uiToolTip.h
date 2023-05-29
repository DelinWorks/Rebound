#pragma once

#include "axmol.h"
#include "custom_ui.h"
#include "uiContainer.h"
#include "uiLabel.h"

namespace CustomUi
{
    class ToolTip : public Container {
    public:
        ToolTip();

        static CustomUi::ToolTip* create();

        void update(f32 dt);

        bool hover(cocos2d::Vec2 mouseLocationInView, cocos2d::Camera* cam);

        void showToolTip(std::wstring tooltip, float time_override = -1);

        CustomUi::Label* label;

        Vec2 position;
        float rotation = 0.0;
        Vec2 mousePos;
    };
}
