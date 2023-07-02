#pragma once

#include "axmol.h"
#include "custom_ui.h"
#include "uiContainer4Edge.h"
#include "uiButton.h"

namespace CUI
{
    class List : public Container {
    public:
        List(Vec2 _prefferedSize, bool rescalingAllowed = true);

        ~List();

        static CUI::List* create(Vec2 _prefferedSize, bool rescalingAllowed = true);

        void addElement(Container* container, int extendCoeff = 0);

        void update(f32 dt) override;

        virtual void mouseScroll(EventMouse* event) override;

        bool hover(cocos2d::Vec2 mouseLocationInView, cocos2d::Camera* cam);
        bool press(cocos2d::Vec2 mouseLocationInView, cocos2d::Camera* cam);
        bool release(cocos2d::Vec2 mouseLocationInView, cocos2d::Camera* cam);

        void calculateContentBoundaries() override;
        void updateLayoutManagers(bool recursive = false) override;

        Vec2 prefferredListSize = ax::Vec2::ZERO;
        Vec2 ePos;
    private:
        EventPassClippingNode* clipping;
        Container* elementCont;
        Container* scrollCont;
        CUI::Button* scrollKnob;
        float dtScroll = 0.0f;
        CUI::Button* upB;
        CUI::Button* downB;
        float vel = 0.0;
        Vec2 elemContPos;
        float deltaScroll;
        float deltaScroll2;
        std::vector<Container*> elements;
    };
}
