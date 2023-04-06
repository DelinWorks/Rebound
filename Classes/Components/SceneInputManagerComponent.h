#ifndef __H_SCENEINPUTMANAGERCOMPONENT__
#define __H_SCENEINPUTMANAGERCOMPONENT__

#include <axmol.h>
#include "shared_scenes/GameUtils.h"
#include "Nodes/ui/include_ui.h"

using namespace GameUtils;

class SceneInputManagerComponent : public Component {
public:
    cocos2d::EventListenerKeyboard* _keyboardListener;
    cocos2d::EventListenerMouse* _mouseListener;
    cocos2d::EventListenerTouchOneByOne* _touchListener;

    std::function<void(EventKeyboard::KeyCode, Event*)> onKeyPressed;
    std::function<void(EventKeyboard::KeyCode, Event*)> onKeyReleased;
    std::function<void(EventKeyboard::KeyCode, Event*)> onKeyHold;
    std::function<void(EventMouse* event)> onMouseDown;
    std::function<void(EventMouse* event)> onMouseUp;
    std::function<void(EventMouse* event)> onMouseMove;
    std::function<void(EventMouse* event)> onMouseScroll;
    Event* onKeyEvent;
    std::vector<int> _pressedKeys;

    ax::Vec2 _mouseLocation;
    ax::Vec2 _oldMouseLocation;
    ax::Vec2 _newMouseLocation;
    ax::Vec2 _oldMouseLocationOnUpdate;
    ax::Vec2 _newMouseLocationOnUpdate;
    ax::Vec2 _mouseLocationDelta;
    ax::Vec2 _mouseLocationInView;

    ax::Vec2 _mouseLocationInViewNoScene;

    CustomUi::Container* _uiContainer = nullptr;

    SceneInputManagerComponent();

    ~SceneInputManagerComponent();

    void onAdd();

    void update(f32 dt);

    SceneInputManagerComponent* initKeyboard(
        std::function<void(EventKeyboard::KeyCode, Event*)> _onKeyPressed,
        std::function<void(EventKeyboard::KeyCode, Event*)> _onKeyReleased,
        std::function<void(EventKeyboard::KeyCode, Event*)> _onKeyHold
    );

    Camera* getCamera();

    SceneInputManagerComponent* initMouse(
        std::function<void(EventMouse* event)> _onMouseMove,
        std::function<void(EventMouse* event)> _onMouseUp,
        std::function<void(EventMouse* event)> _onMouseDown,
        std::function<void(EventMouse* event)> _onMouseScroll
    );

    SceneInputManagerComponent* initTouchScreenOneByOne(
        std::function<bool(Touch*, Event*)> _onTouchesBegan,
        std::function<void(Touch*, Event*)> _onTouchesMoved,
        std::function<void(Touch*, Event*)> _onTouchesEnded,
        std::function<void(Touch*, Event*)> _onTouchesCancelled
    );

    SceneInputManagerComponent* attachSceneGraph(Node* owner);
};

class SceneInputManager {
public:
    SceneInputManagerComponent* _input;
};

#endif