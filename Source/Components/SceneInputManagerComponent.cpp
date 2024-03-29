#include "SceneInputManagerComponent.h"

SceneInputManagerComponent::SceneInputManagerComponent() : _keyboardListener(nullptr), _mouseListener(nullptr), _touchListener(nullptr), onKeyEvent(nullptr) {
    setName(__func__);
    setEnabled(true);
    autorelease();
}

SceneInputManagerComponent::~SceneInputManagerComponent() {
    LOG_RELEASE;
}

void SceneInputManagerComponent::onAdd() {
    RLOG("Input manager initialized for {}", _owner->getName());
}

void SceneInputManagerComponent::update(F32 dt) {
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
                if (i == (I32)keyCode)
                    return;
            _pressedKeys.push_back((I32)keyCode);
        } while (false);

        if ((std::find(_pressedKeys.begin(), _pressedKeys.end(), (int)EventKeyboard::KeyCode::KEY_LEFT_ALT) != _pressedKeys.end()) ||
            (std::find(_pressedKeys.begin(), _pressedKeys.end(), (int)EventKeyboard::KeyCode::KEY_RIGHT_ALT) != _pressedKeys.end()))
        {
            if (keyCode == EventKeyboard::KeyCode::KEY_ENTER) {
                if (!Rebound::getInstance()->gameWindow.isFullscreen)
                {
                    Rebound::getInstance()->gameWindow.isFullscreen = true;
                    //GameUtils::GLFW_SetBorder(glfwGetWin32Window(Rebound::getInstance()->gameWindow.window), 1);
                    GLFWmonitor* monitor = glfwGetPrimaryMonitor();
                    const GLFWvidmode* mode = glfwGetVideoMode(monitor);
                    I32 xpos, ypos;
                    Size frameSize = Director::getInstance()->getOpenGLView()->getFrameSize();
                    glfwGetWindowPos(Rebound::getInstance()->gameWindow.window, &xpos, &ypos);
                    Rebound::getInstance()->gameWindow.lastKnownWindowRect = Rect(xpos, ypos, frameSize.width, frameSize.width / (F32)(16.0 / 9.0));
                    Director::getInstance()->getOpenGLView()->setFrameSize(mode->width, mode->height);
                    glfwSetWindowSizeLimits(Rebound::getInstance()->gameWindow.window, 640, 360, mode->width, mode->height);
                    glfwSetWindowPos(Rebound::getInstance()->gameWindow.window, 0, 0);
                    glfwSetWindowSize(Rebound::getInstance()->gameWindow.window, mode->width, mode->height);
                    GameUtils::GLFW_ClipCursor(true);
                    return;
                }
                else if (Rebound::getInstance()->gameWindow.isFullscreen) {

                    Rebound::getInstance()->gameWindow.isFullscreen = false;
                    //GameUtils::GLFW_SetBorder(glfwGetWin32Window(Rebound::getInstance()->gameWindow.window), 0);
                    GLFWmonitor* monitor = glfwGetPrimaryMonitor();
                    const GLFWvidmode* mode = glfwGetVideoMode(monitor);
                    auto rect = Rebound::getInstance()->gameWindow.lastKnownWindowRect;
                    glfwSetWindowMonitor(Rebound::getInstance()->gameWindow.window, nullptr, rect.origin.x, rect.origin.y, rect.size.width, rect.size.height, mode->refreshRate);
                }
            }
            else if (keyCode == EventKeyboard::KeyCode::KEY_EQUAL) {
                Rebound::getInstance()->gameWindow.guiScale += 0.25;
                Rebound::getInstance()->gameWindow.guiScale = clampf(Rebound::getInstance()->gameWindow.guiScale, 0.5, 8);
                Rebound::getInstance()->gameWindow.isScreenSizeDirty = true;
                GameUtils::SignalHandeler::signalSceneRoot("tooltip_gui_scale_advice");
            }
            else if (keyCode == EventKeyboard::KeyCode::KEY_MINUS) {
                Rebound::getInstance()->gameWindow.guiScale -= 0.25;
                Rebound::getInstance()->gameWindow.guiScale = clampf(Rebound::getInstance()->gameWindow.guiScale, 0.5, 8);
                Rebound::getInstance()->gameWindow.isScreenSizeDirty = true;
                GameUtils::SignalHandeler::signalSceneRoot("tooltip_gui_scale_advice");
            }
        }

        if (!Rebound::getInstance()->gameWindow.isFullscreen)
            GameUtils::GLFW_ClipCursor(false);

        //if (keyCode == EventKeyboard::KeyCode::KEY_ESCAPE)
        //    Rebound::destroyInstance();

        if (keyCode == EventKeyboard::KeyCode::KEY_F2)
            Rebound::restartInstance();

        if (keyCode == EventKeyboard::KeyCode::KEY_F5)
            FMODAudioEngine::destroyInstance();

        if (_uiContainer) _uiContainer->keyPress(keyCode);

        if (_uiContainer)
            if (_uiContainer->blockKeyboard())
                return;

        onKeyEvent = event;
        onKeyPressed(keyCode, event);
    };

    auto _onKeyHoldRelease = [&](EventKeyboard::KeyCode keyCode, Event* event) {
        do {
            for (auto& i : _pressedKeys)
                if (i == (I32)keyCode)
                    _pressedKeys.erase(std::remove(_pressedKeys.begin(), _pressedKeys.end(), (I32)keyCode), _pressedKeys.end());
        } while (false);

        if (_uiContainer) _uiContainer->keyRelease(keyCode);

        //if (_uiContainer)
        //    if (_uiContainer->blockKeyboard())
        //        return;

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

        if (e->getMouseButton() != EventMouse::MouseButton::BUTTON_MIDDLE)
            Rebound::getInstance()->setCursorHand();
        else
            Rebound::getInstance()->setCursorHold();

        if (_uiContainer) {
            if ((e->getMouseButton() == EventMouse::MouseButton::BUTTON_LEFT || _uiContainer->isUiFocused()))
                if (_uiContainer->press(e->getLocationInView(), getCamera())) return;
            if (_uiContainer->blockMouse()) return;
        }

        onMouseDown(event);
    };

    auto _onMouseUpCheck = [&](EventMouse* event) {
        EventMouse* e = (EventMouse*)event;

        Rebound::getInstance()->setCursorNormal();

        if (_uiContainer && e->getMouseButton() == EventMouse::MouseButton::BUTTON_LEFT) {
            if (_uiContainer->release(e->getLocationInView(), getCamera())) return;
            //if (_uiContainer->blockMouse()) return;
        }

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
        CUI::_savedLocationInView = _mouseLocationInView;

        if (CUI::_pCurrentHeldItem && CUI::_pCurrentScrollControlItem && !_mouseLocationInView.fuzzyEquals(CUI::_currentHeldItemLocationInView, 5)) {
            CUI::_pCurrentHeldItem->release(INVALID_LOCATION, getCamera());
            CUI::_pCurrentHeldItem = nullptr;
        }

        //if (CUI::_pCurrentHoveredItem)
        //    Rebound::getInstance()->setCursorHand();
        //else
        //    Rebound::getInstance()->setCursorNormal();

        //if (_uiContainer)
        //    if (_uiContainer->blockMouse()) return;

        onMouseMove(event);
    };

    auto _onMouseScrollCheck = [&](EventMouse* event) {
        if (_uiContainer) {
            _uiContainer->mouseScroll(event);
            if (_uiContainer->blockMouse() || _uiContainer->blockKeyboard()) return;
        }

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

CUI::Container* SceneInputManager::getContainer()
{
    return _input->_uiContainer;
}
