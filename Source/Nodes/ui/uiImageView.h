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
        static CUI::ImageView* create(S2D _contentsize, ax::Texture2D* texture, bool rescalingAllowed = true);

        ~ImageView();

        ui::Button* button;
        ui::Button* auxButton;
        ax::ClippingRectangleNode* scissor;
        ax::Node* imageP;
        ax::Sprite* image;
        ax::Sprite* bg;
        S2D textureSize;
        V2D mousePos;
        ax::DrawNode* grid;
        ax::DrawNode* selection;
        S2D gridSize;
        V2D pressLocation;
        float zoom;
        int selectedIndex = 0;

        ChangeValue<bool> hover_cv;
        
        bool init(S2D _contentsize = S2D::ZERO, ax::Texture2D* texture = nullptr, bool rescalingAllowed = true);

        void enableGridSelection(S2D _gridsize);

        bool hover(V2D mouseLocationInView, Camera* cam) override;
        bool press(V2D mouseLocationInView, Camera* cam);
        bool release(V2D mouseLocationInView, Camera* cam);

        void mouseScroll(ax::EventMouse* event);

        void onEnable() override;

        void onDisable() override;
    };
}
