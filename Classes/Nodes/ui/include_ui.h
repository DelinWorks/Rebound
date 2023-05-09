#pragma once

#include "uiContainer.h"

#include "uiHoverEffect.h"
#include "uiButton.h"
#include "uiTextField.h"
#include "uiLabel.h"
#include "uiModal.h"
#include "uiDiscardPanel.h"

#ifndef EXCLUDE_EXTENSIONS

#else
#undef EXCLUDE_EXTENSIONS;
#endif

#define CONTAINER_MAKE_MINIMIZABLE CustomUi::Behaviours::makeMinimizable

namespace CustomUi {
    class Behaviours {
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
                auto action = ActionFloat::create(0.08, isClosed ? -1 : 1, isClosed ? 1 : -1, [=](float v) {
                    auto s = DCAST(CustomUi::Container, c);
                    s->setAnchorOffset({ v,v });
                    s->getChildByTag<CustomUi::Container*>(CONTAINER_CLOSE_TAG)->updateLayoutManagers();
                });
                action->setTarget(c);
                c->runAction(action);
                target->icon->setSpriteFrame(isClosed ? "editor_arrow_tl" : "editor_arrow_br");
            };
        }
    };
}