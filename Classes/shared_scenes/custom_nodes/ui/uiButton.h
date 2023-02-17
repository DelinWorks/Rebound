#ifndef __ADVANCED_UI_BUTTON_H__
#define __ADVANCED_UI_BUTTON_H__

#include "cocos2d.h"
#include "uiContainer.h"

using namespace ax;

namespace CustomUi
{
    class Button : public GUI {
    public:
        static CustomUi::Button* create();

        ui::Button* button;
        ui::Scale9Sprite* sprite;
        ui::Text* text;
        cocos2d::Size content_size;
        cocos2d::Size content_anchor;
        std::string normal_sp;
        Color3B selected_color;
        cocos2d::Rect capinsets;
        cocos2d::Color3B normal_color;

        void init(cocos2d::Rect _capinsets, cocos2d::Size _contentsize,
            std::string _normal_sp, Color3B _normal_color = Color3B(117, 179, 255));

        bool update(cocos2d::Vec2 mouseLocationInView, Camera* cam);

        void onEnable();
        void onDisable();

        bool click(cocos2d::Vec2 mouseLocationInView, Camera* cam);
    };
}

#endif
