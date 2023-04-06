#include "SceneInputManagerComponent.h"

SceneInputManagerComponent::SceneInputManagerComponent() : _keyboardListener(nullptr), _mouseListener(nullptr), _touchListener(nullptr), onKeyEvent(nullptr) {
    setName(__func__);
    setEnabled(true);
}

SceneInputManagerComponent::~SceneInputManagerComponent() {
    Director::getInstance()->getScheduler()->unscheduleUpdate(this);

    if (_keyboardListener)
        Director::getInstance()->getEventDispatcher()->removeEventListener(_keyboardListener);
    if (_mouseListener)
        Director::getInstance()->getEventDispatcher()->removeEventListener(_mouseListener);
    if (_touchListener)
        Director::getInstance()->getEventDispatcher()->removeEventListener(_touchListener);

    AX_SAFE_DELETE(_keyboardListener);
    AX_SAFE_DELETE(_mouseListener);
    AX_SAFE_DELETE(_touchListener);
}

void SceneInputManagerComponent::onAdd() {
    AXLOG("Input manager initialized for %s", std::string(_owner->getName()).c_str());
    Director::getInstance()->getScheduler()->scheduleUpdate(this, 0, false);
}

void SceneInputManagerComponent::update(f32 dt) {
    for (auto& keyCode : _pressedKeys)
        onKeyHold((EventKeyboard::KeyCode)keyCode, onKeyEvent);
}

SceneInputManagerComponent* SceneInputManagerComponent::initKeyboard(std::function<void(EventKeyboard::KeyCode, Event*)> _onKeyPressed, std::function<void(EventKeyboard::KeyCode, Event*)> _onKeyReleased, std::function<void(EventKeyboard::KeyCode, Event*)> _onKeyHold) {

#if AX_TARGET_PLATFORM == AX_PLATFORM_WIN32
    _keyboardListener = EventListenerKeyboard::create();

    onKeyPressed = _onKeyPressed;
    onKeyReleased = _onKeyReleased;
    onKeyHold = _onKeyHold;

    auto _onKeyHoldCheck = [&](EventKeyboard::KeyCode keyCode, Event* event) {
        do {
            for (auto& i : _pressedKeys)
                if (i == (i32)keyCode)
                    return;
            _pressedKeys.push_back((i32)keyCode);
        } while (false);

        if ((std::find(_pressedKeys.begin(), _pressedKeys.end(), (int)EventKeyboard::KeyCode::KEY_RIGHT_ALT) != _pressedKeys.end()) && keyCode == EventKeyboard::KeyCode::KEY_ENTER)
        {
            if (!Darkness::getInstance()->gameWindow.isFullscreen)
            {
                Darkness::getInstance()->gameWindow.isFullscreen = true;
                GameUtils::GLFW_SetBorder(glfwGetWin32Window(Darkness::getInstance()->gameWindow.window), 1);
                GLFWmonitor* monitor = glfwGetPrimaryMonitor();
                const GLFWvidmode* mode = glfwGetVideoMode(monitor);
                i32 xpos, ypos;
                Size frameSize = Director::getInstance()->getOpenGLView()->getFrameSize();
                glfwGetWindowPos(Darkness::getInstance()->gameWindow.window, &xpos, &ypos);
                Darkness::getInstance()->gameWindow.lastKnownWindowRect = Rect(xpos, ypos, frameSize.width, frameSize.width / (f32)(16.0 / 9.0));
                Director::getInstance()->getOpenGLView()->setFrameSize(mode->width, mode->height);
                glfwSetWindowSizeLimits(Darkness::getInstance()->gameWindow.window, 640, 360, mode->width, mode->height);
                glfwSetWindowPos(Darkness::getInstance()->gameWindow.window, 0, 0);
                glfwSetWindowSize(Darkness::getInstance()->gameWindow.window, mode->width, mode->height);
                GameUtils::GLFW_ClipCursor(true);
                return;
            }
            else if (Darkness::getInstance()->gameWindow.isFullscreen) {

                Darkness::getInstance()->gameWindow.isFullscreen = false;
                GameUtils::GLFW_SetBorder(glfwGetWin32Window(Darkness::getInstance()->gameWindow.window), 0);
                GLFWmonitor* monitor = glfwGetPrimaryMonitor();
                const GLFWvidmode* mode = glfwGetVideoMode(monitor);
                auto rect = Darkness::getInstance()->gameWindow.lastKnownWindowRect;
                glfwSetWindowMonitor(Darkness::getInstance()->gameWindow.window, nullptr, rect.origin.x, rect.origin.y, rect.size.width, rect.size.height, mode->refreshRate);
            }
        }

        if (!Darkness::getInstance()->gameWindow.isFullscreen)
            GameUtils::GLFW_ClipCursor(false);

        if (keyCode == EventKeyboard::KeyCode::KEY_ESCAPE)
            Darkness::destroyInstance();

        if (keyCode == EventKeyboard::KeyCode::KEY_F2)
            Darkness::restartInstance();

        if (keyCode == EventKeyboard::KeyCode::KEY_F5)
            FMODAudioEngine::destroyInstance();

        if (_uiContainer)
            if (_uiContainer->blockKeyboard())
                return;

        onKeyEvent = event;
        onKeyPressed(keyCode, event);
    };

    auto _onKeyHoldRelease = [&](EventKeyboard::KeyCode keyCode, Event* event) {
        do {
            for (auto& i : _pressedKeys)
                if (i == (i32)keyCode)
                    _pressedKeys.erase(std::remove(_pressedKeys.begin(), _pressedKeys.end(), (i32)keyCode), _pressedKeys.end());
        } while (false);

        if (_uiContainer)
            if (_uiContainer->blockKeyboard())
                return;

        onKeyReleased(keyCode, event);
    };

    _keyboardListener->onKeyPressed = _onKeyHoldCheck;
    _keyboardListener->onKeyReleased = _onKeyHoldRelease;
#endif

    return this;
}

