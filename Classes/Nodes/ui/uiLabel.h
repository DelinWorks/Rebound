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

        void init(std::wstring _text, i32 _fontsize, Size _size = Size(0, 0), float _wrap = 0);

        void init(std::wstring& _text, std::string_view _fontname, i32 _fontsize, Size _size = Size(0, 0), float _wrap = 0);

        bool hasOutline = false;
        void enableOutline();

        void update(f32 dt) override;

        bool hover(cocos2d::Vec2 mouseLocationInView, Camera* cam) override;

        void focus();

        void defocus();

        void onEnable() override;

        void onDisable() override;

        bool press(cocos2d::Vec2 mouseLocationInView, Camera* cam);
        bool release(cocos2d::Vec2 mouseLocationInView, Camera* cam);

        Size getDynamicContentSize();

        void onFontScaleUpdate(float scale) override;

        void updatePositionAndSize();

        void notifyLayout() override;

        std::wstring text;
        float wrap = 0;
        ax::TextHAlignment hAlignment = ax::TextHAlignment::LEFT;
        ax::TextVAlignment vAlignment = ax::TextVAlignment::TOP;

        void setString(std::string _text);
        void setString(std::wstring _text);
    };
}
