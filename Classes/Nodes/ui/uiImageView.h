#pragma once

#include "axmol.h"
#include "uiContainer.h"
#include "uiHoverEffect.h"
#include "Helper/ShapingEngine.hpp"
#include "Components/UiRescaleComponent.h"

USING_NS_CC;

namespace CUI
{
    class ImageView : public GUI {
    public:
        static CUI::ImageView* create(ax::Size _contentsize, ax::Texture2D* texture);

        ~ImageView();

        ui::Button* button;
        ui::Button* auxButton;
        ax::ClippingRectangleNode* scissor;
        ax::Node* imageP;
        ax::Sprite* image;
        ax::Sprite* bg;
        ax::Size textureSize;
        ax::Vec2 mousePos;
        ax::DrawNode* grid;
        ax::DrawNode* selection;
        ax::Size gridSize;
        ax::Vec2 pressLocation;
        int selectedIndex = 0;
        
        bool init(ax::Size _contentsize = ax::Size::ZERO, ax::Texture2D* texture = nullptr);

        void enableGridSelection(ax::Size _gridsize);

        bool hover(cocos2d::Vec2 mouseLocationInView, Camera* cam) override;
        bool press(cocos2d::Vec2 mouseLocationInView, Camera* cam);
        bool release(cocos2d::Vec2 mouseLocationInView, Camera* cam);

        void mouseScroll(ax::EventMouse* event);

        void onEnable() override;

        void onDisable() override;
    };
}
