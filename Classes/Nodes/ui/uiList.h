#pragma once

#include "axmol.h"
#include "custom_ui.h"
#include "uiContainer4Edge.h"
#include "uiButton.h"

namespace CUI
{
    class List : public Container {
    public:
        List(Vec2 _prefferedSize);

        ~List();

        static CUI::List* create(Vec2 _prefferedSize);

        void addElement(Container* container);

        void update(f32 dt) override;

        virtual void mouseScroll(EventMouse* event) override;

    private:
        EventPassClippingNode* clipping;
        Container* elementCont;
        Container4Edge* scrollCont;
        CUI::Button* scrollKnob;
        float dtScroll = 0.0f;
        CUI::Button* upB;
        CUI::Button* downB;
        float vel = 0.0;
        Vec2 elemContPos;
        Vec2 ePos;
        std::vector<Container*> elements;

        void calculateContentBoundaries() override;
        void updateLayoutManagers(bool recursive = false) override;
    };
}
