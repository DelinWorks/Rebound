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

        static Container* createFledgedHSVPanel(GameUtils::Editor::ColorChannelManager* manager) {
            auto container = Container::create();
            container->setBorderLayout(LEFT, BorderContext::PARENT);
            container->setBorderLayoutAnchor(LEFT);
            container->setLayout(FlowLayout(SORT_VERTICAL));
            container->setBackgroundSprite();
            container->setBackgroundBlocking();

            auto hsv = HSVWheel::create();
            hsv->channelMgr = manager;
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

            auto optionsCont = Container::create();
            optionsCont->setLayout(FlowLayout(SORT_VERTICAL, STACK_CENTER, 6, 0));
            auto toggle = Toggle::create();
            toggle->hoverTooltip = L"Whether to enable color blending on this color channel.";
            toggle->init(L"Enable Blending ");
            toggle->toggle(true);
            optionsCont->addChild(toggle);
            optionsCont->addChild(Separator::create(Vec2(1, 10)));
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
            optionsCont->addChild(Separator::create(Vec2(1, 10)));

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
            options2Cont->addChild(Separator::create(Vec2(1, 10)));

            hsvcontrol->addChild(options2Cont);

            auto selGroupCont = Container::create();
            selGroupCont->setBorderLayout(BOTTOM_LEFT, BorderContext::PARENT);
            selGroupCont->setBorderLayoutAnchor(BOTTOM_LEFT);
            selGroupCont->setMargin({ 0, 12 });
            auto glb = Label::create();
            glb->init(L"Channel: 0", TTFFS);
            glb->setUiPadding(Vec2(34, 0));
            selGroupCont->addChild(glb);
            hsvcontrol->addChild(selGroupCont, 1);

            auto channelCont = Container::create();
            channelCont->setPositionY(8);

            auto flowCL = FlowLayout(SORT_HORIZONTAL, STACK_CENTER, 0);
            auto ns = GameUtils::getNodeIgnoreDesignScale();
            Vec2 hpadding = { 6, 85 };
            auto channelPageBCont = Container::create();
            channelPageBCont->setLayout(flowCL);
            channelCont->addChild(channelPageBCont);
            auto rightPageB = Button::create();
            rightPageB->initIcon("editor_arrow_right", hpadding);
            auto leftPageB = Button::create();
            leftPageB->initIcon("editor_arrow_left", hpadding);
            channelPageBCont->addChild(rightPageB);
            channelPageBCont->addChild(leftPageB);

            Vec2 rowcol = Vec2(3, 5);

            auto onColorSelect = [=](int i) {
                glb->setString(WFMT(L"Channel: %d", i));
                hsv->currentChannel = i;
                hsv->updateColorValues(manager->getColor(i).color);
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
                fl.constSize = true; fl.constSizeV = Vec2(hsvcontrol->getContentSize().x / ns.x - 40, 1);
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
                container->removeFromParent();
            };
            dismissCont->addChild(closeB);

            auto tabs = Tabs::create(ax::Vec2::ZERO);
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