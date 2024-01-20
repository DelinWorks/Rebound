#include "MapEditor.h"

USING_NS_CC;
using namespace backend;

using namespace GameUtils;

using namespace CUI;

void MapEditor::buildEntireUi()
{
    GameUtils::updateIgnoreDesignScale();

    CUI::callbackAccess.clear();

    auto container = _input->_uiContainer = CUI::Container::create();
    container->setStatic();
    container->setBorderLayoutAnchor();
    uiNode->addChild(container);
    CUI::callbackAccess.emplace("main", container);

    //container->addChild(CUI::Functions::createFledgedHSVPanel());

    //auto test4edge1 = CUI::Container::create();
    //auto test4edge2 = CUI::Container::create();
    //test4edge1->setStatic(); test4edge1->setContentSize({ 50, 50 });
    //test4edge2->setStatic(); test4edge2->setContentSize({ 50, 50 });

    //auto container4edge = CUI::Container4Edge::create({0, 200});
    //container->addChild(container4edge);
    //container4edge->setChildRight(test4edge1);
    //container4edge->setChildLeft(test4edge2);

    //auto tabs = CUI::Tabs::create({ 300, 20 });
    //tabs->setBackgroundSpriteCramped2({ 10, 2 });
    //container->addChild(tabs);
    //BENCHMARK_SECTION_BEGIN("create ui");
    //for (int i = 0; i < 30; i++)
    //    tabs->addElement(Strings::widen(Strings::gen_random(RandomHelper::random_int<int>(4, 8))));
    //BENCHMARK_SECTION_END();

    auto topRightContainer = CUI::Container::create();
    topRightContainer->setBorderLayout(BorderLayout::TOP_RIGHT, BorderContext::PARENT, true);
    topRightContainer->setBorderLayoutAnchor(BorderLayout::TOP_RIGHT);
    auto flow = CUI::FlowLayout(CUI::SORT_VERTICAL, CUI::STACK_CENTER, 20, 0, true);
    topRightContainer->setBackgroundBlocking();
    topRightContainer->setLayout(flow);
    topRightContainer->setBackgroundSpriteCramped({ 3,13 }, { 1, -1 });
    container->addChild(topRightContainer);

    auto cont2 = CUI::Container::create();
    cont2->setLayout(CUI::FlowLayout(CUI::SORT_HORIZONTAL, CUI::STACK_CENTER, 10, 0, false));
    cont2->DenyRescaling();
    auto labelOpacity = CUI::Label::create();
    labelOpacity->DenyRescaling();
    labelOpacity->init(L"Opacity: ", TTFFS);
    cont2->addChild(labelOpacity);
    auto layerOpacity = CUI::Slider::create();
    layerOpacity->DenyRescaling();
    layerOpacity->init(V2D(90, 10));
    layerOpacity->scheduleUpdate();
    cont2->addChild(layerOpacity);
    auto textFieldOpacity = CUI::TextField::create();
    textFieldOpacity->DenyRescaling();
    textFieldOpacity->init(L" A ", TTFFS, V2D{66, 0}, 3, "0123456789");
    textFieldOpacity->setStyleDotted();
    cont2->addChild(textFieldOpacity);
    topRightContainer->addChild(cont2);

    layerOpacity->_callback = [=](float p, CUI::Slider* target)
        {
            int alpha = p * 255;

            textFieldOpacity->setString(std::to_wstring(alpha));
        };

    textFieldOpacity->_callback = [=](CUI::TextField* target)
        {
            layerOpacity->setValue(std::stoi(target->cachedString) / 255.0);
        };

    _layerManager.layersList = CUI::List::create({ 400, 300 }, false);
    _layerManager.layersList->setEmptyText(L"No layers present.\n\ncreate layers to add\ntiles & objcets.")
        ->field->setLineSpacing(10);
    topRightContainer->addChild(_layerManager.layersList);

    cont2 = CUI::Container::create();
    //cont2->setBorderLayoutAnchor(BorderLayout::RIGHT);
    cont2->DenyRescaling();
    cont2->setLayout(FlowLayout(SORT_HORIZONTAL, STACK_CENTER, 25, 0, false));
    //cont2->setConstraint(ContentSizeConstraint(topRightContainer, V2D::ZERO, false, true, false));

    auto btnt2 = createLayerBtn = CUI::Button::create();
    btnt2->DenyRescaling();
    btnt2->disableArtMul();
    btnt2->setUiPadding(V2D(10, 0));
    btnt2->initIcon("editor_new_layer");
    btnt2->hoverTooltip = L"Creates a new empty layer with a name. The layer can be used for both Tilemaps & Objects.\nThe position at which the layer is in determines rendering order, Top will render front.\nYou can modify whether a layer is visible or interactable or both.\nYou can access advanced options for a specific layer using the wrench icon.";
    cont2->addChild(btnt2);

    btnt2->_callback = [&](CUI::Button* target)
    {
        auto cont = getContainer();
        if (cont)
        {
            auto dis = CUI::DiscardPanel::create(CENTER, PARENT);
            dis->init(L"> Create a New Layer <", L"Layer Name", CUI::SUBMIT_CANCEL);
            cont->pushModal(dis);
            dis->enterCallback = [&](CUI::Button*, std::wstring name)
                {
                    _layerManager.addGeneralLayer(name);

                    int idx = _layerManager.getLayerCount() - 1;

                    editorPushUndoState();
                    auto& undo = editorTopUndoStateOrDefault();
                    undo.setAction(EditorToolbox::UNDOREDO_LAYER_MODIFY);

                    undo.affectedLayers.action = EditorToolbox::UNDOREDO_LAYER_ADD;
                    undo.affectedLayers.manager = &_layerManager;

                    undo.affectedLayers.prev_layer_idx = idx;

                    undo.affectedLayers.layer.name = _layerManager.layers[idx];
                    undo.affectedLayers.layer.layer = map->_layers[idx];
                    AX_SAFE_RETAIN(undo.affectedLayers.layer.layer);
                };
        }
    };

    btnt2 = moveLayerUpBtn = CUI::Button::create();
    btnt2->DenyRescaling();
    btnt2->disableArtMul();
    btnt2->initIcon("editor_arrow_up_2");
    btnt2->hoverTooltip = L"Moves the selected layer UP one level.\nHold to move the layer to the top.\nYou can set the order index in advanced menu too.";
    cont2->addChild(btnt2);

    btnt2->_callback = [&](CUI::Button* target)
        {
            int idx = _layerManager.boundLayer;

            editorPushUndoState();
            auto& undo = editorTopUndoStateOrDefault();
            undo.setAction(EditorToolbox::UNDOREDO_LAYER_MODIFY);

            undo.affectedLayers.action = EditorToolbox::UNDOREDO_LAYER_MOVE;
            undo.affectedLayers.manager = &_layerManager;

            undo.affectedLayers.prev_layer_idx = idx;
            undo.affectedLayers.next_layer_idx = idx - 1;

            undo.affectedLayers.layer.layer = map->_layers[idx];

            _layerManager.moveLayer(_layerManager.boundLayer, _layerManager.boundLayer - 1);
        };

    btnt2 = moveLayerDownBtn = CUI::Button::create();
    btnt2->DenyRescaling();
    btnt2->disableArtMul();
    btnt2->initIcon("editor_arrow_down_2");
    btnt2->hoverTooltip = L"Moves the selected layer DOWN one level.\nHold to move the layer to the bottom.\nYou can set the order index in advanced menu too.";
    cont2->addChild(btnt2);

    btnt2->_callback = [&](CUI::Button* target)
        {
            int idx = _layerManager.boundLayer;

            editorPushUndoState();
            auto& undo = editorTopUndoStateOrDefault();
            undo.setAction(EditorToolbox::UNDOREDO_LAYER_MODIFY);

            undo.affectedLayers.action = EditorToolbox::UNDOREDO_LAYER_MOVE;
            undo.affectedLayers.manager = &_layerManager;

            undo.affectedLayers.prev_layer_idx = idx;
            undo.affectedLayers.next_layer_idx = idx + 1;

            undo.affectedLayers.layer.layer = map->_layers[idx];

            _layerManager.moveLayer(_layerManager.boundLayer, _layerManager.boundLayer + 1);
        };

    btnt2 = renameLayerBtn = CUI::Button::create();
    btnt2->DenyRescaling();
    btnt2->disableArtMul();
    btnt2->setUiPadding(V2D(10, 0));
    btnt2->initIcon("editor_rename");
    btnt2->hoverTooltip = L"Renames the selected layer\nYou can set the name in advanced menu too.";
    cont2->addChild(btnt2);

    btnt2->_callback = [&](CUI::Button* target)
        {
            if (_layerManager.boundLayer != UINT32_MAX)
            {
                auto cont = getContainer();
                if (cont)
                {
                    auto dis = CUI::DiscardPanel::create(CENTER, PARENT);
                    dis->init(L"> Rename Layer <", L"Layer Name", CUI::SUBMIT_CANCEL);
                    dis->textField->setString(Strings::narrow(_layerManager.layers[_layerManager.boundLayer]));
                    cont->pushModal(dis);
                    dis->enterCallback = [&](CUI::Button*, std::wstring name)
                        {
                            int idx = _layerManager.boundLayer;

                            editorPushUndoState();
                            auto& undo = editorTopUndoStateOrDefault();
                            undo.setAction(EditorToolbox::UNDOREDO_LAYER_MODIFY);

                            undo.affectedLayers.action = EditorToolbox::UNDOREDO_LAYER_RENAME;
                            undo.affectedLayers.manager = &_layerManager;

                            undo.affectedLayers.prev_layer_idx = idx;

                            undo.affectedLayers.layer.name = _layerManager.layers[idx];
                            undo.affectedLayers.layer.layer = map->_layers[idx];
                            undo.affectedLayers.new_name = name;

                            _layerManager.renameGeneralLayer(idx, name);
                        };
                }
            }
        };

    btnt2 = removeLayerBtn = CUI::Button::create();
    btnt2->DenyRescaling();
    btnt2->disableArtMul();
    btnt2->setUiPadding(V2D(10, 0));
    btnt2->initIcon("editor_trashbin");
    btnt2->hoverTooltip = L"Removes the selected layer along with its content altogether.\nThis action can safely be undone.";
    cont2->addChild(btnt2);

    btnt2->_callback = [&](CUI::Button* target)
        {
            editorPushUndoState();
            auto& undo = editorTopUndoStateOrDefault();
            undo.setAction(EditorToolbox::UNDOREDO_LAYER_MODIFY);
            int idx = _layerManager.boundLayer;
            if (idx < _layerManager.getLayerCount())
            {
                undo.affectedLayers.action = EditorToolbox::UNDOREDO_LAYER_DELETE;
                undo.affectedLayers.manager = &_layerManager;

                undo.affectedLayers.prev_layer_idx = idx;

                undo.affectedLayers.layer.name = _layerManager.layers[idx];
                undo.affectedLayers.layer.layer = map->_layers[idx];
                AX_SAFE_RETAIN(undo.affectedLayers.layer.layer);

                _layerManager.removeLayer(idx);
            }
        };

    topRightContainer->addChild(cont2);

    auto tilesetContainer = CUI::Container::create();
    tilesetContainer->setBorderLayout(BorderLayout::BOTTOM_RIGHT, BorderContext::PARENT);
    tilesetContainer->setBorderLayoutAnchor(BorderLayout::BOTTOM_RIGHT);
    tilesetContainer->setBackgroundSpriteCramped({ 0, 0 });
    tilesetContainer->setBackgroundBlocking();
    tilesetContainer->setElementBlocking();
    tilesetContainer->setMargin({ 6, 6 });
    tilesetContainer->setLayout(CUI::FlowLayout(CUI::SORT_VERTICAL, CUI::STACK_CENTER, 20));
    container->addChild(tilesetContainer);

    auto tilesetListButton = CUI::Button::create();
    tilesetListButton->init(L"World سلام", TTFFS, { 394, 0});
    tilesetContainer->addChild(tilesetListButton);
    tilesetListButton->_callback = [=](CUI::Button*)
        {
            auto panel = EmptyPanel::create();
            panel->init(L"Menu Test 123");
            container->pushModal(panel);
        };

    _tilesetPicker = CUI::ImageView::create({ 400, 400 }, ADD_IMAGE("maps/level1/textures/atlas_002.png"));
    _tilesetPicker->enableGridSelection(map->_tileSize);
    tilesetContainer->addChild(_tilesetPicker);

    //cont2 = CUI::Container::create();
    //cont2->setLayout(FlowLayout(SORT_HORIZONTAL, STACK_CENTER, 25, 0, false));

    //btnt2 = CUI::Button::create();
    //btnt2->disableArtMul();
    //btnt2->setUiPadding(V2D(10, 0));
    //btnt2->initIcon("editor_new_layer");
    //btnt2->hoverTooltip = L"Creates a new empty layer with a name. The layer can be used for both Tilemaps & Objects.\nThe position at which the layer is in determines rendering order, Top will render front.\nYou can modify whether a layer is visible or interactable or both.\nYou can access advanced options for a specific layer using the wrench icon.";
    //cont2->addChild(btnt2);

    //btnt2 = CUI::Button::create();
    //btnt2->disableArtMul();
    //btnt2->initIcon("editor_arrow_up_2");
    //btnt2->hoverTooltip = L"Creates a new empty layer with a name. The layer can be used for both Tilemaps & Objects.\nThe position at which the layer is in determines rendering order, Top will render front.\nYou can modify whether a layer is visible or interactable or both.\nYou can access advanced options for a specific layer using the wrench icon.";
    //cont2->addChild(btnt2);

    //btnt2 = CUI::Button::create();
    //btnt2->disableArtMul();
    //btnt2->initIcon("editor_arrow_down_2");
    //btnt2->hoverTooltip = L"Creates a new empty layer with a name. The layer can be used for both Tilemaps & Objects.\nThe position at which the layer is in determines rendering order, Top will render front.\nYou can modify whether a layer is visible or interactable or both.\nYou can access advanced options for a specific layer using the wrench icon.";
    //cont2->addChild(btnt2);

    //btnt2 = CUI::Button::create();
    //btnt2->disableArtMul();
    //btnt2->setUiPadding(V2D(10, 0));
    //btnt2->initIcon("editor_rename");
    //btnt2->hoverTooltip = L"Creates a new empty layer with a name. The layer can be used for both Tilemaps & Objects.\nThe position at which the layer is in determines rendering order, Top will render front.\nYou can modify whether a layer is visible or interactable or both.\nYou can access advanced options for a specific layer using the wrench icon.";
    //cont2->addChild(btnt2);

    //btnt2 = CUI::Button::create();
    //btnt2->disableArtMul();
    //btnt2->setUiPadding(V2D(10, 0));
    //btnt2->initIcon("editor_trashbin");
    //btnt2->hoverTooltip = L"Creates a new empty layer with a name. The layer can be used for both Tilemaps & Objects.\nThe position at which the layer is in determines rendering order, Top will render front.\nYou can modify whether a layer is visible or interactable or both.\nYou can access advanced options for a specific layer using the wrench icon.";
    //cont2->addChild(btnt2);

    //tilesetContainer->addChild(cont2);


    auto topLeftContainer = CUI::Container::create();
    topLeftContainer->setBorderLayout(BorderLayout::TOP_LEFT, BorderContext::PARENT);
    topLeftContainer->setLayout(CUI::FlowLayout(CUI::SORT_VERTICAL, CUI::STACK_CENTER, 0, 0, true));
    topLeftContainer->setBorderLayoutAnchor();
    topLeftContainer->setBackgroundSpriteCramped(V2D::ONE, { -1, -1 });
    container->addChild(topLeftContainer);

    auto menuContainer = CUI::Container::create();
    menuContainer->setLayout(CUI::FlowLayout(CUI::SORT_HORIZONTAL, CUI::STACK_CENTER, 0, 0, false));
    //menuContainer->setBorderLayoutAnchor(BorderLayout::RIGHT);
    menuContainer->setTag(CONTAINER_FLOW_TAG);
    topLeftContainer->addChild(menuContainer);
    menuContainer->setBackgroundBlocking();

    auto padding = Size(2, 10);
    auto hpadding = Size(3, 20);

    auto fileB = CUI::Button::create();
    fileB->init(L"File", 16, V2D::ZERO, hpadding);
    fileB->disableArtMul();
    fileB->setUiPadding(padding);
    menuContainer->addChild(fileB);

    auto editB = CUI::Button::create();
    editB->init(L"Edit", 16, V2D::ZERO, hpadding);
    editB->disableArtMul();
    editB->setUiPadding(padding);
    menuContainer->addChild(editB);

    editB->_callback = [=](CUI::Button* target) {
        auto fcontainer = CUI::Container::create();
        auto vis = Director::getInstance()->getVisibleSize();
        fcontainer->setBorderLayoutAnchor(TOP_LEFT);
        fcontainer->setConstraint(CUI::DependencyConstraint(CUI::callbackAccess["ext_edit_container"],
            BOTTOM_LEFT, { -0.55, 0.65 }, true, vis / -2));
        fcontainer->setLayout(CUI::FlowLayout(CUI::SORT_VERTICAL, CUI::STACK_CENTER));
        fcontainer->setMargin({ 0, 10 });
        fcontainer->setBackgroundSprite();
        fcontainer->setBlocking();
        fcontainer->setDismissible();
        fcontainer->setBackgroundBlocking();

        auto lb = CUI::Button::create();
        lb->init(L"Add Text Object", TTFFS);
        lb->setUiPadding({ 10, 5 });
        fcontainer->addChildAsContainer(lb);
        fcontainer->addSpecialChild(lb);

        lb->_callback = [=](CUI::Button* target) {
            for (int i = 0; i < 100; i++) {
                std::string s = Strings::gen_random(Random::rangeInt(5, 30));
                auto textObj = ax::Label::createWithBMFont(CUI::_fontName, s);
                auto selectable = Selectable::create(textObj);
                selectable->setScale(Random::float01() + 1);
                selectable->setRotation(Random::rangeFloat(-30, 30));
                _worlds[0]->addChild(selectable);
                _selectables.push_back(selectable);
                selectable->setPositionY(20 * i);
            }
        };

        lb = CUI::Button::create();
        lb->init(L"----------------------------------", TTFFS);
        lb->disable();
        lb->setUiPadding({ 10, 5 });
        fcontainer->addChildAsContainer(lb);
        fcontainer->addSpecialChild(lb);

        lb = CUI::Button::create();
        lb->init(L"Toggle TileMap Grid Visibility", TTFFS);
        lb->setUiPadding({ 10, 5 });
        fcontainer->addChildAsContainer(lb);
        fcontainer->addSpecialChild(lb);

        lb->_callback = [=](CUI::Button* target) {
            grid->setVisible(!grid->isVisible());
            CUI::callbackAccess["fcontainer"]->removeFromParent();
        };

        lb = CUI::Button::create();
        lb->init(L"Toggle TileMap 3D Perspective View", TTFFS);
        lb->setUiPadding({ 10, 5 });
        fcontainer->addChildAsContainer(lb);
        fcontainer->addSpecialChild(lb);

        lb->_callback = [=](CUI::Button* target) {
            auto action = ActionFloat::create(1, zPositionMultiplier, zPositionMultiplier < 0.5 ? 1 : 0,
                [=](float value) { zPositionMultiplier = tweenfunc::quintEaseInOut(value); });
            runAction(action);
            CUI::callbackAccess["fcontainer"]->removeFromParent();
        };

        CUI::Functions::menuContentFitButtons(fcontainer);

        CUI::callbackAccess["fcontainer"] = fcontainer;
        CUI::callbackAccess["main"]->addChild(fcontainer);
    };

    auto settingsB = CUI::Button::create();
    settingsB->init(L"Menu", 16, V2D::ZERO, hpadding);
    settingsB->disableArtMul();
    settingsB->setUiPadding(padding);
    menuContainer->addChild(settingsB);

    auto editContainer = CUI::Container::create();
    //editContainer->setBorderLayoutAnchor(BorderLayout::RIGHT);
    editContainer->setLayout(CUI::FlowLayout(CUI::SORT_HORIZONTAL, CUI::STACK_CENTER, 0, 0, false));
    editContainer->setTag(CONTAINER_FLOW_TAG);
    editContainer->setMargin({ 0, 1 });
    editContainer->setBackgroundBlocking();
    CUI::callbackAccess.emplace("edit_container", editContainer);

    padding = { 52, 5 };

    auto extContainer = CUI::Container::create();
    auto ext2Container = CUI::Container::create();

    CUI::callbackAccess.emplace("ext_edit_container", extContainer);

    fileB->_callback = [=](CUI::Button* target) {
        auto fcontainer = CUI::Container::create();
        auto vis = Director::getInstance()->getVisibleSize();
        fcontainer->setBorderLayoutAnchor(TOP_LEFT);
        fcontainer->setConstraint(CUI::DependencyConstraint(CUI::callbackAccess["ext_edit_container"],
            BOTTOM_LEFT, { -0.55, 0.65 }, true, vis / -2));
        fcontainer->setLayout(CUI::FlowLayout(CUI::SORT_VERTICAL, CUI::STACK_CENTER));
        fcontainer->setMargin({ 0, 10 });
        fcontainer->setBackgroundSprite();
        fcontainer->setBlocking();
        fcontainer->setDismissible();
        fcontainer->setBackgroundBlocking();

        auto lb = CUI::Button::create();
        lb->init(L"-- Resources ----------", TTFFS);
        lb->disable();
        lb->setUiPadding({ 10, 5 });
        fcontainer->addChild(lb);
        fcontainer->addSpecialChild(lb);

        lb = CUI::Button::create();
        lb->init(L"New Map", TTFFS);
        lb->setUiPadding({ 10, 5 });
        fcontainer->addChild(lb);
        fcontainer->addSpecialChild(lb);

        lb = CUI::Button::create();
        lb->init(L"Open Map", TTFFS);
        lb->setUiPadding({ 10, 5 });
        fcontainer->addChild(lb);
        fcontainer->addSpecialChild(lb);

        lb = CUI::Button::create();
        lb->init(L"Import Texture", TTFFS);
        lb->setUiPadding({ 10, 5 });
        fcontainer->addChild(lb);
        fcontainer->addSpecialChild(lb);

        lb = CUI::Button::create();
        lb->init(L"Reload Textures", TTFFS);
        lb->setUiPadding({ 10, 5 });
        fcontainer->addChild(lb);
        fcontainer->addSpecialChild(lb);

        lb = CUI::Button::create();
        lb->init(L"-- Changes ------------", TTFFS);
        lb->disable();
        lb->setUiPadding({ 10, 5 });
        fcontainer->addChild(lb);
        fcontainer->addSpecialChild(lb);

        lb = CUI::Button::create();
        lb->init(L"Save", TTFFS);
        lb->setUiPadding({ 10, 5 });
        fcontainer->addChild(lb);
        fcontainer->addSpecialChild(lb);

        lb = CUI::Button::create();
        lb->init(L"Save and Exit", TTFFS);
        lb->setUiPadding({ 10, 5 });
        fcontainer->addChild(lb);
        fcontainer->addSpecialChild(lb);

        lb = CUI::Button::create();
        lb->init(L"Exit without Saving", TTFFS);
        lb->setUiPadding({ 10, 5 });
        fcontainer->addChild(lb);
        fcontainer->addSpecialChild(lb);

        lb = CUI::Button::create();
        lb->init(L"-- Other --------------", TTFFS);
        lb->disable();
        lb->setUiPadding({ 10, 5 });
        fcontainer->addChild(lb);
        fcontainer->addSpecialChild(lb);

        auto slc = CUI::Container::create();
        slc->setLayout(CUI::FlowLayout(CUI::SORT_HORIZONTAL, CUI::STACK_CENTER, 0, 0, false));
        slc->setMargin({ 0, 0 });

        auto opl = CUI::Label::create();
        opl->init(L"UI Opacity  ", TTFFS);
        slc->addChild(opl);

        auto sl = CUI::Slider::create();
        sl->init({ 64, 6 });
        slc->addChild(sl);

        sl->_callback = [&](float v, CUI::Slider*) {
            getContainer()->setUiOpacity(v);
        };

        fcontainer->addChild(slc);

        auto t = CUI::Toggle::create();
        t->init(L"Toggle 1.0");
        fcontainer->addChild(t);

        lb = CUI::Button::create();
        lb->init(L"More Options...", TTFFS);
        lb->setUiPadding({ 10, 5 });
        fcontainer->addChild(lb);
        fcontainer->addSpecialChild(lb);

        /*lb->_callback = [=](CUI::Button* target) {
            fcontainer->removeFromParent();
            auto fcontainer1 = CUI::Container::create();
            auto vis = Director::getInstance()->getVisibleSize();
            fcontainer1->setBorderLayoutAnchor(TOP_LEFT);
            fcontainer1->setConstraint(CUI::DependencyConstraint(CUI::callbackAccess["ext_edit_container"],
                BOTTOM_LEFT, { -0.55, 0.55 }, true, vis / -2));
            fcontainer1->setLayout(CUI::FlowLayout(CUI::SORT_VERTICAL, CUI::STACK_CENTER));
            fcontainer1->setMargin({ 0, 10 });
            fcontainer1->setBackgroundSprite();
            fcontainer1->setBlocking();
            fcontainer1->setDismissible();

            auto lb = CUI::Button::create();
            lb->init(L"-- Resources --------------------------", TTFFS);
            lb->disable();
            lb->setUiPadding({ 10, 5 });
            fcontainer1->addChild(lb);

            lb = CUI::Button::create();
            lb->init(L"Import .TTF/.OTF Font File", TTFFS);
            lb->setUiPadding({ 10, 5 });
            fcontainer1->addChild(lb);

            lb = CUI::Button::create();
            lb->init(L"-- Passes -----------------------------", TTFFS);
            lb->disable();
            lb->setUiPadding({ 10, 5 });
            fcontainer1->addChild(lb);

            lb = CUI::Button::create();
            lb->init(L"Open Render Pass Editor", TTFFS);
            lb->setUiPadding({ 10, 5 });
            fcontainer1->addChild(lb);

            lb = CUI::Button::create();
            lb->init(L"-- TileMaps ---------------------------", TTFFS);
            lb->disable();
            lb->setUiPadding({ 10, 5 });
            fcontainer1->addChild(lb);

            lb = CUI::Button::create();
            lb->init(L"Switch to Bilinear Rendering", TTFFS);
            lb->setUiPadding({ 10, 5 });
            fcontainer1->addChild(lb);

            lb = CUI::Button::create();
            lb->init(L"-- Shaders ----------------------------", TTFFS);
            lb->disable();
            lb->setUiPadding({ 10, 5 });
            fcontainer1->addChild(lb);

            lb = CUI::Button::create();
            lb->init(L"Import GLSL Shader", TTFFS);
            lb->setUiPadding({ 10, 5 });
            fcontainer1->addChild(lb);

            lb = CUI::Button::create();
            lb->init(L"Compile GLSL v3.0 Compliant Shader Code", TTFFS);
            lb->setUiPadding({ 10, 5 });
            fcontainer1->addChild(lb);

            lb = CUI::Button::create();
            lb->init(L"-- Native -----------------------------", TTFFS);
            lb->disable();
            lb->setUiPadding({ 10, 5 });
            fcontainer1->addChild(lb);

            lb = CUI::Button::create();
            lb->init(L"SQLite Vacuum File", TTFFS);
            lb->setUiPadding({ 10, 5 });
            fcontainer1->addChild(lb);

            CUI::Functions::menuContentFitButtons(fcontainer1);

            CUI::callbackAccess["main"]->addChild(fcontainer1);
        };*/

        CUI::Functions::menuContentFitButtons(fcontainer);

        CUI::callbackAccess["main"]->addChild(fcontainer);
    };

    undoB = CUI::Button::create();
    undoB->hoverTooltip = L"undo last action (Ctrl+Z)";
    undoB->initIcon("editor_undo");
    undoB->setUiPadding(padding);
    editContainer->addChild(undoB);

    undoB->_callback = [&](CUI::Button* target) {
        editorUndo();
    };

    redoB = CUI::Button::create();
    redoB->hoverTooltip = L"redo last action (Ctrl+Y)";
    redoB->initIcon("editor_redo");
    redoB->setUiPadding(padding);
    editContainer->addChild(redoB);

    redoB->_callback = [&](CUI::Button* target) {
        editorRedo();
    };

    auto moveB = CUI::Button::create();
    moveB->hoverTooltip = L"move camera (M)... isn't it obvious?";
    moveB->initIcon("editor_move");
    moveB->setUiPadding(padding);
    editContainer->addChild(moveB);

    CONTAINER_MAKE_MINIMIZABLE(topLeftContainer);

    auto vis = Director::getInstance()->getVisibleSize();
    ext2Container->setBorderLayoutAnchor(TOP_LEFT);
    ext2Container->setConstraint(CUI::DependencyConstraint(topLeftContainer,
        TOP_RIGHT, { -0, 0 }, false));
    ext2Container->setLayout(CUI::FlowLayout(CUI::SORT_VERTICAL, CUI::STACK_CENTER, 0));
    ext2Container->setBackgroundSpriteCramped(V2D::ZERO, { -1, -1 });
    ext2Container->setTag(GUI_ELEMENT_EXCLUDE);
    ext2Container->setBackgroundBlocking();
    ext2Container->setMargin({ 5, 0 });

    auto rowContainer = CUI::Container::create();
    rowContainer->setLayout(CUI::FlowLayout(CUI::SORT_HORIZONTAL, CUI::STACK_CENTER, 32, 0, false));
    rowContainer->setTag(CONTAINER_FLOW_TAG);
    rowContainer->setMargin({ 0, 4 });

    modeDropdown = CUI::DropDown::create();
    modeDropdown->setUiPadding({ 10, 10 });
    auto items = std::vector<std::wstring>{ L"Object Mode (UNIMPLEMENTED)",L"TileMap Mode" };
    modeDropdown->init(items);
    modeDropdown->_callback = [=](DropDown* target) {
        target->showMenu(ext2Container, LEFT, TOP_LEFT, V2D(-0.55, 0.5));
    };
    rowContainer->addChild(modeDropdown);

    ext2Container->addChild(rowContainer);

    extContainer->setBorderLayoutAnchor(TOP_LEFT);
    extContainer->setConstraint(CUI::DependencyConstraint(CUI::callbackAccess["edit_container"],
        BOTTOM_LEFT, { -0, 0 }, false, V2D(-2, 0.25)));
    extContainer->setLayout(CUI::FlowLayout(CUI::SORT_VERTICAL, CUI::STACK_CENTER, 0));
    extContainer->setBackgroundSpriteCramped(V2D::ZERO, { -1, -1 });
    extContainer->setTag(GUI_ELEMENT_EXCLUDE);
    extContainer->setBackgroundBlocking();
    extContainer->setMargin({ 5, 0 });

    rowContainer = CUI::Container::create();
    rowContainer->setLayout(CUI::FlowLayout(CUI::SORT_HORIZONTAL, CUI::STACK_CENTER, 32, 0, false));
    rowContainer->setTag(CONTAINER_FLOW_TAG);
    rowContainer->setMargin({ 0, 4 });

    padding = V2D(8, 2);

    auto extEditB = placeB = CUI::Button::create();
    extEditB->hoverTooltip = L"Place (B) Tiles into the selected layer,\nright click for rectangle placement.";
    extEditB->initIcon("editor_place", padding);
    rowContainer->addChild(extEditB);

    extEditB->_callback = [&](CUI::Button* target) {
        setTileMapEditMode(TileMapEditMode::PLACE);
    };

    extEditB = removeB = CUI::Button::create();
    extEditB->hoverTooltip = L"Remove (R) Tiles from the selected layer,\nright click for rectangle removal.";
    extEditB->initIcon("editor_remove", padding);
    rowContainer->addChild(extEditB);

    extEditB->_callback = [&](CUI::Button* target) {
        setTileMapEditMode(TileMapEditMode::REMOVE);
    };

    extEditB = bucketB = CUI::Button::create();
    extEditB->hoverTooltip = L"Bucket Fill (F) Tiles into the selected layer.";
    extEditB->initIcon("editor_bucket_fill", padding);
    rowContainer->addChild(extEditB);

    extEditB->_callback = [&](CUI::Button* target) {
        setTileMapEditMode(TileMapEditMode::BUCKET);
    };

    extEditB = selectB = CUI::Button::create();
    extEditB->hoverTooltip = L"Select (P) Tiles in the selected layer,\nright click for rectangle selection.";
    extEditB->initIcon("editor_select", padding);
    rowContainer->addChild(extEditB);

    extEditB->_callback = [&](CUI::Button* target) {
        setTileMapEditMode(TileMapEditMode::SELECT);
    };

    extContainer->addChild(rowContainer);

    rowContainer = CUI::Container::create();
    rowContainer->setLayout(CUI::FlowLayout(CUI::SORT_HORIZONTAL, CUI::STACK_CENTER, 30, 0, false));
    rowContainer->setTag(CONTAINER_FLOW_TAG);
    rowContainer->setMargin({ 0, 4 });

    tileFlipV = CUI::Button::create();
    tileFlipV->hoverTooltip = L"Flip Tile Vertically (V)";
    tileFlipV->initIcon("editor_flip_v", padding);
    rowContainer->addChild(tileFlipV);

    tileFlipV->_callback = [&](CUI::Button* target) {
        editorTileCoords.flipV();
        editorTileFlipRotateUpdateState();
    };


    tileFlipH = CUI::Button::create();
    tileFlipH->hoverTooltip = L"Flip Tile Horizontally (H)";
    tileFlipH->initIcon("editor_flip_h", padding);
    rowContainer->addChild(tileFlipH);

    tileFlipH->_callback = [&](CUI::Button* target) {
        editorTileCoords.flipH();
        editorTileFlipRotateUpdateState();
    };

    tileRot90 = CUI::Button::create();
    tileRot90->hoverTooltip = L"Rotate Tile (E)\nThis will modify H & V";
    tileRot90->initIcon("editor_rotate_r", padding);
    rowContainer->addChild(tileRot90);

    tileRot90->_callback = [&](CUI::Button* target) {
        editorTileCoords.cw();
        editorTileFlipRotateUpdateState();
    };

    extContainer->addChild(rowContainer);

    editContainer->addChild(extContainer);

    topLeftContainer->addChild(ext2Container);
    topLeftContainer->addChild(editContainer);

    auto cameraScaleContainer = CUI::Container::create();
    cameraScaleContainer->setBorderLayout(BorderLayout::TOP, BorderContext::PARENT);
    cameraScaleContainer->setLayout(CUI::FlowLayout(CUI::SORT_HORIZONTAL, CUI::STACK_CENTER, 20, 0, false));
    cameraScaleContainer->setBorderLayoutAnchor();
    cameraScaleContainer->setMargin({ 0, 2 });
    cameraScaleContainer->setBackgroundBlocking();
    container->addChild(cameraScaleContainer);

    cameraScaleB = CUI::Button::create();
    cameraScaleB->initIcon("editor_zoom_aligned");
    cameraScaleB->_callback = [&](CUI::Button* target) {
        if (cameraScale != 1)
        {
            I32 i = 0;
            I32 n = (sizeof(possibleCameraScales) / sizeof(possibleCameraScales[0])) - 1;
            while (i < n)
            {
                if (possibleCameraScales[i] == 1.0F)
                    break;
                i++;
            }
            cameraScaleIndex = i;
            cameraScale = possibleCameraScales[cameraScaleIndex];
            setCameraScaleUiText(cameraScale);
        }
    };
    cameraScaleContainer->addChild(cameraScaleB);

    cameraScaleL = CUI::Label::create();
    cameraScaleL->init(L"", TTFFS);
    cameraScaleL->enableOutline();
    cameraScaleContainer->addChild(cameraScaleL);

    rebuildableUiNodes->removeAllChildren();
    _debugText = CUI::Label::create();
    _debugText->init(L"", TTFFS);
    _debugText->enableOutline();
    auto _debugTextCont = TO_CONTAINER(_debugText);
    _debugTextCont->setBorderLayout(BOTTOM_LEFT, PARENT);
    _debugTextCont->setBorderLayoutAnchor(BOTTOM_LEFT);
    _debugTextCont->setMargin({ 4, 2 });
    getContainer()->addChild(_debugTextCont, 99);
    /* FPS COUNTER CODE BODY */ {
        _debugText->stopAllActions();
        auto update_fps_action = CallFunc::create([&]() {
            wchar_t buff[14];
            wchar_t buffDt[14];
            fps_dt += (_director->getInstance()->getDeltaTime() - fps_dt) * 0.5f;
            _snwprintf(buff, sizeof(buff), L"%.1lf", 1.0F / fps_dt);
            _snwprintf(buffDt, sizeof(buffDt), L"%.1lf", fps_dt * 1000);
            std::wstring buffAsStdStr = buff;
            std::wstring buffAsStdStrDt = buffDt;
            I32 verts = (I32)Director::getInstance()->getRenderer()->getDrawnVertices();
            I32 batches = (I32)Director::getInstance()->getRenderer()->getDrawnBatches();
            std::wstring text = WFMT(L"T: %d | C: %d\n", map->_tileCount, 0) + L"D3D11: " + buffAsStdStr + L" / " + buffAsStdStrDt + L"ms\n" +
                WFMT(L"%s: %d / ", L"Draw Calls", batches) + WFMT(L"%s: %d", L"Vertices Drawn", verts);
            _debugText->setString(text);
            });
        auto wait_fps_action = DelayTime::create(0.5f);
        auto make_seq = Sequence::create(update_fps_action, wait_fps_action, nullptr);
        auto seq_repeat_forever = RepeatForever::create(make_seq);
        _debugText->runAction(seq_repeat_forever);
    }

    _editorToolTip = CUI::ToolTip::create();
    container->addChild(_editorToolTip, 2);

    BENCHMARK_SECTION_BEGIN("Create Color Manager Panel");
    container->addChild(createFledgedHSVPanel());
    BENCHMARK_SECTION_END();

    rebuildEntireUi();
}

