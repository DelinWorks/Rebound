#ifndef __CUSTOM_UI_H__
#define __CUSTOM_UI_H__

#include "cocos2d.h"
#include "ui/CocosGUI.h"
#include "Helper/short_types.h"

namespace CustomUi
{
    //#define SHOW_BUTTON_HITBOX

    inline cocos2d::ui::Button* createPlaceholderButton()
    {
        auto button = cocos2d::ui::Button::create();
#ifdef SHOW_BUTTON_HITBOX
        button->loadTextureNormal("shared/debug/button.png");
        button->setOpacity(120);
#else
        button->setOpacity(0);
#endif
        return button;
    }

    inline void hookPlaceholderButtonToNode(cocos2d::Node* node_ref, cocos2d::ui::Button* button_ref, const cocos2d::Size hitbox_scale = cocos2d::Size(0, 0), bool add_hitbox_from_parent = true)
    {
        node_ref->addChild(button_ref);
        button_ref->setPosition(cocos2d::Vec2(node_ref->getBoundingBox().size.width / 2, node_ref->getBoundingBox().size.height / 2));
        button_ref->ignoreContentAdaptWithSize(false);
        if (add_hitbox_from_parent)
            button_ref->setContentSize(node_ref->getContentSize() + hitbox_scale);
        else
            button_ref->setContentSize(hitbox_scale);
    }

    // Inherit Node GUI Manager
    class GUI : public cocos2d::Node {
    public:
        GUI() {}
        virtual ~GUI() {}

        // DO NOT ACCESS, USE AdvancedUiContainer
        virtual bool hover(cocos2d::Vec2 mouseLocationInView, cocos2d::Camera* cam) = 0;
        // DO NOT ACCESS, USE AdvancedUiContainer
        virtual bool click(cocos2d::Vec2 mouseLocationInView, cocos2d::Camera* cam) = 0;

        void onEnter() override;
        void onExit() override;

        void notifyFocused(bool focused);

        void updateEnabled(bool state);
        void notifyEnabled();
        bool isEnabled();
        bool isInternalEnabled();

        void enable();
        void disable();

        virtual void onEnable() = 0;
        virtual void onDisable() = 0;

        void setAsContainer(bool isContainer = true) { _isContainer = isContainer; }

        bool shouldSkipCallback() {
            return _skipCallback;
        }

    private:
        bool _isInternalEnabled = true;
        bool _isEnabledState = true;

    protected:
        bool _skipCallback = false;
        bool _isForceFocused = false;
        bool _isFocused = false;
        bool _isContainer = false;
        bool _isEnabled = true;
        float _hoverShaderTime = 0.0;
    };
}

#endif
