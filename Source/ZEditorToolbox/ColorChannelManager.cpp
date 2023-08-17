#include "ColorChannelManager.h"

EditorToolbox::ColorChannelManager::ColorChannelManager()
{
    _colors = new ColorChannel[COLOR_CHANNEL_SIZE];
}

EditorToolbox::ColorChannelManager::~ColorChannelManager()
{
    delete[] _colors;
}

void EditorToolbox::ColorChannelManager::nullifyCells()
{
    for (int i = 0; i < COLOR_CHANNEL_SIZE; i++)
        _colors[i].pCell = nullptr;
}

EditorToolbox::ColorChannel& EditorToolbox::ColorChannelManager::getColor(U16 id)
{
    return _colors[id];
}

void EditorToolbox::ColorChannelManager::setColor(U16 id, ColorChannel c)
{
    _colors[id].color = c.color;
    _colors[id].blend = c.blend;
}

void EditorToolbox::ColorChannelManager::updateCell(U16 id)
{
    auto it = _colors[id];
    if (it.pCell) {
        auto n = it.pCell;
        n->setColor(ax::Color3B(it.color));
        n->setOpacity(it.color.a * 255);
    }
}
