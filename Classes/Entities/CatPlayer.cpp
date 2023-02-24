#include "CatPlayer.h"
#include "shared_scenes/GameUtils.h"
#include "2d/CCTweenFunction.h"

CatPlayer* CatPlayer::createEntity()
{
	CatPlayer* p = new CatPlayer();
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

bool CatPlayer::init()
{
	player_sprite_parent = Node::create();
	player_sprite = Sprite::createWithSpriteFrameName("player_idle_1");

	player_sprite_parent->setScale(3);

	addChild(player_sprite_parent);
	player_sprite_parent->addChild(player_sprite);

	auto material = ax::PhysicsMaterial();
	material.density = 0;
	material.restitution = 0;
	material.friction = 0;

	player_body = ax::PhysicsBody::createBox({ 11.5, 12.15 }, material);
	player_body->setRotationEnable(false);

	player_sprite_parent->setPhysicsBody(player_body);

	currentAnim = "idle";

	idleAnimationFrames.push_back("player_idle_1");
	idleAnimationFrames.push_back("player_idle_2");
	idleAnimationFrames.push_back("player_idle_3");

	return true;
}

void CatPlayer::attachCamera(ax::Camera* camera)
{
	cam = camera;
}

void CatPlayer::update(f32 dt)
{
	if (cam)
		cam->setPosition(0, 0);

	if (abs(player_body->getVelocity().x) < abs(speed)) {
		numberOfFlips++;
		speed *= -1;
		if (actionButtonPress)
			jump(true);
	}

	player_body->setVelocity({ speed, player_body->getVelocity().y});

	if (actionButtonPress)
		jump();

	if (currentAnim == "idle") {
		idleAnimCurrentIndex += idleAnimDir * dt * 7;

		if (idleAnimCurrentIndex >= 3)
		{
			idleAnimDir = -1;
			idleAnimCurrentIndex -= 1;
		}

		if (idleAnimCurrentIndex <= 0)
		{
			idleAnimDir = 1;
			idleAnimCurrentIndex += 1;
		}

		player_sprite->setSpriteFrame(idleAnimationFrames[floor(idleAnimCurrentIndex)]);
	}
}

void CatPlayer::setInputState(bool isReceivingInputs)
{
	Darkness::getInstance()->setupController();
	this->isReceivingInputs = isReceivingInputs;
}

void CatPlayer::onKeyPressed(ax::EventKeyboard::KeyCode keyCode)
{
	using key = ax::EventKeyboard::KeyCode;
}

void CatPlayer::onKeyReleased(ax::EventKeyboard::KeyCode keyCode)
{
	using key = ax::EventKeyboard::KeyCode;
}

void CatPlayer::onMouseDown(ax::Event* event)
{
	EventMouse* e = (EventMouse*)event;
	using button = EventMouse::MouseButton;

	if (e->getMouseButton() == button::BUTTON_LEFT)
		actionButtonPress = true;
}

void CatPlayer::onMouseUp(ax::Event* event)
{
	EventMouse* e = (EventMouse*)event;
	using button = EventMouse::MouseButton;

	if (e->getMouseButton() == button::BUTTON_LEFT)
		actionButtonPress = false;
}

void CatPlayer::onMouseScroll(ax::Event* event)
{
	EventMouse* e = (EventMouse*)event;
}

void CatPlayer::jump(bool ignoreCond)
{
	if (abs(player_body->getVelocity().y) < 1 || ignoreCond)
	{
		player_body->setVelocity({ player_body->getVelocity().x, 0 });
		player_body->applyImpulse({ 0, 800 });
		actionButtonPress = false;
	}
}
