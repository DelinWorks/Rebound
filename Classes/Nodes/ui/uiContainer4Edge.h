#pragma once

#include "axmol.h"
#include "custom_ui.h"
#include "uiContainer.h"

namespace CUI
{
    class Container4Edge : public Container {
    public:
        Container4Edge();

        static CUI::Container4Edge* create(Vec2 _prefferedSize);

        void setChildTop(CUI::GUI* gui);
        void setChildBottom(CUI::GUI* gui);
        void setChildRight(CUI::GUI* gui);
        void setChildLeft(CUI::GUI* gui);

        void calculateContentBoundaries() override;
        void updateLayoutManagers(bool recursive = false) override;

    private:
        Container* top = nullptr;
        Container* right = nullptr;
        Container* left = nullptr;
        Container* bottom = nullptr;
    };
}
