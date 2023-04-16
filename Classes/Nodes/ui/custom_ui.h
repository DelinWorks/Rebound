#ifndef __CUSTOM_UI_H__
#define __CUSTOM_UI_H__

#include "axmol.h"
#include "Helper/short_types.h"
#include <ui/CocosGUI.h>
#include <ui/UIButton.h>
#include <ui/UITextField.h>

using namespace ax;

namespace CustomUi
{
    //#define SHOW_BUTTON_HITBOX

    inline ui::Button* createPlaceholderButton()
    {
        auto button = ui::Button::create();
#ifdef SHOW_BUTTON_HITBOX
        button->loadTextureNormal("shared/debug/button.png");
        button->setOpacity(120);
#else
        button->setVisible(false);
        button->setOpacity(0);
#endif
        return button;
    }

    inline void hookPlaceholderButtonToNode(Node* node_ref, ui::Button* button_ref, const Size hitbox_scale = Size(0, 0), bool add_hitbox_from_parent = true)
    {
        node_ref->addChild(button_ref);
        button_ref->setPosition(Vec2(node_ref->getBoundingBox().size.width / 2, node_ref->getBoundingBox().size.height / 2));
        button_ref->ignoreContentAdaptWithSize(false);
        if (add_hitbox_from_parent)
            button_ref->setContentSize(node_ref->getContentSize() + hitbox_scale);
        else
            button_ref->setContentSize(hitbox_scale);
    }

    // Inherit Node GUI Manager
    class GUI : public Node {
    public:
        GUI() {}
        virtual ~GUI() {}

        // DO NOT ACCESS, USE AdvancedUiContainer
        virtual bool hover(Vec2 mouseLocationInView, Camera* cam) = 0;
        // DO NOT ACCESS, USE AdvancedUiContainer
        virtual bool click(Vec2 mouseLocationInView, Camera* cam) = 0;

        void onEnter() override;
        void onExit() override;

        void notifyFocused(GUI* sender, bool focused);
        void notifyEnabled();
        virtual void notifyLayout();

        void setContentSize(const Vec2& size) override;

        virtual void onFontScaleUpdate(float scale);

        void updateEnabled(bool state);
        bool isEnabled();
        bool isInternalEnabled();

        void enable();
        void disable();

        virtual void onEnable() = 0;
        virtual void onDisable() = 0;

        void setDynamic() { _isDynamic = true; }
        void setStatic() { _isDynamic = false; }

    private:
        bool _isInternalEnabled = true;
        bool _isEnabledState = true;

    public:
        bool _isDynamic = false;
        bool _isContainer = false;
        std::set<GUI*> _focusedElements;
        bool _isForceFocused = false;
        bool _isFocused = false;
        bool _isHovered = false;
        bool _isEnabled = true;
    };
}

#endif
