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
        Tabs(Vec2 _prefferedSize);

        static CUI::Tabs* create(Vec2 _prefferedSize);

        void addElement(std::wstring e, GUI* container = nullptr);

        void update(f32 dt) override;

        virtual void mouseScroll(EventMouse* event) override;

        void setSelection(int idx = 0);

    private:
        EventPassClippingNode* clipping;
        Container* elementCont;
        Container* scrollCont;
        CUI::Button* rightB;
        CUI::Button* leftB;
        float vel = 0.0;
        Vec2 elemContPos;
        Vec2 ePos;
        i32 tabIndex;
        std::vector<TabGroup> tabIndices;

        void calculateContentBoundaries() override;
        void updateLayoutManagers(bool recursive = false) override;
    };
}
