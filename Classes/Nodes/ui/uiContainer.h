#ifndef __CUSTOM_UI_CONTAINER_H__
#define __CUSTOM_UI_CONTAINER_H__

#include "axmol.h"
#include "custom_ui.h"

#include "Helper/ChangeValue.h"
#include "Helper/short_types.h"
#include "Helper/Math.h"
#include "renderer/backend/Backend.h"

#include "shared_scenes/GameSingleton.h"
#include "shared_scenes/SoundGlobals.h"

#include "Components/UiRescaleComponent.h"

#define ADVANCEDUI_P1_CAP_INSETS Rect(12, 12, 28 - 24, 28 - 24)
#define ADVANCEDUI_TEXTURE "9_slice_box_1.png"sv

namespace CustomUi
{
    enum Layout : u8 {
        NONE = 0,
        FLOW = 1,
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
            FlowLayoutDirection _direction = FlowLayoutDirection::STACK_RIGHT,
            float _spacing = 0, float _margin = 0, i16 _maxNodes = -1)
            : sort(_sort), direction(_direction), spacing(_spacing), margin(_margin), maxNodes(_maxNodes) { }
        FlowLayoutSort sort;
        FlowLayoutDirection direction;
        float spacing;
        float margin;
        i16 maxNodes;

        void build(CustomUi::Container* container);
    };

    class Container : public GUI {
    public:
        Container() : _layout(Layout::NONE), _borderLayout(BorderLayout::CENTER), _flowLayout(), _contentSizeDebug(nullptr) {}
        static CustomUi::Container* create();
        static CustomUi::Container* create(BorderLayout border, BorderContext context = BorderContext::SCREEN_SPACE);
        void setLayout(FlowLayout layout);
        void setBorderLayoutAnchor();

        void notifyLayout() override;

        void calculateContentBoundaries();
        void updateLayoutManagers(bool recursive = false);

        bool _isHitSwallowed = false;
        // should be called every frame, it will update all ui elements to react if mouseLocationInView vector is inside that object on a specific camera and react on hover or hover leave
        bool hover(cocos2d::Vec2 mouseLocationInView, cocos2d::Camera* cam);

        // should be called on onMouseDown or onTouchBegan, it will check on every element and react if mouseLocationInView vector is inside that object on a specific camera and perform a click action or defocus action if outside
        bool click(cocos2d::Vec2 mouseLocationInView, cocos2d::Camera* cam);
        
        bool blockMouse() {
            return _isHitSwallowed;
        }

        bool blockKeyboard() {
            return _isHitSwallowed || _isFocused;
        }

        void setMargin(ax::Vec2 margin) {
            _margin = margin;
        }

        void onEnable();
        void onDisable();

        DrawNode* _contentSizeDebug;
        bool _closestStaticBorder = false;

    protected:
        ax::Vec2 _margin;
        Layout _layout;
        BorderLayout _borderLayout;
        FlowLayout _flowLayout;
    };
}

#endif
