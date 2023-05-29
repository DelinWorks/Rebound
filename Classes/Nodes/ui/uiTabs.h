#pragma once

#include "axmol.h"
#include "custom_ui.h"
#include "uiContainer4Edge.h"
#include "uiButton.h"

namespace CustomUi
{
    class Tabs : public Container4Edge {
    public:
        Tabs(Vec2 _prefferedSize);

        static CustomUi::Tabs* create(Vec2 _prefferedSize);

        void addElement(std::wstring e);

    private:
        Container* elementCont;
        Container* scrollCont;
        Vec2 prefferedSize;
        bool isScaled = false;

        void calculateContentBoundaries() override;
        void updateLayoutManagers(bool recursive = false) override;
    };
}
