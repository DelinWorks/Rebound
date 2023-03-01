#include "CatPlayer.h"
#include "shared_scenes/GameUtils.h"
#include "2d/CCTweenFunction.h"

#include "chipmunk/chipmunk_private.h"

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
	scheduleUpdate();

	player_sprite_parent = Node::create();
	player_turn = Node::create();
	//player_turn_dir = Node::create();
	player_sprite = Sprite::createWithSpriteFrameName("player_idle_right_1");
	addChild(player_sprite);
	
	player_sprite_parent->setScale(3.5);
	player_sprite->setScale(3);

	addChild(player_sprite_parent);
	//player_sprite_parent->addChild(player_turn_dir);
	//player_turn_dir->addChild(player_turn);

	auto material = ax::PhysicsMaterial();
	material.density = 0;
	material.restitution = 0;
	material.friction = 0;

	player_body = ax::PhysicsBody::createBox({ 8, 9.1 }, material);
	player_body->setPositionOffset({ 0, -1.25 });
	player_body->setTag(4);
	player_body->setRotationEnable(false);
	player_body->setTag(3);

	//player_turn_hitbox = ax::PhysicsBody::createBox({ 0.1, 3 }, material);
	//player_turn_hitbox->setVelocityLimit(0);
	//player_turn_hitbox->setRotationEnable(false);
	//player_turn_hitbox->setTag(2);
	//player_turn_hitbox->setGroup(4);
	//player_turn_hitbox->setContactTestBitmask(9);

	player_body->getFirstShape()->_cpShapes[0]->filter = cpShapeFilterNew(2, 1, 1);
	//player_turn_hitbox->getFirstShape()->_cpShapes[0]->filter = cpShapeFilterNew(2, 1, 1);

	trail = ax::MotionStreak::create(10, 10, 100, Color3B::BLACK, "streak.png");
	player_sprite->addChild(trail);

	contactor = EventListenerPhysicsContactWithGroup::create(9);
	contactor->onContactBegin = AX_CALLBACK_1(CatPlayer::onContactBegin, this);
	contactor->onContactSeparate = AX_CALLBACK_1(CatPlayer::onContactSeperate, this);

	player_body->setContactTestBitmask(8);

	jumpActionDelay = ax::Sequence::create(
		ax::DelayTime::create(0.05),
		ax::CallFunc::create([&] {
			actionButtonPress = false;
		}),
		_NOTHING
	);
	jumpActionDelay->retain();

	player_sprite_parent->setPhysicsBody(player_body);
	//player_turn->setPhysicsBody(player_turn_hitbox);

	currentAnim = "idle";

	idleRightAnimationFrames.push_back("player_idle_right_1");
	idleRightAnimationFrames.push_back("player_idle_right_2");
	idleRightAnimationFrames.push_back("player_idle_right_3");

	idleLeftAnimationFrames.push_back("player_idle_left_1");
	idleLeftAnimationFrames.push_back("player_idle_left_2");
	idleLeftAnimationFrames.push_back("player_idle_left_3");

	wallTurnAnimationFrames.push_back("player_turn_1");

	jumpAnimationFrames.push_back("player_jump_1");
	jumpAnimationFrames.push_back("player_jump_2");
	jumpAnimationFrames.push_back("player_jump_3");

	rayCastFunc = [&] (PhysicsWorld& world, const PhysicsRayCastInfo& info, void* data) -> bool {
		isHeadBlocked = true;
		return true;
	};

	runAction(ax::Sequence::create(
		ax::DelayTime::create(1),
		ax::CallFunc::create([&] {
			speed = 300;
		}),
		_NOTHING
	));

	return true;
}

CatPlayer::~CatPlayer()
{
	jumpActionDelay->release();
}

void CatPlayer::attachCamera(ax::Camera* camera)
{
	cam = camera;
}

void CatPlayer::update(f32 dt)
{
	if (!rayCastDebug) {
		rayCastDebug = ax::DrawNode::create();
		Director::getInstance()->getRunningScene()->addChild(rayCastDebug);
	}

	rayCastDebug->clear();
	rayCastDebug->drawLine(startVec, endVec, Color4B::RED);

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

		idleAnimCurrentIndex = clampf(idleAnimCurrentIndex, 0, 2.999);

		if (speed > 0)
			player_sprite->setSpriteFrame(idleRightAnimationFrames[floor(idleAnimCurrentIndex)]);
		else
			player_sprite->setSpriteFrame(idleLeftAnimationFrames[floor(idleAnimCurrentIndex)]);
	}
	else if (currentAnim == "wall_turn") {
		idleAnimCurrentIndex += dt * 7;

		if (idleAnimCurrentIndex >= 1)
		{
			changeAnimation("idle");
		}
		else
			player_sprite->setSpriteFrame(wallTurnAnimationFrames[floor(idleAnimCurrentIndex)]);
	}
	else if (currentAnim == "jump") {
		idleAnimCurrentIndex += dt * 7;

		idleAnimCurrentIndex = clampf(idleAnimCurrentIndex, 0, 2.5);

		player_sprite->setSpriteFrame(jumpAnimationFrames[floor(idleAnimCurrentIndex)]);

		if (isOnGround() && idleAnimCurrentIndex > 1)
			changeAnimation("idle", true);
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

	if (keyCode == key::KEY_UP_ARROW || keyCode == key::KEY_SPACE)
	{
		stopAction(jumpActionDelay);
		actionButtonPress = true;
	}
}

