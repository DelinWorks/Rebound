﻿#pragma once

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

        static Container* createFledgedHSVPanel() {
            auto container = Container::create();
            container->setBorderLayout(LEFT, BorderContext::PARENT);
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
            lb->init(L"  hexadecimal:  ", TTFFS);
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
            lb->setGlobalZOrder(UINT32_MAX);

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
                auto ddlb = Label::create();
                ddlb->init(L"SRC A:", TTFFS);
                ddlb->color = LB_INACTIVE;
                auto dd1 = DropDown::create();
                dd1->init(alphaBlending);
                dd1->_callback = [=](DropDown* target) {
                    target->showMenu(hsvcontrol, LEFT, LEFT, Vec2(-0.03, 0));
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
                auto ddlb = Label::create();
                ddlb->init(L"DST A:", TTFFS);
                ddlb->color = LB_INACTIVE;
                auto dd1 = DropDown::create();
                dd1->init(alphaBlending);
                dd1->_callback = [=](DropDown* target) {
                    target->showMenu(hsvcontrol, LEFT, LEFT, Vec2(-0.03, 0));
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
                auto ddlb = Label::create();
                ddlb->init(L"SRC C:", TTFFS);
                ddlb->color = LB_INACTIVE;
                auto dd1 = DropDown::create();
                dd1->init(colorBlending);
                dd1->_callback = [=](DropDown* target) {
                    target->showMenu(hsvcontrol, LEFT, LEFT, Vec2(-0.03, 0));
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
                auto ddlb = Label::create();
                ddlb->init(L"DST C:", TTFFS);
                ddlb->color = LB_INACTIVE;
                auto dd1 = DropDown::create();
                dd1->init(colorBlending);
                dd1->_callback = [=](DropDown* target) {
                    target->showMenu(hsvcontrol, LEFT, LEFT, Vec2(-0.03, 0));
                };
                dd1c->addChild(ddlb);
                dd1c->addChild(dd1);
                options2Cont->addChild(dd1c);
            }

            hsvcontrol->addChild(options2Cont);

            auto dismissCont = Container::create();
            dismissCont->setBorderLayout(BOTTOM, BorderContext::PARENT);
            dismissCont->setBorderLayoutAnchor(BOTTOM);
            dismissCont->setMargin({ 0, 18 });
            hsvcontrol->addChild(dismissCont);
            auto closeB = Button::create();
            closeB->init(L"Hide Panel", TTFFS);
            ((ax::Label*)closeB->field)->setAdditionalKerning(3);
            closeB->_callback = [=](Button* target) {
                container->removeFromParent();
            };
            dismissCont->addChild(closeB);

            auto tabs = Tabs::create(ax::Vec2::ZERO);
            tabs->setConstraint(ContentSizeConstraint(hsv, { -20, 0 }, true, false, true));
            tabs->setBorderLayout(TOP, BorderContext::PARENT);
            tabs->setBorderLayoutAnchor(TOP);
            hsvcontrol->addChild(tabs);
            tabs->addElement(L"Values", colorCont);
            tabs->addElement(L"Custom", savedCont);
            tabs->addElement(L"Contrast");
            tabs->addElement(L"Blend Oper", optionsCont);
            tabs->addElement(L"Blend Func", options2Cont);

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