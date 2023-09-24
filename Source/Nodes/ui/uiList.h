#pragma once

#include "axmol.h"
#include "custom_ui.h"
#include "uiContainer4Edge.h"
#include "uiButton.h"
#include "uiLabel.h"

namespace CUI
{
    class List : public Container {
    public:
        List(V2D _prefferedSize, bool rescalingAllowed = true);

        ~List();

        static CUI::List* create(V2D _prefferedSize, bool rescalingAllowed = true);

        void addElement(Container* container, int extendCoeff = 0);

        void update(F32 dt) override;

        virtual void mouseScroll(EventMouse* event) override;

        bool hover(V2D mouseLocationInView, cocos2d::Camera* cam);
        bool press(V2D mouseLocationInView, cocos2d::Camera* cam);
        bool release(V2D mouseLocationInView, cocos2d::Camera* cam);

        void calculateContentBoundaries() override;
        void updateLayoutManagers(bool recursive = false) override;

        CUI::Label* setEmptyText(std::wstring _text);

        V2D prefferredListSize = V2D::ZERO;
        V2D ePos = V2D::ZERO;
    private:
        EventPassClippingNode* clipping;
        Container* elementCont;
        Container* scrollCont;
        CUI::Button* scrollKnob;
        float dtScroll = 0.0f;
        CUI::Button* upB;
        CUI::Button* downB;
        float vel = 0.0;
        V2D elemContPos = V2D::ZERO;
        float deltaScroll = 0.0;
        float deltaScroll2 = 0.0;
        std::vector<Container*> elements;
        bool isListDirty = false;
    };
}
