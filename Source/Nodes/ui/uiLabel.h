#pragma once

#include "axmol.h"
#include "uiContainer.h"
#include "uiHoverEffect.h"
#include "Helper/ShapingEngine.hpp"
#include "Components/UiRescaleComponent.h"

USING_NS_CC;

namespace CUI
{
    class Label : public GUI {
    public:
        static CUI::Label* create();

        ~Label();

        UiFontDescriptor desc;
        ax::Label* field;
        ax::Size size;

        void init(std::wstring _text, I32 _fontsize, Size _size = Size(0, 0), float _wrap = 0);

        void init(std::wstring& _text, std::string_view _fontname, I32 _fontsize, Size _size = Size(0, 0), float _wrap = 0);

        bool hasOutline = false;
        void enableOutline();

        void update(F32 dt) override;

        bool hover(V2D mouseLocationInView, Camera* cam) override;

        void focus();

        void defocus();

        void onEnable() override;

        void onDisable() override;

        bool press(V2D mouseLocationInView, Camera* cam);
        bool release(V2D mouseLocationInView, Camera* cam);

        Size getDynamicContentSize();

        void onFontScaleUpdate(float scale) override;

        void updatePositionAndSize();

        void notifyLayout() override;

        std::wstring text;
        float wrap = 0;
        ax::TextHAlignment hAlignment = ax::TextHAlignment::LEFT;
        ax::TextVAlignment vAlignment = ax::TextVAlignment::TOP;
        Color3B color = Color3B::WHITE;

        void setString(std::string _text);
        void setString(std::wstring _text);

        void updateInternalObjects() override;
    };
}