Camera* SceneInputManagerComponent::getCamera() {
    return DCAST(ax::Scene, getOwner())->getDefaultCamera();
}

SceneInputManagerComponent* SceneInputManagerComponent::initMouse(std::function<void(EventMouse* event)> _onMouseMove, std::function<void(EventMouse* event)> _onMouseUp, std::function<void(EventMouse* event)> _onMouseDown, std::function<void(EventMouse* event)> _onMouseScroll) {
#if AX_TARGET_PLATFORM == AX_PLATFORM_WIN32
    _mouseListener = EventListenerMouse::create();

    onMouseDown = _onMouseDown;
    onMouseUp = _onMouseUp;
    onMouseMove = _onMouseMove;
    onMouseScroll = _onMouseScroll;

    auto _onMouseDownCheck = [&](EventMouse* event) {
        EventMouse* e = (EventMouse*)event;
        if (_uiContainer) {
            if (_uiContainer->click(e->getLocationInView(), getCamera())) return;
            if (_uiContainer->blockMouse()) return;
        }

        onMouseDown(event);
    };

    auto _onMouseUpCheck = [&](EventMouse* event) {
        onMouseUp(event);
    };

    auto _onMouseMoveCheck = [&](EventMouse* event) {
        EventMouse* e = (EventMouse*)event;
        _mouseLocationInViewNoScene = e->getLocationInView();
        _mouseLocation = e->getLocation();
        _oldMouseLocation = _newMouseLocation;
        _newMouseLocation = _mouseLocation;
        _mouseLocationDelta = _oldMouseLocation - _newMouseLocation;
        _mouseLocationInView = e->getLocationInView();
        if (_uiContainer)
            if (_uiContainer->blockMouse() || _uiContainer->blockKeyboard()) return;

        onMouseMove(event);
    };

    auto _onMouseScrollCheck = [&](EventMouse* event) {
        if (_uiContainer) if (_uiContainer->blockMouse() || _uiContainer->blockKeyboard()) return;

        onMouseScroll(event);
    };

    _mouseListener->onMouseDown = _onMouseDownCheck;
    _mouseListener->onMouseUp = _onMouseUpCheck;
    _mouseListener->onMouseMove = _onMouseMoveCheck;
    _mouseListener->onMouseScroll = _onMouseScrollCheck;
#endif
    return this;
}

SceneInputManagerComponent* SceneInputManagerComponent::initTouchScreenOneByOne(std::function<bool(Touch*, Event*)> _onTouchesBegan, std::function<void(Touch*, Event*)> _onTouchesMoved, std::function<void(Touch*, Event*)> _onTouchesEnded, std::function<void(Touch*, Event*)> _onTouchesCancelled) {
#ifdef AX_PLATFORM_MOBILE
    _touchListener = EventListenerTouchOneByOne::create();
    _touchListener->onTouchBegan = _onTouchesBegan;
    _touchListener->onTouchMoved = _onTouchesMoved;
    _touchListener->onTouchEnded = _onTouchesEnded;
    _touchListener->onTouchCancelled = _onTouchesCancelled;
#endif
    return this;
}

SceneInputManagerComponent* SceneInputManagerComponent::attachSceneGraph(Node* owner) {

#if AX_TARGET_PLATFORM == AX_PLATFORM_WIN32
    Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(_keyboardListener, owner);
    Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(_mouseListener, owner);
#endif

#ifdef AX_PLATFORM_MOBILE
    Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(_touchListener, owner);
#endif

    return this;
}