#include "MapEditor.h"

USING_NS_CC;
using namespace backend;

using namespace GameUtils;

void MapEditor::onKeyHold(ax::EventKeyboard::KeyCode keyCode, ax::Event* event)
{}

void MapEditor::onKeyPressed(EventKeyboard::KeyCode keyCode, Event* event)
{
    if (keyCode == EventKeyboard::KeyCode::KEY_1) map->bindLayer(0);
    if (keyCode == EventKeyboard::KeyCode::KEY_2) map->bindLayer(1);
    if (keyCode == EventKeyboard::KeyCode::KEY_3) map->bindLayer(2);

    if (keyCode == EventKeyboard::KeyCode::KEY_W) editorWASDCamMoveRect.origin.y = 1;
    if (keyCode == EventKeyboard::KeyCode::KEY_A) editorWASDCamMoveRect.origin.x = -1;
    if (keyCode == EventKeyboard::KeyCode::KEY_S) editorWASDCamMoveRect.size.y = -1;
    if (keyCode == EventKeyboard::KeyCode::KEY_D) editorWASDCamMoveRect.size.x = 1;

    if (keyCode == EventKeyboard::KeyCode::KEY_CTRL) { isCtrlPressed = true; }
    if (keyCode == EventKeyboard::KeyCode::KEY_SHIFT) { isShiftPressed = true; }

    if (keyCode == EventKeyboard::KeyCode::KEY_Z && isCtrlPressed) editorUndo();

    if (keyCode == EventKeyboard::KeyCode::KEY_Y && isCtrlPressed) editorRedo();

    if (keyCode == EventKeyboard::KeyCode::KEY_LEFT_ALT)
    {
        isEditorDragging = true;
    }

    if (keyCode == EventKeyboard::KeyCode::KEY_T)
    {
        if (getContainer()) {
            auto dis = CUI::DiscardPanel::create(CENTER, PARENT);
            dis->init(L"> Set Layer Name <", L"Layer Name", CUI::OKAY);
            getContainer()->pushModal(dis);
        }
    }

    if (keyCode == EventKeyboard::KeyCode::KEY_E)
        tileRot90->_callback(nullptr);

    if (keyCode == EventKeyboard::KeyCode::KEY_H)
        tileFlipH->_callback(nullptr);

    if (keyCode == EventKeyboard::KeyCode::KEY_V)
        tileFlipV->_callback(nullptr);
    //
    //#ifdef WIN32
    //    if (keyCode == EventKeyboard::KeyCode::KEY_T)
    //    {
    //        auto s = getSingleFileDialog(glfwGetWin32Window(Darkness::getInstance()->gameWindow.window));
    //        RLOG("Texture Path: {}", Strings::narrow(s));
    //    }
    //#endif
}

void MapEditor::onKeyReleased(EventKeyboard::KeyCode keyCode, Event* event)
{
    if (keyCode == EventKeyboard::KeyCode::KEY_CTRL) isCtrlPressed = false;
    if (keyCode == EventKeyboard::KeyCode::KEY_SHIFT) isShiftPressed = false;

    if (keyCode == EventKeyboard::KeyCode::KEY_LEFT_ALT) isEditorDragging = false;

    if (keyCode == EventKeyboard::KeyCode::KEY_W) editorWASDCamMoveRect.origin.y = 0;
    if (keyCode == EventKeyboard::KeyCode::KEY_A) editorWASDCamMoveRect.origin.x = 0;
    if (keyCode == EventKeyboard::KeyCode::KEY_S) editorWASDCamMoveRect.size.y = 0;
    if (keyCode == EventKeyboard::KeyCode::KEY_D) editorWASDCamMoveRect.size.x = 0;
}

void MapEditor::onMouseDown(ax::Event* event)
{
    EventMouse* e = (EventMouse*)event;

    if (e->getMouseButton() == EventMouse::MouseButton::BUTTON_MIDDLE)
    {
        isEditorDragging = true;
    }

    if (e->getMouseButton() == EventMouse::MouseButton::BUTTON_LEFT)
    {
        if (selectionNode->isVisible()) {
            editorPushUndoState();
            selectionPosition = Vec2(INFINITY, INFINITY);
            isPlacing = true;
        }
        //auto mouseClick = DrawNode::create(1);
        //mouseClick->setPosition(Vec2(_input->_mouseLocation.x - (visibleSize.x / 2), (_input->_mouseLocation.y + (visibleSize.y / -2)) * -1));
        //mouseClick->addComponent(new DrawNodeCircleExpandComponent(.5, 80, 16));
        //DESTROY(mouseClick, .5);
        //uiNode->addChild(mouseClick, 999);
    }
    if (e->getMouseButton() == EventMouse::MouseButton::BUTTON_RIGHT)
    {
        if (selectionNode->isVisible()) {
            editorPushUndoState();
            isRemoving = true;
            removeSelectionStartPos = convertFromScreenToSpace(_input->_mouseLocation, _camera, true);
        }
    }
}

