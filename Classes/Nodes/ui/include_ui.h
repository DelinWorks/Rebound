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

#ifndef EXCLUDE_EXTENSIONS

#else
#undef EXCLUDE_EXTENSIONS;
#endif

#define CONTAINER_MAKE_MINIMIZABLE CustomUi::Functions::makeMinimizable

#define TO_CONTAINER(T) CustomUi::Functions::containerize(T)

namespace CustomUi {
    class Functions {
    public:
        static void makeMinimizable(CustomUi::Container* c) {
            auto closeCont = CustomUi::Container::create();
            closeCont->setBorderLayoutAnchor(TOP_LEFT, { 0.9, 0.9 });
            closeCont->setConstraint(CustomUi::DependencyConstraint(c, BOTTOM_RIGHT));
            closeCont->setTag(CONTAINER_CLOSE_TAG);
            c->addChild(closeCont);
            auto closeB = CustomUi::Button::create();
            closeB->initIcon("editor_arrow_tl", { 5, 5 });
            closeCont->addChild(closeB);
            closeB->_callback = [=](CustomUi::Button* target) {
                RLOGW("POSITION: {}, {}", closeCont->getWorldPosition().x, closeCont->getWorldPosition().y);                                                                                                                   \

                auto g = DCAST(CustomUi::GUI, c);
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
                    auto s = DCAST(CustomUi::Container, c);
                    
                    s->setAnchorOffset({ v, v });
                    s->getChildByTag<CustomUi::Container*>(CONTAINER_CLOSE_TAG)->updateLayoutManagers();
                });
                action->setTarget(c);
                c->runAction(action);
                target->icon->setSpriteFrame(isClosed ? "editor_arrow_tl" : "editor_arrow_br");
            };
        }

        static void menuContentFitButtons(CustomUi::Container* c, float maxWidth = INFINITY, float prefferedHeight = 0) {
            auto size = ax::Vec2(0, prefferedHeight);
            for (auto& _ : c->getChildren()) {
                auto b = DCAST(CustomUi::GUI, _);
                auto c = DCAST(CustomUi::Container, _);
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
                auto b = DCAST(CustomUi::Button, _);
                if (b) b->field_size = size * (1.0 / _UiScale);
            }
        }

        static CustomUi::Container* containerize(CustomUi::GUI* g) {
            auto cont = CustomUi::Container::create();
            cont->addChild(g);
            return cont;
        }
    };
}