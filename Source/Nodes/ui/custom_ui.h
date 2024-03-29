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

#define DRAW_NODE_DEBUG
//#define SHOW_BUTTON_HITBOX

#define IS_LOCATION_INVALID(L) (L.x == INT16_MAX || L.y == INT16_MAX)
#define INVALID_LOCATION Vec2(INT16_MAX, INT16_MAX)

#define TTFFS CUI::_TTFFontSize

#define BUTTON_HITBOX_CORNER_TOLERANCE Size(3, 3)

#define GET_UI_SCALE_MUL (_UiScaleMul ? _UiScale : 1.0)

#define FULL_HD_NODE_SCALING Vec2(1280, 720) / Vec2(1920, 1080)

namespace CUI
{
    inline float _UiScale = 1; // Dynamically modified within runtime.

    inline float _UiScaleMul = 1;
    inline float _PmtFontScale = 1;
    inline float _PmtFontOutline = 2;
    inline bool _ForceOutline = false;
    inline float _PxArtMultiplier = 2;
    inline float _TTFFontSize = 16;
    inline float _BMFontScale = 2;

    class GUI;

    inline std::stack<GUI*> _modalStack;
    inline GUI* _pCurrentHeldItem = nullptr;
    inline GUI* _pCurrentHoveredItem = nullptr;
    inline GUI* _pCurrentScrollControlItem = nullptr;
    inline GUI* _pCurrentHoveredTooltipItem = nullptr;
    inline ax::backend::ProgramState* _pHoverShader = nullptr;

    inline bool _doNotShowWin32 = false;

    inline std::map<std::string, GUI*> callbackAccess;

    inline ax::backend::ProgramState* _backgroundShader;

    inline ax::Vec2 _currentHeldItemLocationInView;
    inline ax::Vec2 _savedLocationInView;

    inline std::string_view _fontName = "fonts/bitsy.fnt"sv;

    struct UiFontDescriptor {
        std::string fontName;
        float fontSize;
    };

    inline ui::Button* createPlaceholderButton()
    {
        auto button = new ui::Button();
        button->autorelease();
        button->setAnchorPoint(Vec2(0.5, 0.5));
#ifdef SHOW_BUTTON_HITBOX
        button->loadTextureNormal("pixel.png");
        button->setOpacity(120);
#else
        button->setVisible(false);
        button->setOpacity(0);
#endif
        button->setTouchEnabled(false);
        button->setSwallowTouches(false);
        //button->setEnabled(false);
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

        static void DisableDynamicsRecursive(Node* n);

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
        void notifyEnabled(bool _processToggleTree = true);
        virtual void notifyLayout();

        bool setContentSize(const Vec2& size, bool recursive = true);

        virtual V2D getScaledContentSize();

        virtual void onFontScaleUpdate(float scale);

        void updateEnabled(bool state, bool _processToggleTree = true);
        bool isEnabled();
        bool isInternalEnabled();

        void enable(bool show = false);
        void disable(bool hide = false);

        void enableSelf(bool show = false);
        void disableSelf(bool hide = false);

        virtual void onEnable();
        virtual void onDisable();

        void setDynamic() { _isDynamicX = _isDynamicY = true; }
        void setStatic() { _isDynamicX = _isDynamicY = false; }

        void setDynamicX() { _isDynamicX = true; }
        void setStaticX() { _isDynamicX = false; }
        
        void setDynamicY() { _isDynamicY = true; }
        void setStaticY() { _isDynamicY = false; }

        bool isDynamic() { return _isDynamicX && _isDynamicY; };
        bool isDynamicX() { return _isDynamicX; };
        bool isDynamicY() { return _isDynamicY; };

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

        void DenyRescaling(bool evenFlow = false) { _rescalingAllowed = false; _flowLayoutRescalingAllowed = evenFlow; }

        void setForceRawInput(bool force) { _forceRawInput = force; }
        bool isForceRawInput() { return _forceRawInput; }

        void disableArtMul() { _iconArtMulEnabled = false; }

        virtual Size getFitContentSize();

        virtual const Size& getPrefferedContentSize() const;

        virtual void updateInternalObjects();

        void disableProcessToggleTree() { _processEnableTree = false; }
        void enableProcessToggleTree() { _processEnableTree = true; }

        void disableRebuildOnEnter() { _rebuildOnEnter = false; }
        void disableUiScaleMul() { _UiScaleMul = false; }

        std::wstring hoverTooltip;

        Color3B _contentSizeDebugColor = Color3B::MAGENTA;

    protected:
        bool _UiScaleMul = true;
        bool _rebuildOnEnter = true;
        bool _processEnableTree = true;
        bool _isContentSizeDynamic = true;
        bool _actionOnDisable = true;
        float _pretextIconScaling = 2;
        bool _iconArtMulEnabled = true;
        bool _forceRawInput = false;
        bool _rescalingAllowed = true;
        bool _flowLayoutRescalingAllowed = false;
        bool _ignoreDesignScale = false;
        bool _isInternalEnabled = true;
        bool _isEnabledState = true;
        DrawNode* _contentSizeDebug;

        ax::Vec2 _prefferedSize = ax::Vec2::ZERO;
        ax::Vec2 _anchorOffset = ax::Vec2::ONE;
        ax::Vec2 _anchorPoint;
        ax::Vec2 _padding = Vec2::ZERO;
        bool _isContainer = false;
        bool _isDynamicX = false;
        bool _isDynamicY = false;
        bool _containerFlow = false;
        std::set<GUI*> _focusedElements;

        bool _isFocused = false;
        bool _isHovered = false;
        bool _isEnabled = true;

        friend class FlowLayout;
        friend class ContentSizeConstraint;
    };
}

#endif
