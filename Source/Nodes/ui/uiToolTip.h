#pragma once

#include "axmol.h"
#include "custom_ui.h"
#include "uiContainer.h"
#include "uiLabel.h"

namespace CUI
{
    class ToolTip : public Container {
    public:
        ToolTip();
        ~ToolTip();

        static CUI::ToolTip* create();

        void update(F32 dt);

        bool hover(V2D mouseLocationInView, cocos2d::Camera* cam);

        void showToolTip(std::wstring tooltip, float time_override = -1);
        void hideToolTip();

        CUI::Label* label;

        V2D position;
        float rotation = 0.0;
        V2D mousePos;
    };
}
