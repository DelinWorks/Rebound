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

namespace CUI
{
    enum Layout : u8 {
        LAYOUT_NONE = 0,
        LAYOUT_FLOW = 1,
    };

    enum Constraint : u8 {
        CONSTRAINT_NONE = 0,
        CONSTRAINT_DEPENDENCY = 1,
        CONSTRAINT_CONTENTSIZE = 2,
    };

    enum FlowLayoutSort : u8 {
        SORT_HORIZONTAL = 0,
        SORT_VERTICAL = 1,
    };

    enum FlowLayoutDirection : u8 {
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
            : sort(_sort), direction(_direction), spacing(_spacing), margin(_margin), reverseStack(_reverseStack) { }
        FlowLayoutSort sort;
        FlowLayoutDirection direction;
        float spacing;
        float margin;
        bool reverseStack;

        void build(CUI::Container* container);
    };

    class DependencyConstraint {
    public:
        DependencyConstraint(GUI* _parent = nullptr, BorderLayout _position = CENTER, Vec2 _offset = ax::Vec2::ZERO,
            bool _worldPos = false, Vec2 _worldPosOffset = ax::Vec2::ZERO)
            : parent(_parent), position(_position), offset(_offset), worldPos(_worldPos), worldPosOffset(_worldPosOffset) { }
        GUI* parent;
        BorderLayout position;
        Vec2 offset;
        bool worldPos;
        Vec2 worldPosOffset;

        void build(CUI::GUI* element);
    };

    class ContentSizeConstraint {
    public:
        ContentSizeConstraint(GUI* _parent = nullptr, Vec2 _offset = ax::Vec2::ZERO, bool _scale = false, bool _lockX = false, bool _lockY = false)
            : parent(_parent), offset(_offset), scale(_scale), lockX(_lockX), lockY(_lockY) { }
        GUI* parent;
        Vec2 offset;
        bool scale;
        bool lockX;
        bool lockY;

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
        void setBorderLayout(BorderLayout border, BorderContext context = BorderContext::SCREEN_SPACE);
        
        ~Container();

        static CUI::Container* create();

        void setLayout(FlowLayout layout);
        void setConstraint(DependencyConstraint layout);
        void setConstraint(ContentSizeConstraint layout);

        void setBorderLayoutAnchor(ax::Vec2 offset = ax::Vec2::ONE);
        void setBorderLayoutAnchor(BorderLayout border, ax::Vec2 offset = ax::Vec2::ONE);

        void setBackgroundSprite(ax::Vec2 padding = {0, 0}, BgSpriteType type = BgSpriteType::BG_NORMAL);
        void setBackgroundSpriteCramped(ax::Vec2 padding = { 0, 0 }, ax::Vec2 scale = {1, 1});
        void setBackgroundSpriteCramped2(ax::Vec2 padding = { 0, 0 }, ax::Vec2 scale = {1, 1});
        void setBackgroundSpriteCramped3(ax::Vec2 padding = { 0, 0 }, ax::Vec2 scale = {1, 1});
        void setBackgroundSpriteDarken(ax::Vec2 padding = { 0, 0 });
        void setBackgroundDim();

        void setBlocking();
        void setDismissible();
        void setBackgroundBlocking();
        void setElementBlocking();
        void setSelfHover();

        void notifyLayout() override;

        virtual void calculateContentBoundaries();
        virtual void updateLayoutManagers(bool recursive = false);

        Vec2 getScaledContentSize();

        void onEnter() override;

        bool _isHitSwallowed = false;
        // should be called every frame, it will update all ui elements to react if mouseLocationInView vector is inside that object on a specific camera and react on hover or hover leave
        bool hover(cocos2d::Vec2 mouseLocationInView, cocos2d::Camera* cam);

        // should be called on onMouseDown or onTouchBegan, it will check on every element and react if mouseLocationInView vector is inside that object on a specific camera and perform a click action or defocus action if outside
        bool press(cocos2d::Vec2 mouseLocationInView, cocos2d::Camera* cam);
        bool release(cocos2d::Vec2 mouseLocationInView, cocos2d::Camera* cam);

        void keyPress(EventKeyboard::KeyCode keyCode);
        void keyRelease(EventKeyboard::KeyCode keyCode);

        void mouseScroll(EventMouse* event);
        
        bool blockMouse() {
            return _isHitSwallowed || _pCurrentHeldItem;
        }

        bool blockKeyboard() {
            return _isHitSwallowed || isUiFocused();
        }

        void setMargin(const ax::Vec2& margin) { _margin = margin; }
        const Vec2& getMargin() { return _margin; }

        void onEnable();
        void onDisable();

        void addSpecialChild(CUI::GUI* gui);
        CUI::Container* addChildAsContainer(CUI::GUI* gui);

        void recalculateChildDimensions();

        ui::Button* _bgButton = nullptr;
        ax::ui::Scale9Sprite* _background = nullptr;
        ax::LayerColor* _bgDim= nullptr;
        ax::Vec2 _backgroundPadding = ax::Vec2::ZERO;
        bool _closestStaticBorder = false;
        std::vector<CUI::GUI*> _allButtons;

    protected:
        bool _isSelfHover = false;
        bool _isElementBlocking = false;
        bool _isBlocking = false;
        bool _isDismissible = false;
        bool _isMinimized = true;
        ax::Vec2 _margin;
        Layout _layout;
        Constraint _constraint;
        BorderLayout _borderLayout;
        FlowLayout _flowLayout;
        DependencyConstraint _depConst;
        ContentSizeConstraint _csConst;
    };

    class Separator : public GUI {
    public:
        static Separator* create(ax::Vec2 size);
    };

    class EventPassClippingNode : public GUI {
    public:
        static EventPassClippingNode* create(Container* _child);
        Container* child;
        ax::ClippingRectangleNode* clip = nullptr;
        void setClipRegion(ax::Rect r);

        virtual bool hover(Vec2 mouseLocationInView, Camera* cam);
        virtual bool press(Vec2 mouseLocationInView, Camera* cam);
        virtual bool release(Vec2 mouseLocationInView, Camera* cam);
        virtual void keyPress(EventKeyboard::KeyCode keyCode);
        virtual void keyRelease(EventKeyboard::KeyCode keyCode);
        virtual void mouseScroll(EventMouse* event);
    };
}
