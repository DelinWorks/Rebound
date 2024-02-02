#pragma once

#include "Helper/short_types.h"
#include "Helper/Math.h"
#include <Types.h>
#include "Nodes/TileMapSystem.h"
#include "Nodes/ui/include_ui.h"

USING_NS_AX;

namespace EditorToolbox
{
    struct HeterogeneousLayer
    {
        std::wstring name;
        TileSystem::Layer* layer = nullptr;
    };

    class LayerManager
    {
    public:
        std::vector<std::wstring> layers;
        U32 boundLayer = UINT32_MAX;
        CUI::Button* boundLayerBtn = nullptr;
        CUI::List* layersList = nullptr;
        TileSystem::Map* tileMap;
        void bindLayer(U32 index, bool asUserInput = false);
        void addListElement(std::wstring name);
        void addGeneralLayer(std::wstring name, bool bindIt = true);
        void renameGeneralLayer(U32 index, std::wstring newName);
        void removeLayer(U32 index);
        //void rotateLayer(U32 index, U32 newIndex);
        void moveLayer(U32 index, U32 newIndex);
        U32 getLayerCount();
        U32 getLayerIndex();
        std::function<void()> _updateStateCallback = [](){};
	};
}