void MapEditor::fillContainerColorGrid(EditorToolbox::ColorChannelManager* m, CUI::Container* c, int rows, int columns, int page, std::function<void(int i)> _onColorSelect)
{
    m->nullifyCells();

    for (auto& _ : c->_userData.l1)
        _->removeFromParentAndCleanup(true);
    c->_userData.l1.clear();

    auto flowCL = FlowLayout(SORT_VERTICAL, STACK_CENTER, 5);
    flowCL.constSize = true; flowCL.constSizeV = V2D(1, 30);
    auto colBColorCont = Container::create();
    colBColorCont->setLayout(flowCL);
    auto colColorCont = Container::create();
    colColorCont->setLayout(flowCL);
    c->addChild(colBColorCont);
    c->addChild(colColorCont);
    c->_userData.l1.push_back(colBColorCont);
    c->_userData.l1.push_back(colColorCont);
    int idx = page * rows * columns;
    for (int i = 0; i < rows; i++) {
        if (idx > 999) break;
        if (i != 0) colBColorCont->addChild(Separator::create(V2D(1, 3)));
        auto rowBColorCont = Container::create();
        auto flowL = FlowLayout(SORT_HORIZONTAL, STACK_CENTER, 24, 0, false);
        flowL.constSize = true; flowL.constSizeV = V2D(50, 1);
        rowBColorCont->setLayout(flowL);
        for (int i = 0; i < columns; i++) {
            if (idx > 999) break;
            auto bgb = Button::create();
            bgb->initIcon("color_cell", Rect::ZERO);
            bgb->runActionOnIcon = false;
            auto& col = m->getColor(idx);
            bgb->icon->setColor(Color3B(col.color));
            bgb->icon->setOpacity(col.color.a * 255);
            //if (col.pCell) AX_SAFE_RELEASE(col.pCell); // release ownership of cell
            col.pCell = bgb->icon; //AX_SAFE_RETAIN(bgb->icon); // capture ownership of cell
            rowBColorCont->addChild(bgb);
            bgb->_callback = [=](Button* target) {
                _onColorSelect(idx);
            };
            idx++;
        }
        colBColorCont->addChild(rowBColorCont);
    }
    idx = page * rows * columns;
    for (int i = 0; i < rows; i++) {
        if (idx > 999) break;
        if (i != 0) colColorCont->addChild(Separator::create(V2D(1, 3)));
        auto rowColorCont = Container::create();
        auto flowL = FlowLayout(SORT_HORIZONTAL, STACK_CENTER, 24, 0, false);
        flowL.constSize = true; flowL.constSizeV = V2D(50, 1);
        rowColorCont->setLayout(flowL);
        for (int i = 0; i < columns; i++) {
            if (idx > 999) break;
            auto lb = CUI::Label::create();
            lb->init(WFMT(L"%d", idx++), TTFFS);
            lb->setOpacity(200);
            lb->field->setAdditionalKerning(2);
            rowColorCont->addChild(lb);
        }
        colColorCont->addChild(rowColorCont);
    }
}

