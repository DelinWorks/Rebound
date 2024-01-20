#pragma once

#include "axmol.h"
#include "custom_ui.h"

#include "Helper/ChangeValue.h"
#include "Helper/short_types.h"
#include "Helper/Math.h"
#include "renderer/backend/Backend.h"

#include "shared_scenes/GameSingleton.h"
#include "shared_scenes/SoundGlobals.h"

#define ADVANCEDUI_P1_CAP_INSETS Rect(12, 12, 4, 4)
#define ADVANCEDUI_SLIDER_CAP_INSETS Rect(2, 2, 2, 2)
#define ADVANCEDUI_TEXTURE "9_slice_box_1"sv
#define ADVANCEDUI_TEXTURE_INV "9_slice_box_1_inv"sv
#define ADVANCEDUI_TEXTURE_GRAY "9_slice_box_1_gray"sv
#define ADVANCEDUI_TEXTURE_CRAMPED "9_slice_box_1_cramped"sv
#define ADVANCEDUI_TEXTURE_CRAMPED2 "9_slice_box_1_cramped2"sv
#define ADVANCEDUI_TEXTURE_CRAMPED3 "9_slice_box_1_cramped3"sv

#define ANY_ENTER(KC) (KC == EventKeyboard::KeyCode::KEY_ENTER || KC == EventKeyboard::KeyCode::KEY_KP_ENTER)

namespace CUI
{
    class Container;

    struct UserData {
        std::vector<GUI*> l1;
        Container* c1;
        int index;
    };

    struct KeyboardModifierState
    {
        bool isCtrl = false;
        bool isShift = false;
        bool isAlt = false;
    };

    enum Layout : U8 {
        LAYOUT_NONE = 0,
        LAYOUT_FLOW = 1,
    };

    enum Constraint : U8 {
        CONSTRAINT_NONE = 0,
        CONSTRAINT_DEPENDENCY = 1,
        CONSTRAINT_CONTENTSIZE = 2,
    };

    enum FlowLayoutSort : U8 {
        SORT_HORIZONTAL = 0,
        SORT_VERTICAL = 1,
    };

    enum FlowLayoutDirection : U8 {
        STACK_LEFT = 0,
        STACK_CENTER = 1,
        STACK_RIGHT = 2,
        STACK_BOTTOM = 3,
        STACK_TOP = 4,
    };

    class Container;

    class FlowLayout {
    public:
        FlowLayout(FlowLayoutSort _sort = FlowLayoutSort::SORT_HORIZONTAL,
            FlowLayoutDirection _direction = FlowLayoutDirection::STACK_CENTER,
            float _spacing = 0, float _margin = 0, bool _reverseStack = true)
            : sort(_sort), direction(_direction), spacing(_spacing), margin(_margin), reverseStack(_reverseStack), constSize(false), mulSpace(false), constSizeV(V2D::ZERO) { }
        FlowLayoutSort sort;
        FlowLayoutDirection direction;
        float spacing;
        float margin;
        bool reverseStack;
        bool constSize;
        bool mulSpace;
        V2D constSizeV;

        void build(CUI::Container* container);
    };

    class DependencyConstraint {
    public:
        DependencyConstraint(GUI* _parent = nullptr, BorderLayout _position = CENTER, V2D _offset = V2D::ZERO,
            bool _worldPos = false, V2D _worldPosOffset = V2D::ZERO)
            : parent(_parent), position(_position), offset(_offset), worldPos(_worldPos), worldPosOffset(_worldPosOffset) { }
        GUI* parent;
        BorderLayout position;
        V2D offset;
        bool worldPos;
        V2D worldPosOffset;

        void build(CUI::GUI* element);
    };

    class ContentSizeConstraint {
    public:
        ContentSizeConstraint(GUI* _parent = nullptr, const V2D& _offset = V2D::ZERO, bool _scaled = false, bool _lockX = false, bool _lockY = false, const V2D& _minSize = V2D::ZERO)
            : parent(_parent), offset(_offset), scaled(_scaled), lockX(_lockX), lockY(_lockY), minSize(_minSize) { }
        GUI* parent;
        V2D offset;
        bool scaled;
        bool lockX;
        bool lockY;
        V2D minSize;

        void build(CUI::GUI* element);
    };

    enum BgSpriteType {
        BG_NORMAL = 0,
        BG_INVERTED = 1,
        BG_GRAY = 2
    };

    class Container : public GUI {
    public:
        Container();
        void setBorderLayout(BorderLayout border, BorderContext context = BorderContext::SCREEN_SPACE, bool designScaleIgnoring = false);
        
