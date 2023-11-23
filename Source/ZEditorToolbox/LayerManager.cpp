#include "LayerManager.h"
using namespace GameUtils;

void EditorToolbox::LayerManager::bindLayer(U32 index, bool asUserInput)
{
    if (index >= layers.size()) return;

    if (asUserInput)
    {
        CUI::Button* widget = dynamic_cast<CUI::Button*>(
            findNodesByTag(layersList->getElements()[index], CUI::EditorLayerWidget::kLayerNameButton)[0]);

        widget->_callback(widget);
    }

    boundLayer = index;
    tileMap->bindLayer(index);

    _updateStateCallback();
}

void EditorToolbox::LayerManager::addListElement(std::wstring name)
{
    layersList->addElement(CUI::Functions::createLayerWidget(name, [&](CUI::Button* target)
        {
            if (boundLayerBtn == target) return;
            target->enableIconHighlight();
            if (boundLayerBtn) boundLayerBtn->disableIconHighlight();
            boundLayerBtn = target;
            auto str = std::string(target->field->getString());

            auto findIndex = [](const std::vector<CUI::Container*>& vec, Node* ptr) -> U32 {
                auto it = std::find(vec.begin(), vec.end(), ptr);
                return U32((it != vec.end()) ? std::distance(vec.begin(), it) : -1);
            };

            bindLayer(findIndex(layersList->getElements(), target->getParent()->getParent()));
        }));
}

void EditorToolbox::LayerManager::addGeneralLayer(std::wstring name, bool bindIt)
{
    std::wstring layerName = Strings::alt_duplicate<std::wstring>(layers, name);

    layers.push_back(layerName);
    addListElement(layerName);
    tileMap->addLayer(Strings::narrow(layerName));

    if (bindIt)
    {
        bindLayer(layers.size() - 1, true);
        layersList->scrollToIndex(layers.size() - 1);
    }

    _updateStateCallback();
}

void EditorToolbox::LayerManager::renameGeneralLayer(U32 index, std::wstring name)
{
    if (index != UINT32_MAX && layers[index] != name)
    {
        std::wstring layerName = Strings::alt_duplicate<std::wstring>(layers, name);

        CUI::Container* container = dynamic_cast<CUI::Container*>(
            findNodesByTag(layersList->getElements()[index], CUI::EditorLayerWidget::kLayerMainContainer)[0]);

        CUI::Button* widget = dynamic_cast<CUI::Button*>(
            findNodesByTag(layersList->getElements()[index], CUI::EditorLayerWidget::kLayerNameButton)[0]);

        widget->setString(layerName);
        widget->updateInternalObjects();
        container->setContentSize(V2D(0, widget->preCalculatedHeight() + container->getMargin().y), true);
        layers[index] = layerName;
        tileMap->_layers[index]->_layerName = Strings::narrow(layerName);

        layersList->scrollToIndex(index);
        _updateStateCallback();
    }
}

void EditorToolbox::LayerManager::removeLayer(U32 index)
{
    if (index < layers.size())
    {
        layers.erase(layers.begin() + index);
        layersList->removeElement(index);
        tileMap->removeLayer(index);
        boundLayerBtn = nullptr;
        boundLayer = UINT32_MAX;

        _updateStateCallback();
    }
}

//void EditorToolbox::LayerManager::rotateLayer(U32 index, U32 newIndex)
//{
//    if (index < newIndex)
//    {
//        for (int i = index; i < newIndex + 1; ++i)
//        {
//            moveLayer(i, i + 1);
//        }
//    }
//    else if (index > newIndex)
//    {
//        int newIndexS = newIndex;
//        for (; index > newIndex; --index)
//            moveLayer(MAX(0, index-1), index);
//    }
//}

void EditorToolbox::LayerManager::moveLayer(U32 index, U32 newIndex)
{
    if (index < layers.size() && newIndex < layers.size())
    {
        if (index != newIndex)
        {
            moveElement<std::wstring>(layers, index, newIndex);
            moveElement<TileSystem::Layer*>(tileMap->_layers, index, newIndex);
            layersList->moveElement(index, newIndex);
            boundLayer = newIndex;
        }
        bindLayer(newIndex, true);

        _updateStateCallback();
    }
}

U32 EditorToolbox::LayerManager::getLayerCount()
{
    return layers.size();
}

U32 EditorToolbox::LayerManager::getLayerIndex()
{
	return boundLayer;
}
