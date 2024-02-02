#pragma once

#include "axmol.h"
#include "custom_ui.h"
#include "uiContainer4Edge.h"
#include "uiButton.h"

namespace CUI
{
    struct TabGroup {
        Button* button;
        GUI* cont;
    };

    class Tabs;
    using TabsCallback = std::function<void(Tabs* target)>;

    class Tabs : public Container {
    public:
        Tabs(V2D _prefferedSize);

        ~Tabs();

        static CUI::Tabs* create(V2D _prefferedSize);

        void addElement(std::wstring e, GUI* container = nullptr);

        void update(F32 dt) override;

        virtual void mouseScroll(EventMouse* event) override;

        void setSelection(int idx = 0);

    private:
        EventPassClippingNode* clipping;
        Container* elementCont;
        Container* scrollCont;
        CUI::Button* rightB;
        CUI::Button* leftB;
        float vel = 0.0;
        V2D elemContPos;
        V2D ePos;
        I32 tabIndex;
        std::vector<TabGroup> tabIndices;

        void calculateContentBoundaries() override;
        void updateLayoutManagers(bool recursive = false) override;
    };
}
