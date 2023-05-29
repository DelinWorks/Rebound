#pragma once

#include "axmol.h"
#include "custom_ui.h"
#include "uiContainer.h"

namespace CustomUi
{
    class Container4Edge : public Container {
    public:
        Container4Edge();

        static CustomUi::Container4Edge* create(Vec2 _prefferedSize);

        void setChildTop(CustomUi::GUI* gui);
        void setChildBottom(CustomUi::GUI* gui);
        void setChildRight(CustomUi::GUI* gui);
        void setChildLeft(CustomUi::GUI* gui);

    private:
        Vec2 prefferedSize;
        Container* top = nullptr;
        Container* right = nullptr;
        Container* left = nullptr;
        Container* bottom = nullptr;

        void calculateContentBoundaries() override;
        void updateLayoutManagers(bool recursive = false) override;
    };
}
