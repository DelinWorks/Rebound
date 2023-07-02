#include "Player.h"
#include "shared_scenes/GameUtils.h"
#include "2d/TweenFunction.h"

Player* Player::createPlayer()
{
	Player* p = new Player();
	if (p->init())
	{
		p->autorelease();
	}
	else
	{
		AX_SAFE_DELETE(p);
	}
	return p;
}

bool Player::init()
{
	chaseNode = Node::create();
	addChild(chaseNode);

	base = Node::create();
	addChild(base);

	setPosition(0, 0);
	auto sp = ax::Sprite::create();
	SET_TEXTURE(sp, "Player/Player.png", true);
	base->addChild(sp);

	return true;
}

void Player::attachCamera(ax::Camera* camera)
{
	cam = camera;
	auto component1 = new FollowNodeTransformComponent(base, true, false, false);
	auto component2 = new FollowNodeTransformComponent(chaseNode, true, false, false);
	base->addComponent(component2);
	camera->addComponent(component1);
}

void Player::update(f32 dt)
{

	auto joystickX = Darkness::getInstance()->getKeyState(ax::Controller::Key::JOYSTICK_LEFT_X).value;
	auto joystickY = Darkness::getInstance()->getKeyState(ax::Controller::Key::JOYSTICK_LEFT_Y).value;

	if (!isMovingRight && !isMovingLeft && abs(joystickX) < 0.1)
		movementDirection.x = LERP(movementDirection.x, 0, playerMoveStopEase * dt);
	if (!isMovingUp && !isMovingDown && abs(joystickY) < 0.1)
		movementDirection.y = LERP(movementDirection.y, 0, playerMoveStopEase * dt);

	auto triggerRight = (Darkness::getInstance()->getKeyState(ax::Controller::Key::AXIS_RIGHT_TRIGGER).value + 1) / 2.0;
	auto triggerLeft = (Darkness::getInstance()->getKeyState(ax::Controller::Key::AXIS_LEFT_TRIGGER).value + 1) / 2.0;

	if (abs(joystickX) >= 0.1)
		movementDirection.x = LERP(movementDirection.x, clampf(joystickX * 1.5 * tweenfunc::easeIn(abs(joystickX), 1), -1, 1), playerMoveBeginEase * dt);
	if (abs(joystickY) >= 0.1)
	movementDirection.y = LERP(movementDirection.y, clampf(joystickY * -1.5 * tweenfunc::easeIn(abs(joystickY), 1), -1, 1), playerMoveBeginEase * dt);

	curZoom += triggerRight * zoomAmount * dt;
	curZoom -= triggerLeft * zoomAmount * dt;

	if (isMovingRight && !isMovingLeft)
		movementDirection.x = LERP(movementDirection.x, 1.0, playerMoveBeginEase * dt);
	if (isMovingLeft && !isMovingRight)
		movementDirection.x = LERP(movementDirection.x, -1.0, playerMoveBeginEase * dt);
	if (isMovingUp && !isMovingDown)
		movementDirection.y = LERP(movementDirection.y, 1.0, playerMoveBeginEase * dt);
	if (isMovingDown && !isMovingUp)
		movementDirection.y = LERP(movementDirection.y, -1.0, playerMoveBeginEase * dt);

	auto normalized = movementDirection;
	if (movementDirection.length() > 1)
		normalized = movementDirection.getNormalized();

	playerPosX = playerPosX + speed * normalized.x * dt;
	playerPosY = playerPosY + speed * normalized.y * dt;

	chaseNode->setPositionX(round(playerPosX));
	chaseNode->setPositionY(round(playerPosY));

	zoomSnapTimer += dt;

	if (zoomDir != 0)
		zoomSnapTimer = 0;

	if (zoomDir == 0 && curZoom > 0.9 && curZoom < 1.1 && zoomSnapTimer > 1.0)
		curZoom = 1;

	curZoom += (zoomAmount + tweenfunc::quadEaseIn(curZoom)) * zoomDir * dt;
	curZoom = clampf(curZoom, 0.5f, 30);

	if (zoomDir != 0)
		cam->setZoom(LERP(cam->getZoom(), curZoom, 0.1));
	else
		cam->setZoom(LERP(cam->getZoom(), curZoom, 0.05));
	cam->applyZoom();
}

void Player::setInputState(bool isReceivingInputs)
{
	Darkness::getInstance()->setupController();
	this->isReceivingInputs = isReceivingInputs;
}

void Player::onKeyPressed(ax::EventKeyboard::KeyCode keyCode)
{
	using key = ax::EventKeyboard::KeyCode;

	if (keyCode == key::KEY_W)
		isMovingUp = true;
	else if (keyCode == key::KEY_A)
		isMovingLeft = true;
	else if (keyCode == key::KEY_S)
		isMovingDown = true;
	else if (keyCode == key::KEY_D)
		isMovingRight = true;
}

void Player::onKeyReleased(ax::EventKeyboard::KeyCode keyCode)
{
	using key = ax::EventKeyboard::KeyCode;

	if (keyCode == key::KEY_W)
		isMovingUp = false;
	else if (keyCode == key::KEY_A)
		isMovingLeft = false;
	if (keyCode == key::KEY_S)
		isMovingDown = false;
	else if (keyCode == key::KEY_D)
		isMovingRight = false;
}

void Player::onMouseDown(ax::Event* event)
{
	EventMouse* e = (EventMouse*)event;
	using button = EventMouse::MouseButton;

	if (e->getMouseButton() == button::BUTTON_4)
		zoomDir = 1;
	else if (e->getMouseButton() == button::BUTTON_5)
		zoomDir = -1;
}

void Player::onMouseUp(ax::Event* event)
{
	EventMouse* e = (EventMouse*)event;
	using button = EventMouse::MouseButton;

	if (e->getMouseButton() == button::BUTTON_4)
		zoomDir = 0;
	else if (e->getMouseButton() == button::BUTTON_5)
		zoomDir = 0;
}

void Player::onMouseScroll(ax::Event* event)
{
	EventMouse* e = (EventMouse*)event;
	zoomDir = e->getScrollY();
}