        ~Container();

        static CUI::Container* create();
        static CUI::Container* createDenyScaling();

        void setLayout(FlowLayout layout);
        void setConstraint(DependencyConstraint layout);
        void setConstraint(ContentSizeConstraint layout);

        void setBorderLayoutAnchor(V2D offset = V2D::ONE);
        void setBorderLayoutAnchor(BorderLayout border, V2D offset = V2D::ONE);

        void setBackgroundSprite(V2D padding = {0, 0}, BgSpriteType type = BgSpriteType::BG_NORMAL);
        void setBackgroundSpriteCramped(V2D padding = { 0, 0 }, V2D scale = {1, 1});
        void setBackgroundSpriteCramped2(V2D padding = { 0, 0 }, V2D scale = {1, 1});
        void setBackgroundSpriteCramped3(V2D padding = { 0, 0 }, V2D scale = {1, 1});
        void setBackgroundSpriteDarken(V2D padding = { 0, 0 });
        void setBackgroundDim();

        void setBlocking();
        void setDismissible();
        void setBackgroundBlocking();
        void setElementBlocking();
        void setSelfHover();

        void notifyLayout() override;

        virtual void calculateContentBoundaries();
        virtual void updateLayoutManagers(bool recursive = false);

        V2D getScaledContentSize();

        void onEnter() override;

        bool _isHitSwallowed = false;
        // should be called every frame, it will update all ui elements to react if mouseLocationInView vector is inside that object on a specific camera and react on hover or hover leave
        bool hover(V2D mouseLocationInView, Camera* cam);

        // should be called on onMouseDown or onTouchBegan, it will check on every element and react if mouseLocationInView vector is inside that object on a specific camera and perform a click action or defocus action if outside
        bool press(V2D mouseLocationInView, Camera* cam);
        bool release(V2D mouseLocationInView, Camera* cam);

        void keyPress(EventKeyboard::KeyCode keyCode);
        void keyRelease(EventKeyboard::KeyCode keyCode);

        void mouseScroll(EventMouse* event);
        
        bool blockMouse() {
            return _isHitSwallowed || _pCurrentHeldItem;
        }

        bool blockKeyboard() {
            return _isHitSwallowed || isUiFocused();
        }

        void setMargin(const V2D& margin) { _margin = margin; }
        const V2D& getMargin() { return _margin; }

        void setTightBoundaries(bool isTight) { _isTightBoundaries = isTight; }

        void onEnable();
        void onDisable();

        void addSpecialChild(CUI::GUI* gui);
        CUI::Container* addChildAsContainer(CUI::GUI* gui);

        void recalculateChildDimensions();

        void setBackgroundVisible(bool visible);

        ui::Button* _bgButton = nullptr;
        ax::ui::Scale9Sprite* _background = nullptr;
        ax::LayerColor* _bgDim= nullptr;
        V2D _backgroundPadding = V2D::ZERO;
        bool _closestStaticBorder = false;
        std::vector<CUI::GUI*> _allButtons;

        std::function<void(Container*)> _onContainerLayoutUpdate = [](Container* self) {};

        std::function<void()> _onContainerDismiss = []() {};

        std::function<void(Container*)> _onContainerTabSelected = [](Container* self) {};

        UserData _userData;
        FlowLayout _flowLayout;

    protected:
        bool _isSelfHover = false;
        bool _isElementBlocking = false;
        bool _isBlocking = false;
        bool _isDismissible = false;
        bool _isMinimized = true;
        bool _isTightBoundaries = true;
        V2D _margin;
        Layout _layout;
        Constraint _constraint;
        BorderLayout _borderLayout;
        DependencyConstraint _depConst;
        ContentSizeConstraint _csConst;
    };

    class Separator : public GUI {
    public:
        static Separator* create(V2D size);
    };

    class EventPassClippingNode : public GUI {
    public:
        static EventPassClippingNode* create(Container* _child);
        Container* child;
        ax::ClippingRectangleNode* clip = nullptr;
        void setClipRegion(ax::Rect r);

        virtual bool hover(V2D mouseLocationInView, Camera* cam);
        virtual bool press(V2D mouseLocationInView, Camera* cam);
        virtual bool release(V2D mouseLocationInView, Camera* cam);
        virtual void keyPress(EventKeyboard::KeyCode keyCode);
        virtual void keyRelease(EventKeyboard::KeyCode keyCode);
        virtual void mouseScroll(EventMouse* event);
    };
}
