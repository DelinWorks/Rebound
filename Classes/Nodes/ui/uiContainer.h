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

#define ADVANCEDUI_P1_CAP_INSETS Rect(12, 12, 28 - 24, 28 - 24)
#define ADVANCEDUI_TEXTURE "9_slice_box_1.png"sv

namespace CustomUi
{
    class Container : public GUI {
    public:
        Container() {}
        static CustomUi::Container* create();

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

        void onEnable();
        void onDisable();
    };
}

#endif