CUI::Container* MapEditor::createFledgedHSVPanel() {
    auto container = Container::create();
    container->setBorderLayout(LEFT, BorderContext::PARENT);
    container->setBorderLayoutAnchor(LEFT);
    container->setLayout(FlowLayout(SORT_VERTICAL));
    container->setBackgroundSprite();
    container->setBackgroundBlocking();

    auto manager = &channelMgr;

    auto hsv = currentShownWheel = HSVWheel::create();
    hsv->channelMgr = manager;
    container->addChild(hsv);

    hsv->_onClickCallback = CALL0(MapEditor::editorPushUndoColorPalette);

    auto hsvcontrol = Container::create();
    hsvcontrol->setStatic();
    hsvcontrol->setConstraint(ContentSizeConstraint(hsv, V2D::ZERO, true));
    container->addChild(hsvcontrol);

    auto colorCont = Container::create();
    colorCont->setLayout(FlowLayout(SORT_VERTICAL, STACK_CENTER, 4, 0));
    hsvcontrol->addChild(colorCont);

    auto colorContHex = Container::create();
    colorContHex->setLayout(FlowLayout(SORT_HORIZONTAL, STACK_CENTER, 8, 0, false));
    auto lb = CUI::Label::create();
    lb->init(L"  hexadecimal:  ", TTFFS);
    auto hextf = TextField::create();
    hextf->init(L"HEX COLOR", TTFFS, { 145, 30 }, 9, "#0123456789abcdefABCDEF");
    hextf->_onFocusCallback = CALL0(MapEditor::editorPushUndoColorPalette);
    hextf->setStyleDotted();
    hextf->remove_zeros = false;
    hextf->hoverTooltip = L"You can use hexadecimal format to get a color value including alpha (i.e #ffffffff)\nif there are no alpha bits (i.e #ffffff) then alpha channel will be set to 255\nThe '#' at the beginning is not mandatory";
    colorContHex->addChild(lb);
    colorContHex->addChild(hextf);
    colorCont->addChild(colorContHex);

    auto colorContRed = Container::create();
    colorContRed->setLayout(FlowLayout(SORT_HORIZONTAL, STACK_CENTER, 8, 0, false));
    lb = CUI::Label::create();
    lb->init(L"  red:", TTFFS);
    auto slr = Slider::create();
    slr->init({ 90, 10 });
    auto tfr = TextField::create();
    tfr->init(L" ", TTFFS, { 70, 20 }, 3, "0123456789");
    tfr->_onFocusCallback = CALL0(MapEditor::editorPushUndoColorPalette);
    tfr->setStyleDotted();
    colorContRed->addChild(lb);
    colorContRed->addChild(slr);
    colorContRed->addChild(tfr);
    colorCont->addChild(colorContRed);

    auto colorContGreen = Container::create();
    colorContGreen->setLayout(FlowLayout(SORT_HORIZONTAL, STACK_CENTER, 8, 0, false));
    lb = CUI::Label::create();
    lb->init(L"green:", TTFFS);
    auto slg = Slider::create();
    slg->init({ 90, 10 });
    auto tfg = TextField::create();
    tfg->init(L" ", TTFFS, { 70, 20 }, 3, "0123456789");
    tfg->_onFocusCallback = CALL0(MapEditor::editorPushUndoColorPalette);
    tfg->setStyleDotted();
    colorContGreen->addChild(lb);
    colorContGreen->addChild(slg);
    colorContGreen->addChild(tfg);
    colorCont->addChild(colorContGreen);

    auto colorContBlue = Container::create();
    colorContBlue->setLayout(FlowLayout(SORT_HORIZONTAL, STACK_CENTER, 8, 0, false));
    lb = CUI::Label::create();
    lb->init(L" blue:", TTFFS);
    auto slb = Slider::create();
    slb->init({ 90, 10 });
    auto tfb = TextField::create();
    tfb->init(L" ", TTFFS, { 70, 20 }, 3, "0123456789");
    tfb->_onFocusCallback = CALL0(MapEditor::editorPushUndoColorPalette);
    tfb->setStyleDotted();
    colorContBlue->addChild(lb);
    colorContBlue->addChild(slb);
    colorContBlue->addChild(tfb);
    colorCont->addChild(colorContBlue);

    auto colorContAlpha = Container::create();
    colorContAlpha->setLayout(FlowLayout(SORT_HORIZONTAL, STACK_CENTER, 8, 0, false));
    lb = CUI::Label::create();
    lb->init(L"alpha:", TTFFS);
    auto sla = Slider::create();
    sla->init({ 90, 10 });
    auto tfa = TextField::create();
    tfa->init(L" ", TTFFS, { 70, 20 }, 3, "0123456789");
    tfa->_onFocusCallback = CALL0(MapEditor::editorPushUndoColorPalette);
    tfa->setStyleDotted();
    colorContAlpha->addChild(lb);
    colorContAlpha->addChild(sla);
    colorContAlpha->addChild(tfa);
    colorCont->addChild(colorContAlpha);
    colorCont->addChild(Separator::create(V2D(1, 10)));

    auto optionsCont = Container::create();
    optionsCont->setLayout(FlowLayout(SORT_VERTICAL, STACK_CENTER, 6, 0));
    auto toggle = Toggle::create();
    toggle->hoverTooltip = L"Whether to enable color blending on this color channel.";
    toggle->init(L"Enable Blending ");
    toggle->toggle(true);
    optionsCont->addChild(toggle);
    optionsCont->addChild(Separator::create(V2D(1, 10)));
    auto rg = new RadioGroup();
    toggle = Toggle::create();
    toggle->hoverTooltip = L"GL_FUNC_ADD: This is the default blending operation,\nit adds the source color to the destination color.";
    toggle->init(L"Add (DEFAULT)   ");
    rg->addChild(toggle);
    toggle->toggle(true);
    optionsCont->addChild(toggle);
    toggle = Toggle::create();
    toggle->hoverTooltip = L"GL_FUNC_SUBTRACT: This subtracts the source color from the destination color.";
    toggle->init(L"Subtract        ");
    rg->addChild(toggle);
    optionsCont->addChild(toggle);
    toggle = Toggle::create();
    toggle->hoverTooltip = L"GL_FUNC_REVERSE_SUBTRACT: This subtracts the destination color from the source color.";
    toggle->init(L"Reserve Subtract");
    rg->addChild(toggle);
    optionsCont->addChild(toggle);
    hsvcontrol->addChild(optionsCont);
    optionsCont->addChild(Separator::create(V2D(1, 10)));

    auto options2Cont = Container::create();
    options2Cont->setBorderLayout(LEFT, BorderContext::PARENT);
    options2Cont->setBorderLayoutAnchor(CENTER);
    options2Cont->setLayout(FlowLayout(SORT_VERTICAL, STACK_CENTER, 0, 0, true));

    {
        auto alphaBlending = std::vector<std::wstring>({
            L"zero", L"one", L"src alpha",
            L"one minus src alpha", L"dst alpha",
            L"one minus dst alpha", L"constant alpha",
            L"src alpha saturate", L"one minus const alpha"
            });

        auto dd1c = Container::create();
        dd1c->setLayout(FlowLayout(SORT_HORIZONTAL, STACK_RIGHT, 10, 15, false));
        auto ddlb = CUI::Label::create();
        ddlb->init(L"SRC A:", TTFFS);
        ddlb->color = LB_INACTIVE;
        auto dd1 = DropDown::create();
        dd1->init(alphaBlending);
        dd1->_callback = [=](DropDown* target) {
            target->showMenu(hsvcontrol, LEFT, LEFT, V2D(-0.1, 0));
        };
        dd1c->addChild(ddlb);
        dd1c->addChild(dd1);
        options2Cont->addChild(dd1c);
    }

    {
        auto alphaBlending = std::vector<std::wstring>({
            L"zero", L"one", L"src alpha",
            L"one minus src alpha", L"dst alpha",
            L"one minus dst alpha", L"constant alpha",
            L"src alpha saturate", L"one minus const alpha"
            });

        auto dd1c = Container::create();
        dd1c->setLayout(FlowLayout(SORT_HORIZONTAL, STACK_RIGHT, 10, 15, false));
        auto ddlb = CUI::Label::create();
        ddlb->init(L"DST A:", TTFFS);
        ddlb->color = LB_INACTIVE;
        auto dd1 = DropDown::create();
        dd1->init(alphaBlending);
        dd1->_callback = [=](DropDown* target) {
            target->showMenu(hsvcontrol, LEFT, LEFT, V2D(-0.1, 0));
        };
        dd1c->addChild(ddlb);
        dd1c->addChild(dd1);
        options2Cont->addChild(dd1c);
    }

    {
        auto colorBlending = std::vector<std::wstring>({
            L"zero", L"one", L"src color",
            L"one minus src color", L"dst color",
            L"one minus dst color"
            });

        auto dd1c = Container::create();
        dd1c->setLayout(FlowLayout(SORT_HORIZONTAL, STACK_RIGHT, 10, 15, false));
        auto ddlb = CUI::Label::create();
        ddlb->init(L"SRC C:", TTFFS);
        ddlb->color = LB_INACTIVE;
        auto dd1 = DropDown::create();
        dd1->init(colorBlending);
        dd1->_callback = [=](DropDown* target) {
            target->showMenu(hsvcontrol, LEFT, LEFT, V2D(-0.1, 0));
        };
        dd1c->addChild(ddlb);
        dd1c->addChild(dd1);
        options2Cont->addChild(dd1c);
    }

    {
        auto colorBlending = std::vector<std::wstring>({
            L"zero", L"one", L"src color",
            L"one minus src color", L"dst color",
            L"one minus dst color"
            });

        auto dd1c = Container::create();
        dd1c->setLayout(FlowLayout(SORT_HORIZONTAL, STACK_RIGHT, 10, 15, false));
        auto ddlb = CUI::Label::create();
        ddlb->init(L"DST C:", TTFFS);
        ddlb->color = LB_INACTIVE;
        auto dd1 = DropDown::create();
        dd1->init(colorBlending);
        dd1->_callback = [=](DropDown* target) {
            target->showMenu(hsvcontrol, LEFT, LEFT, V2D(-0.1, 0));
        };
        dd1c->addChild(ddlb);
        dd1c->addChild(dd1);
        options2Cont->addChild(dd1c);
    }
    options2Cont->addChild(Separator::create(V2D(1, 10)));

    hsvcontrol->addChild(options2Cont);

    auto selGroupCont = Container::create();
    selGroupCont->setBorderLayout(BOTTOM_LEFT, BorderContext::PARENT);
    selGroupCont->setBorderLayoutAnchor(BOTTOM_LEFT);
    selGroupCont->setMargin({ 0, 12 });
    auto glb = CUI::Label::create();
    glb->init(L"Channel: 0", TTFFS);
    glb->setUiPadding(V2D(34, 0));
    selGroupCont->addChild(glb);
    hsvcontrol->addChild(selGroupCont, 1);

    auto channelCont = Container::create();
    channelCont->_onContainerLayoutUpdate = [](Container* self) { self->setPositionY(4 * Rebound::getInstance()->gameWindow.guiScale); };

    auto flowCL = FlowLayout(SORT_HORIZONTAL, STACK_CENTER, 0);
    auto ns = GameUtils::getNodeIgnoreDesignScale();
    V2D hpadding = { 6, 85 };
    auto channelPageBCont = Container::create();
    channelPageBCont->setLayout(flowCL);
    channelCont->addChild(channelPageBCont);
    auto rightPageB = Button::create();
    rightPageB->initIcon("editor_arrow_right", hpadding);
    auto leftPageB = Button::create();
    leftPageB->initIcon("editor_arrow_left", hpadding);
    channelPageBCont->addChild(rightPageB);
    channelPageBCont->addChild(leftPageB);

    V2D rowcol = V2D(3, 5);

    auto onColorSelect = [=](int i) {
        glb->setString(WFMT(L"Channel: %d", i));
        hsv->currentChannel = channelId = i;
        hsv->updateColorValues(manager->getColor(i).color, true);
    };

    rightPageB->_callback = [=](Button* target) {
        fillContainerColorGrid(manager, channelCont, rowcol.u, rowcol.v, ++channelCont->_userData.index, onColorSelect);
        channelCont->updateLayoutManagers(true);
        if (channelCont->_userData.index >= 66) rightPageB->disableSelf();
        leftPageB->enable();
    };

    leftPageB->_callback = [=](Button* target) {
        if (channelCont->_userData.index == 0) return;
        fillContainerColorGrid(manager, channelCont, rowcol.u, rowcol.v, --channelCont->_userData.index, onColorSelect);
        channelCont->updateLayoutManagers(true);
        if (channelCont->_userData.index == 0) leftPageB->disableSelf();
        rightPageB->enable();
    };

    channelCont->_userData.c1 = channelPageBCont;

    channelCont->_userData.index = 0;
    hsvcontrol->addChild(channelCont);
    channelCont->_onContainerTabSelected = [=](Container* s) {
        fillContainerColorGrid(manager, s, rowcol.u, rowcol.v, channelCont->_userData.index, onColorSelect);
        auto& fl = channelCont->_userData.c1->_flowLayout;
        fl.constSize = true; fl.constSizeV = V2D(hsvcontrol->getContentSize().x / ns.x - 40, 1);
        s->updateLayoutManagers(true);
    };

    auto dismissCont = Container::create();
    dismissCont->setBorderLayout(BOTTOM_RIGHT, BorderContext::PARENT);
    dismissCont->setBorderLayoutAnchor(BOTTOM_RIGHT);
    dismissCont->setMargin({ 0, 12 });
    hsvcontrol->addChild(dismissCont, 1);
    auto closeB = Button::create();
    closeB->init(L"Close", TTFFS);
    closeB->_callback = [=](Button* target) {
        editorUndoRedoAlterStacks([=](EditorToolbox::UndoRedoState& state) {
            if (state.affectedColors.wheel == hsv)
                state.affectedColors.wheel = nullptr;
            });
        container->removeFromParent();
    };
    dismissCont->addChild(closeB);

    auto tabs = Tabs::create(V2D::ZERO);
    tabs->setConstraint(ContentSizeConstraint(hsv, { -20, 0 }, true, false, true));
    tabs->setBorderLayout(TOP, BorderContext::PARENT);
    tabs->setBorderLayoutAnchor(TOP);
    hsvcontrol->addChild(tabs);
    tabs->addElement(L"Values", colorCont);
    tabs->addElement(L"Channels", channelCont);
    tabs->addElement(L"Blend Oper", optionsCont);
    tabs->addElement(L"Blend Func", options2Cont);
    tabs->addElement(L"Shaders");
    tabs->addElement(L"Shader Properties");

    hsv->_callback = [=](const HSV& hsv, HSVWheel* target) {
        auto col = hsv.toColor4F();
        hextf->setString(ColorConversion::rgba2hex(col));
        slr->setValue(col.r, false);
        slg->setValue(col.g, false);
        slb->setValue(col.b, false);
        sla->setValue(col.a, false);
        tfr->setString(FMT("%d", int(col.r * 255)));
        tfg->setString(FMT("%d", int(col.g * 255)));
        tfb->setString(FMT("%d", int(col.b * 255)));
        tfa->setString(FMT("%d", int(col.a * 255)));
        if (!target->doNotPushStateOnce) {
            EditorToolbox::ColorChannel newCol = manager->getColor(target->currentChannel);
            newCol.color = col;
            auto state = editorTopUndoStateOrNull();
            if (state) state->affectedColors.setColorNext(newCol);
        }
    };

    slr->_onClickCallback = CALL0(MapEditor::editorPushUndoColorPalette);
    slr->_callback = [=](float v, Slider* target) {
        tfr->setString(FMT("%d", int(v * 255)));
        auto col = hsv->hsv.toColor4F();
        col.r = v;
        hsv->hsv.fromRgba(col);
        hsv->updateColorValues();
        hextf->setString(ColorConversion::rgba2hex(col));
    };

    slg->_onClickCallback = CALL0(MapEditor::editorPushUndoColorPalette);
    slg->_callback = [=](float v, Slider* target) {
        tfg->setString(FMT("%d", int(v * 255)));
        auto col = hsv->hsv.toColor4F();
        col.g = v;
        hsv->hsv.fromRgba(col);
        hsv->updateColorValues();
        hextf->setString(ColorConversion::rgba2hex(col));
    };

    slb->_onClickCallback = CALL0(MapEditor::editorPushUndoColorPalette);
    slb->_callback = [=](float v, Slider* target) {
        tfb->setString(FMT("%d", int(v * 255)));
        auto col = hsv->hsv.toColor4F();
        col.b = v;
        hsv->hsv.fromRgba(col);
        hsv->updateColorValues();
        hextf->setString(ColorConversion::rgba2hex(col));
    };

    sla->_onClickCallback = CALL0(MapEditor::editorPushUndoColorPalette);
    sla->_callback = [=](float v, Slider* target) {
        tfa->setString(FMT("%d", int(v * 255)));
        auto col = hsv->hsv.toColor4F();
        col.a = v;
        hsv->hsv.fromRgba(col);
        hsv->updateColorValues();
        hextf->setString(ColorConversion::rgba2hex(col));
    };

    tfr->_callback = [=](TextField* target) {
        float r = 0.0;
        if (tfr->cachedString.length() != 0)
            r = stoi(tfr->cachedString) / 255.0;
        slr->setValue(r);
    };

    tfg->_callback = [=](TextField* target) {
        float g = 0.0;
        if (tfg->cachedString.length() != 0)
            g = stoi(tfg->cachedString) / 255.0;
        slg->setValue(g);
    };

    tfb->_callback = [=](TextField* target) {
        float b = 0.0;
        if (tfb->cachedString.length() != 0)
            b = stoi(tfb->cachedString) / 255.0;
        slb->setValue(b);
    };

    tfa->_callback = [=](TextField* target) {
        float a = 0.0;
        if (tfa->cachedString.length() != 0)
            a = stoi(tfa->cachedString) / 255.0;
        sla->setValue(a);
    };

    hextf->_callback = [=](TextField* target) {
        if (hextf->cachedString.length() == 0) return;
        Color4F col = ColorConversion::hex2rgba(Strings::narrow(target->cachedString));
        hsv->hsv.fromRgba(col);
        hsv->updateColorValues();
        hsv->_callback(hsv->hsv, hsv);
    };

    hsv->updateColorValues(manager->getColor(0).color);
    hsv->_callback(hsv->hsv, hsv);

    tabs->setSelection();

    return container;
}