void CatPlayer::onKeyReleased(ax::EventKeyboard::KeyCode keyCode)
{
	using key = ax::EventKeyboard::KeyCode;

	if (keyCode == key::KEY_UP_ARROW || keyCode == key::KEY_SPACE)
	{
		stopAction(jumpActionDelay);
		runAction(jumpActionDelay);
	}
}

void CatPlayer::onMouseDown(ax::Event* event)
{
	EventMouse* e = (EventMouse*)event;
	using button = EventMouse::MouseButton;

	if (e->getMouseButton() == button::BUTTON_LEFT)
	{
		stopAction(jumpActionDelay);
		actionButtonPress = true;
	}
}

void CatPlayer::onMouseUp(ax::Event* event)
{
	EventMouse* e = (EventMouse*)event;
	using button = EventMouse::MouseButton;

	if (e->getMouseButton() == button::BUTTON_LEFT)
	{
		stopAction(jumpActionDelay);
		runAction(jumpActionDelay);
	}
}

void CatPlayer::onMouseScroll(ax::Event* event)
{
	EventMouse* e = (EventMouse*)event;
}

void CatPlayer::changeAnimation(std::string_view name, bool enforce)
{
	if (currentAnim == "jump" && !enforce)
		return;
	currentAnim = name;
	idleAnimCurrentIndex = 0;
}

bool CatPlayer::onContactBegin(ax::PhysicsContact& contact)
{
	if (isReceivingInputs)
		onGroundIndex++;
	return true;
}

bool CatPlayer::onContactSeperate(ax::PhysicsContact& contact)
{
	if (isReceivingInputs)
		onGroundIndex--;
	return true;
}

void CatPlayer::physicsTick(ax::PhysicsWorld* world)
{
	float dt = world->deltaTime;

	auto pos = player_sprite_parent->getWorldPosition();

	startVec = Vec2(pos.x, pos.y);
	endVec = Vec2(pos.x, pos.y + 20);

	isHeadBlocked = false;
	world->rayCast(rayCastFunc, cpShapeFilterNew(2, 1, 1), startVec, endVec, nullptr);

	if (cam)
		cam->setPosition(0, 0);

	if (abs(player_body->getVelocity().x) < abs(speed))
	{
		numberOfFlips+=1;
		speed *= -1;
		//player_turn_dir->setScaleX(speed < 0 ? -1 : 1);
		if (actionButtonPress)
			jump(true);
		if (isOnGround())
			changeAnimation("wall_turn");
	}

	//auto worldPos = player_sprite_parent->getWorldPosition();
	//if (worldPos.distanceSquared(lastColliderTogglePos) > 512) {
	//	lastColliderTogglePos = worldPos;
	//	for (auto& wall : Colliders) {
	//		auto distance = worldPos.distanceSquared(wall->getPosition() + wall->contourPos);
	//		if (distance > 1024 * 8 * pow(wall->contourSize, 0.6))
	//			wall->wall_body->setEnabled(false);
	//		else {
	//			wall->wall_body->setEnabled(true);
	//		}
	//	}

	player_sprite->setPositionX(round(player_sprite_parent->getPositionX()));
	player_sprite->setPositionY(ceil(player_sprite_parent->getPositionY()));

	actionRecoveryTime += dt;

	player_body->setVelocity({ speed, player_body->getVelocity().y });

	if (actionButtonPress)
		jump();

	//player_turn->setPosition(4.5, 0);
}

bool CatPlayer::isOnGround()
{
	return onGroundIndex > 0;
}

void CatPlayer::jump(bool ignoreCond)
{
	if (isOnGround() || ignoreCond)
	{
		if (actionRecoveryTime > 0.05f)
		{
			if (!isHeadBlocked) {
				changeAnimation("jump");
				player_body->setVelocity({ player_body->getVelocity().x, 0 });
				player_body->applyImpulse({ 0, 795 });
				actionButtonPress = false;
			}
			actionRecoveryTime = 0;
		}
	}
}
