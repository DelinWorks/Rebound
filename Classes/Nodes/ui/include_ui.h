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

#ifndef EXCLUDE_EXTENSIONS

#else
#undef EXCLUDE_EXTENSIONS;
#endif

#define CONTAINER_MAKE_MINIMIZABLE CUI::Functions::makeMinimizable

#define TO_CONTAINER(T) CUI::Functions::containerize(T)

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

        static Container* createFledgedHSVPanel() {
            auto container = Container::create();
            container->setBorderLayout(LEFT);
            container->setBorderLayoutAnchor(LEFT);
            container->setLayout(FlowLayout(SORT_VERTICAL));
            container->setBackgroundSprite();
            container->setBackgroundBlocking();

            auto hsv = HSVWheel::create();
            container->addChild(hsv);

            auto hsvcontrol = Container::create();
            hsvcontrol->setStatic();
            hsvcontrol->setConstraint(ContentSizeConstraint(hsv, ax::Vec2::ZERO, true));
            container->addChild(hsvcontrol);

            auto colorCont = Container::create();
            colorCont->setLayout(FlowLayout(SORT_VERTICAL, STACK_CENTER, 4, 0));
            hsvcontrol->addChild(colorCont);

            auto colorContHex = Container::create();
            colorContHex->setLayout(FlowLayout(SORT_HORIZONTAL, STACK_CENTER, 8, 0, false));
            auto lb = Label::create();
            lb->init(L"  hex:          ", TTFFS);
            auto hextf = TextField::create();
            hextf->init(L"HEX COLOR", TTFFS, { 145, 30 }, 9, "#0123456789abcdefABCDEF");
            hextf->setStyleDotted();
            hextf->remove_zeros = false;
            hextf->hoverTooltip = L"You can use hexadecimal format to get a color value including alpha (i.e #ffffffff)\nif there are no alpha bits (i.e #ffffff) then alpha channel will be set to 255\nThe '#' at the beginning is not mandatory";
            colorContHex->addChild(lb);
            colorContHex->addChild(hextf);
            colorCont->addChild(colorContHex);

            auto colorContRed = Container::create();
            colorContRed->setLayout(FlowLayout(SORT_HORIZONTAL, STACK_CENTER, 8, 0, false));
            lb = Label::create();
            lb->init(L"  red:", TTFFS);
            auto slr = Slider::create();
            slr->init({ 90, 10 });
            auto tfr = TextField::create();
            tfr->init(L" ", TTFFS, { 70, 20 }, 3, "0123456789");
            tfr->setStyleDotted();
            colorContRed->addChild(lb);
            colorContRed->addChild(slr);
            colorContRed->addChild(tfr);
            colorCont->addChild(colorContRed);

            auto colorContGreen = Container::create();
            colorContGreen->setLayout(FlowLayout(SORT_HORIZONTAL, STACK_CENTER, 8, 0, false));
            lb = Label::create();
            lb->init(L"green:", TTFFS);
            auto slg = Slider::create();
            slg->init({ 90, 10 });
            auto tfg = TextField::create();
            tfg->init(L" ", TTFFS, { 70, 20 }, 3, "0123456789");
            tfg->setStyleDotted();
            colorContGreen->addChild(lb);
            colorContGreen->addChild(slg);
            colorContGreen->addChild(tfg);
            colorCont->addChild(colorContGreen);

            auto colorContBlue = Container::create();
            colorContBlue->setLayout(FlowLayout(SORT_HORIZONTAL, STACK_CENTER, 8, 0, false));
            lb = Label::create();
            lb->init(L" blue:", TTFFS);
            auto slb = Slider::create();
            slb->init({ 90, 10 });
            auto tfb = TextField::create();
            tfb->init(L" ", TTFFS, { 70, 20 }, 3, "0123456789");
            tfb->setStyleDotted();
            colorContBlue->addChild(lb);
            colorContBlue->addChild(slb);
            colorContBlue->addChild(tfb);
            colorCont->addChild(colorContBlue);

            auto colorContAlpha = Container::create();
            colorContAlpha->setLayout(FlowLayout(SORT_HORIZONTAL, STACK_CENTER, 8, 0, false));
            lb = Label::create();
            lb->init(L"alpha:", TTFFS);
            auto sla = Slider::create();
            sla->init({ 90, 10 });
            auto tfa = TextField::create();
            tfa->init(L" ", TTFFS, { 70, 20 }, 3, "0123456789");
            tfa->setStyleDotted();
            colorContAlpha->addChild(lb);
            colorContAlpha->addChild(sla);
            colorContAlpha->addChild(tfa);
            colorCont->addChild(colorContAlpha);
            colorCont->addChild(Separator::create(Vec2(1, 10)));

            auto savedCont = Container::create();
            lb = Label::create();
            lb->init(L"TODO: IMPLEMENT COLOR SAVING", TTFFS);
            savedCont->addChild(lb);
            hsvcontrol->addChild(savedCont);

            auto optionsCont = Container::create();
            optionsCont->setLayout(FlowLayout(SORT_VERTICAL, STACK_CENTER, 2, 0));
            auto rg = new RadioGroup();
            auto toggle = Toggle::create();
            toggle->hoverTooltip = L"GL_FUNC_ADD: This is the default blending operation,\nit adds the source color to the destination color.";
            toggle->init(L"Add (DEFAULT)   ");
            rg->addChild(toggle);
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

            auto tabs = Tabs::create({ 0, 20 });
            tabs->setConstraint(ContentSizeConstraint(hsv, { -10, 0 }, true, false, true));
            tabs->setBorderLayout(TOP_LEFT, BorderContext::PARENT);
            tabs->setBorderLayoutAnchor(TOP_LEFT);
            hsvcontrol->addChild(tabs);
            tabs->addElement(L"Values", colorCont);
            tabs->addElement(L"Custom", savedCont);
            tabs->addElement(L"Contrast");
            tabs->addElement(L"Blend Oper", optionsCont);
            tabs->addElement(L"Blend Func");

            auto dismissCont = Container::create();
            dismissCont->setBorderLayout(BOTTOM, BorderContext::PARENT);
            dismissCont->setBorderLayoutAnchor(BOTTOM);
            dismissCont->setMargin({ 0, 8 });
            hsvcontrol->addChild(dismissCont);
            auto closeB = Button::create();
            closeB->init(L"Dismiss Panel", TTFFS);
            closeB->_callback = [=](Button* target) {
                container->removeFromParent();
            };
            dismissCont->addChild(closeB);

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
            };

            slr->_callback = [=](float v, Slider* target) {
                tfr->setString(FMT("%d", int(v * 255)));
                auto col = hsv->hsv.toColor4F();
                col.r = v;
                hsv->hsv.fromRgba(col);
                hsv->updateColorValues();
                hextf->setString(ColorConversion::rgba2hex(col));
            };

            slg->_callback = [=](float v, Slider* target) {
                tfg->setString(FMT("%d", int(v * 255)));
                auto col = hsv->hsv.toColor4F();
                col.g = v;
                hsv->hsv.fromRgba(col);
                hsv->updateColorValues();
                hextf->setString(ColorConversion::rgba2hex(col));
            };

            slb->_callback = [=](float v, Slider* target) {
                tfb->setString(FMT("%d", int(v * 255)));
                auto col = hsv->hsv.toColor4F();
                col.b = v;
                hsv->hsv.fromRgba(col);
                hsv->updateColorValues();
                hextf->setString(ColorConversion::rgba2hex(col));
            };

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

            hsv->updateColorValues(Color4F(1, 0, 1, 0.5));
            hsv->_callback(hsv->hsv, hsv);

            tabs->setSelection();

            return container;
        }
    };
}