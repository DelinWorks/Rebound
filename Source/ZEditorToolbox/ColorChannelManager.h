#pragma once

#include "Helper/short_types.h"
#include <2d/Node.h>
#include <Types.h>

#define COLOR_CHANNEL_SIZE 1000

namespace EditorToolbox {
    struct ColorChannel {
        ax::Color4F color = ax::Color4F::WHITE;
        ax::backend::BlendDescriptor blend;
        ax::Node* pCell = nullptr;
    };

    class ColorChannelManager {
    public:
        ColorChannelManager();
        ~ColorChannelManager();

        void nullifyCells();

        ColorChannel& getColor(U16 id);
        void setColor(U16 id, ColorChannel c);
        void updateCell(U16 id);

    private:
        ColorChannel* _colors;
    };
}
