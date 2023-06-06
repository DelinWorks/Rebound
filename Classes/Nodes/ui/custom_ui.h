#ifndef __CUSTOM_UI_H__
#define __CUSTOM_UI_H__

#include "axmol.h"
#include "Helper/short_types.h"
#include "Helper/Logging.hpp"
#include <ui/CocosGUI.h>
#include <ui/UIButton.h>
#include <ui/UISlider.h>
#include <ui/UITextField.h>

#include "Components/UiRescaleComponent.h"

using namespace ax;

#define CONTAINER_FLOW_TAG -6942

#define YOURE_NOT_WELCOME_HERE -69420
#define CONTAINER_CLOSE_TAG -69421
#define GUI_ELEMENT_EXCLUDE -69422

//#define DRAW_NODE_DEBUG
//#define SHOW_BUTTON_HITBOX

#define IS_LOCATION_INVALID(L) (L.x == INT16_MAX || L.y == INT16_MAX)
#define INVALID_LOCATION Vec2(INT16_MAX, INT16_MAX)

#define TTFFS CUI::_TTFFontSize

#define BUTTON_HITBOX_CORNER_TOLERANCE Size(3, 3)

namespace CUI
{
    inline float _UiScale = 1; // Dynamically modified within runtime.

    inline float _UiScaleMul = 1;
    inline float _PmtFontScale = 1;
    inline float _PmtFontOutline = 2;
    inline bool _ForceOutline = false;
    inline float _PxArtMultiplier = 2;
    inline float _TTFFontSize = 16;

    class GUI;

    inline std::stack<GUI*> _modalStack;
    inline GUI* _pCurrentHeldItem = nullptr;
    inline GUI* _pCurrentHoveredItem = nullptr;
    inline ax::backend::ProgramState* _pHoverShader = nullptr;

    inline bool _doNotShowWin32 = false;

    inline std::map<std::string, GUI*> callbackAccess;

    inline ax::backend::ProgramState* _backgroundShader;

    inline ax::Vec2 _savedLocationInView;

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
        button->setTouchEnabled(false);
        button->setSwallowTouches(false);
        button->setEnabled(false);
        button->ignoreContentAdaptWithSize(false);
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

        void setUiOpacity(float opacity);

        // DO NOT ACCESS, USE AdvancedUiContainer
        virtual bool hover(Vec2 mouseLocationInView, Camera* cam);
        // DO NOT ACCESS, USE AdvancedUiContainer
        virtual bool press(Vec2 mouseLocationInView, Camera* cam);
        virtual bool release(Vec2 mouseLocationInView, Camera* cam);

        virtual void keyPress(EventKeyboard::KeyCode keyCode);
        virtual void keyRelease(EventKeyboard::KeyCode keyCode);

        virtual void mouseScroll(EventMouse* event);

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

        virtual Vec2 getScaledContentSize();

        virtual void onFontScaleUpdate(float scale);

        void updateEnabled(bool state);
        bool isEnabled();
        bool isInternalEnabled();

        void enable(bool show = false);
        void disable(bool hide = false);

        virtual void onEnable();
        virtual void onDisable();

        void setDynamic() { _isDynamic = true; }
        void setStatic() { _isDynamic = false; }

        bool isContainerDynamic() { return _isDynamic; };

        void setAsContainer() { _isContainer = true; }
        void setContainerFlow() { _containerFlow = true; }

        const std::set<GUI*>& getFocusSet() { return _focusedElements; }

        void setUiAnchorOffset(const ax::Vec2& anchorOffset) { _anchorOffset = anchorOffset; }
        ax::Vec2 getUiAnchorOffset() { return _anchorOffset; }

        void setUiAnchorPoint(const ax::Vec2& anchorPoint) { _anchorPoint = anchorPoint; }
        ax::Vec2 getUiAnchorPoint() { return _anchorPoint; }

        void setUiPadding(const ax::Vec2& padding) { _padding = padding; }
        ax::Vec2 getUiPadding() { return _padding; }

        void setUiFocused(bool isFocused) { _isFocused = isFocused; }
        bool isUiFocused() { return _isFocused; }

        void setUiHovered(bool isHovered) { _isHovered = isHovered; }
        bool isUiHovered() { return _isHovered; }

        void setUiEnabled(bool isEnabled) { _isEnabled = isEnabled; }
        bool isUiEnabled() { return _isEnabled; }

        void DenyRescaling() { _rescalingAllowed = false; }

        void setForceRawInput(bool force) { _forceRawInput = force; }
        bool isForceRawInput() { return _forceRawInput; }

        void disableArtMul() { _iconArtMulEnabled = false; }

        virtual Size getFitContentSize();

        virtual const Size& getPrefferedContentSize() const;

    protected:
        float _pretextIconScaling = 2;
        bool _iconArtMulEnabled = true;
        bool _forceRawInput = false;
        bool _rescalingAllowed = true;
        bool _isInternalEnabled = true;
        bool _isEnabledState = true;
        DrawNode* _contentSizeDebug;

        ax::Vec2 _prefferedSize = ax::Vec2::ZERO;
        ax::Vec2 _anchorOffset = ax::Vec2::ONE;
        ax::Vec2 _anchorPoint;
        ax::Vec2 _padding = Vec2::ZERO;
        bool _isContainer = false;
        bool _isDynamic = false;
        bool _containerFlow = false;
        std::set<GUI*> _focusedElements;

        bool _isFocused = false;
        bool _isHovered = false;
        bool _isEnabled = true;

        friend class FlowLayout;
    };

    class SignalHandeler {
    public:

        virtual void signal(std::string signal) { handleSignal(signal); };
        virtual void handleSignal(std::string signal) = 0;

        static void signalSceneRoot(std::string signal);
    };
}

#endif