void MapEditor::onMouseUp(ax::Event* event)
{
    EventMouse* e = (EventMouse*)event;

    if (e->getMouseButton() == EventMouse::MouseButton::BUTTON_MIDDLE)
    {
        isEditorDragging = false;
    }
    if (e->getMouseButton() == EventMouse::MouseButton::BUTTON_LEFT)
    {
        if (isPlacing) {
            isPlacing = false;
        }
    }
    if (e->getMouseButton() == EventMouse::MouseButton::BUTTON_RIGHT)
    {
        if (!isRemoving) return;
        isRemoving = false;
        selectionPosition = Vec2(INFINITY, INFINITY);
        Rect rect = createEditToolSelectionBox(removeSelectionStartPos, convertFromScreenToSpace(_input->_mouseLocation, _camera, true), map->_tileSize.x);
        editUpdate_place(rect.origin.x, rect.origin.y, rect.size.width, rect.size.height);
        RLOG("remove_selection_tool: begin: {},{} end: {},{}", rect.origin.x, rect.origin.y, rect.size.width, rect.size.height);
        removeSelectionNode->clear();
    }
}

void MapEditor::onMouseMove(ax::Event* event)
{
    Vec2 worldPos = GameUtils::convertFromScreenToSpace(_input->_mouseLocationInView, _camera);
    isSelectableHovered = false;
    for (auto& _ : _selectables) {
        isSelectableHovered = _->editorDraw(worldPos) || isSelectableHovered;
    }

    if (!hasMouseMoved) { hasMouseMoved = true; return; }
    if (isEditorDragging)
    {
        cameraLocation->setPositionX(cameraLocation->getPositionX() + (_input->_mouseLocationDelta.x * _camera->getScale()));
        cameraLocation->setPositionY(cameraLocation->getPositionY() + (_input->_mouseLocationDelta.y * -1 * _camera->getScale()));
    }
    //CCLOG("%f,%f", cameraLocation->getPositionX(), cameraLocation->getPositionY());
}

void MapEditor::onMouseScroll(ax::Event* event)
{
    EventMouse* e = (EventMouse*)event;

    if (isCtrlPressed) {
        if (isShiftPressed)
            cameraLocation->setPositionX(cameraLocation->getPositionX() + e->getScrollY() * 50 * _camera->getScale());
        else
            cameraLocation->setPositionY(cameraLocation->getPositionY() + e->getScrollY() * -50 * _camera->getScale());
    }
    else setCameraScaleIndex(e->getScrollY(), !isShiftPressed);

    if (pdb != nullptr)
    {
        i32 result = 0;
        result = result SQLITE_RESULT_CHECK sqlite3_exec(pdb, std::string("UPDATE EditorMetaData SET store_value='" + std::to_string(cameraScaleIndex) + "' WHERE store_key='editor_camera_scale_index';").c_str(), NULL, NULL, NULL) : result;
        SQLITE_CRASH_CHECK(result);
    }
}

bool MapEditor::onTouchBegan(ax::Touch* touch, ax::Event* event)
{
    isEditorDragging = true;
    isTouchNew = true;
    return true;
}

void MapEditor::onTouchMoved(ax::Touch* touch, ax::Event* event)
{
    //mouseLocation.x = touch->getLocation().x;
    //mouseLocation.y = touch->getLocation().y * -1;
    //oldMouseLocation = newMouseLocation;
    //newMouseLocation = mouseLocation;
    //if (isTouchNew)
    //{
    //    oldMouseLocation = newMouseLocation;
    //    isTouchNew = false;
    //}
    //mouseLocationDelta = oldMouseLocation - newMouseLocation;
    //if (isEditorDragging)
    //{
    //    cameraLocation->setPositionX(cameraLocation->getPositionX() + (mouseLocationDelta.x * _camera->getScale()));
    //    cameraLocation->setPositionX(cameraLocation->getPositionY() + (mouseLocationDelta.y * -1 * _camera->getScale()));
    //}
    //_input->_mouseLocationInView = touch->getLocation();
}

void MapEditor::onTouchEnded(ax::Touch* touch, ax::Event* event)
{
    isEditorDragging = true;
    cameraLocation->setPosition(_camera->getPosition());
}

void MapEditor::onTouchCancelled(ax::Touch* touch, ax::Event* event)
{
}
