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
            auto size = ax::Vec2(0, prefferedHeight);
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

        static void fillContainerColorGrid(GameUtils::Editor::ColorChannelManager* m, Container* c,
            int rows, int columns, int page, std::function<void(int i)> _onColorSelect) {
            for (auto& _ : c->_userData.l1)
                _->removeFromParentAndCleanup(true);
            c->_userData.l1.clear();

            auto flowCL = FlowLayout(SORT_VERTICAL, STACK_CENTER, 5);
            flowCL.constSize = true; flowCL.constSizeV = Vec2(1, 30);
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
                if (i != 0) colBColorCont ->addChild(Separator::create(Vec2(1, 3)));
                auto rowBColorCont = Container::create();
                auto flowL = FlowLayout(SORT_HORIZONTAL, STACK_CENTER, 24, 0, false);
                flowL.constSize = true; flowL.constSizeV = Vec2(50, 1);
                rowBColorCont->setLayout(flowL);
                for (int i = 0; i < columns; i++) {
                    if (idx > 999) break;
                    auto bgb = Button::create();
                    bgb->initIcon("color_cell", Rect::ZERO);
                    bgb->runActionOnIcon = false;
                    auto& col = m->getColor(idx);
                    bgb->icon->setColor(Color3B(col.color));
                    bgb->icon->setOpacity(col.color.a * 255);
                    if (col.pCell) AX_SAFE_RELEASE(col.pCell); // release ownership of cell
                    col.pCell = bgb->icon; AX_SAFE_RETAIN(bgb->icon); // capture ownership of cell
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
                if (i != 0) colColorCont->addChild(Separator::create(Vec2(1, 3)));
                auto rowColorCont = Container::create();
                auto flowL = FlowLayout(SORT_HORIZONTAL, STACK_CENTER, 24, 0, false);
                flowL.constSize = true; flowL.constSizeV = Vec2(50, 1);
                rowColorCont->setLayout(flowL);
                for (int i = 0; i < columns; i++) {
                    if (idx > 999) break;
                    auto lb = Label::create();
                    lb->init(WFMT(L"%d", idx++), TTFFS);
                    lb->setOpacity(200);
                    lb->field->setAdditionalKerning(2);
                    rowColorCont->addChild(lb);
                }
                colColorCont->addChild(rowColorCont);
            }
        }

        static CUI::Container* createLayerWidget(std::wstring layer_name, CUI::ButtonCallback callback) {
            auto main = CUI::Container::create();
            main->DenyRescaling();
            main->setStatic();
            auto elem = CUI::Button::create();
            elem->DenyRescaling();
            elem->init(layer_name, TTFFS, { 180, 0 });
            elem->_callback = callback;
            main->setContentSize(Vec2(0, elem->preCalculatedHeight()), false);
            Vec2 hpadding = Vec2(2, 0);
            //elem->hAlignment = ax::TextHAlignment::LEFT;
            auto left = TO_CONTAINER(elem);
            left->DenyRescaling();
            left->setConstraint(CUI::DependencyConstraint(main, LEFT));
            left->setBorderLayoutAnchor(LEFT);
            auto visi = CUI::Button::create();
            visi->DenyRescaling();
            visi->initIcon("editor_visible", hpadding + Vec2(0, 4));
            auto lock = CUI::Button::create();
            lock->DenyRescaling();
            lock->initIcon("editor_lock", hpadding + Vec2(0, 2));
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
            return main;
        }
    };
}