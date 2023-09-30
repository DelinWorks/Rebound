#pragma once

#include "uiContainer.h"

#include "uiHoverEffect.h"
#include "uiButton.h"
#include "uiTextField.h"
#include "uiSlider.h"
#include "uiLabel.h"
#include "uiToggle.h"
#include "uiModal.h"
#include "uiDiscardPanel.h"
#include "uiImageView.h"
#include "uiHSVWheel.h"
#include "uiToolTip.h"
#include "uiContainer4Edge.h"
#include "uiTabs.h"
#include "uiList.h"
#include "uiDropDown.h"

#ifndef EXCLUDE_EXTENSIONS

#else
#undef EXCLUDE_EXTENSIONS;
#endif

#define CONTAINER_MAKE_MINIMIZABLE CUI::Functions::makeMinimizable

#define TO_CONTAINER(T) CUI::Functions::containerize(T)

const Color3B LB_INACTIVE(100, 100, 100);

namespace CUI {

    typedef enum EditorLayerWidget
    {
        kLayerMainContainer = 0xAA,
        kLayerNameButton = 0xAB,
    } EditorLayerWidget;

    class Functions {
    public:
        static void makeMinimizable(Container* c) {
            auto closeCont = Container::create();
            closeCont->setBorderLayoutAnchor(TOP_LEFT, { 0.9, 0.9 });
            closeCont->setConstraint(DependencyConstraint(c, BOTTOM_RIGHT));
            closeCont->setTag(CONTAINER_CLOSE_TAG);
            c->addChild(closeCont);
            auto closeB = Button::create();
            closeB->initIcon("editor_arrow_tl", { 5, 5 });
            closeCont->addChild(closeB);
            closeB->_callback = [=](Button* target) {
                RLOGW("POSITION: {}, {}", closeCont->getWorldPosition().x, closeCont->getWorldPosition().y);                                                                                                                   \

                auto g = DCAST(GUI, c);
                bool isClosed = g->getUiAnchorOffset().x < 0;
                for (auto& _ : c->getChildren()) {
                    auto cc = DCAST(Container, _);
                    if (cc && cc != closeCont)
                    {
                        if (!isClosed)
                            cc->disable();
                        else cc->enable();
                    }
                }
                auto action = ActionFloat::create(0.1, isClosed ? -1 : 1, isClosed ? 1 : -1, [=](float v) {
                    auto s = DCAST(Container, c);
                    
                    s->setAnchorOffset({ v, v });
                    s->getChildByTag<Container*>(CONTAINER_CLOSE_TAG)->updateLayoutManagers();
                });
                action->setTarget(c);
                c->runAction(action);
                target->icon->setSpriteFrame(isClosed ? "editor_arrow_tl" : "editor_arrow_br");
            };
        }

        static void menuContentFitButtons(Container* c, float maxWidth = INFINITY, float prefferedHeight = 0) {
            auto size = V2D(0, prefferedHeight);
            for (auto& _ : c->getChildren()) {
                auto b = DCAST(GUI, _);
                auto c = DCAST(Container, _);
                if (c) c->updateLayoutManagers();
                if (b) {
                    auto x = b->getFitContentSize().x;
                    if (x > size.x) size.x = x;
                }
            }
            for (auto& _ : c->_allButtons) {
                if (_) {
                    auto x = _->getFitContentSize().x;
                    if (x > size.x) size.x = x;
                }
            }
            if (size.x > maxWidth) size.x = maxWidth;
            for (auto& _ : c->_allButtons) {
                auto b = DCAST(Button, _);
                if (b) b->field_size = size * (1.0 / _UiScale);
            }
        }

        static Container* containerize(GUI* g) {
            auto cont = Container::create();
            cont->addChild(g);
            return cont;
        }

        static CUI::Container* createLayerWidget(std::wstring layer_name, CUI::ButtonCallback callback) {
            auto main = CUI::Container::create();
            main->DenyRescaling();
            main->setStatic();
            auto elem = CUI::Button::create();
            elem->DenyRescaling();
            elem->init(layer_name, TTFFS, { 280, 0 });
            elem->setTag(kLayerNameButton);
            elem->_callback = callback;
            main->setContentSize(V2D(0, elem->preCalculatedHeight()), false);
            main->setTag(kLayerMainContainer);
            V2D hpadding = V2D(2, 0);
            //elem->hAlignment = ax::TextHAlignment::LEFT;
            auto left = TO_CONTAINER(elem);
            left->DenyRescaling();
            left->setConstraint(CUI::DependencyConstraint(main, LEFT));
            left->setBorderLayoutAnchor(LEFT);
            auto visi = CUI::Button::create();
            visi->DenyRescaling();
            visi->initIcon("editor_visible", hpadding + V2D(0, 4));
            auto lock = CUI::Button::create();
            lock->DenyRescaling();
            lock->initIcon("editor_lock", hpadding + V2D(0, 2));
            auto opt = CUI::Button::create();
            opt->DenyRescaling();
            opt->initIcon("editor_settings", hpadding);
            auto right = CUI::Container::create();
            right->DenyRescaling();
            right->setLayout(CUI::FlowLayout(CUI::SORT_HORIZONTAL, CUI::STACK_LEFT, 0, 0, false));
            right->setConstraint(CUI::DependencyConstraint(main, RIGHT));
            right->addChild(visi);
            right->addChild(lock);
            right->addChild(opt);
            main->setMargin({ 0, 10 });
            main->addChild(left);
            main->addChild(right);
            left->disableRebuildOnEnter();
            right->disableRebuildOnEnter();
            main->disableRebuildOnEnter();
            return main;
        }
    };
}