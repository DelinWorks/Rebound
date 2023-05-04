#ifndef __CUSTOM_UI_H__
#define __CUSTOM_UI_H__

#include "axmol.h"
#include "Helper/short_types.h"
#include "Helper/Logging.hpp"
#include <ui/CocosGUI.h>
#include <ui/UIButton.h>
#include <ui/UITextField.h>

using namespace ax;

#define CONTAINER_FLOW_TAG -6942

#define YOURE_NOT_WELCOME_HERE -69420
#define CONTAINER_CLOSE_TAG -69421

//#define DRAW_NODE_DEBUG
//#define SHOW_BUTTON_HITBOX

namespace CustomUi
{
    inline float _UiScale = 1; // Dynamically modified within runtime.

    inline float _UiScaleMul = 1;
    inline float _PmtFontScale = 1;
    inline float _PxArtMultiplier = 2;
    
    class GUI;

    inline std::stack<GUI*> _modalStack;
    inline GUI* _pCurrentHeldItem = nullptr;

    inline bool _doNotShowWin32 = false;

    struct UiFontDescriptor {
        std::string fontName;
        float fontSize;
    };

    inline ui::Button* createPlaceholderButton()
    {
        auto button = ui::Button::create();
#ifdef SHOW_BUTTON_HITBOX
        button->loadTextureNormal("pixel.png");
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
        GUI();
        ~GUI();

        // DO NOT ACCESS, USE AdvancedUiContainer
        virtual bool hover(Vec2 mouseLocationInView, Camera* cam) = 0;
        // DO NOT ACCESS, USE AdvancedUiContainer
        virtual bool press(Vec2 mouseLocationInView, Camera* cam) = 0;
        virtual bool release(Vec2 mouseLocationInView, Camera* cam) = 0;

        virtual void keyPress(EventKeyboard::KeyCode keyCode);
        virtual void keyRelease(EventKeyboard::KeyCode keyCode);

        // This differes from addChild because it adds the passed 
        // child in the very top and enables sorting, like a stack.
        void pushModal(GUI* child);

        void onEnter() override;
        void onExit() override;

        virtual void setAnchorPoint(const ax::Vec2& anchor) override;
        void setAnchorOffset(const ax::Vec2& anchorOffset);

        void notifyFocused(GUI* sender, bool focused, bool ignoreSelf = false);
        void notifyEnabled();
        virtual void notifyLayout();

        void setContentSize(const Vec2& size, bool recursive = true);

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
        DrawNode* _contentSizeDebug;

    public:
        ax::Vec2 _anchorOffset = ax::Vec2::ONE;
        ax::Vec2 _anchorPoint;
        bool _containerFlow = false;
        bool _sortChildren = false;
        ax::Vec2 _padding = Vec2::ZERO;
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
