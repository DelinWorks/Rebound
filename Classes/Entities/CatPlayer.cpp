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

	player_body = ax::PhysicsBody::createBox({ 7.8, 7 }, material);
	player_body->setPositionOffset({ 0, -5.25 });
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
	player_sprite_parent->addChild(trail);

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

	rayCastFunc1 = [&] (PhysicsWorld& world, const PhysicsRayCastInfo& info, void* data) -> bool {
		if (~info.shape->getBody()->getTag() & ONE_WAY_COLLISION_INDEX)
			isHeadBlocked = true;
		return false;
	};

	rayCastFunc2 = [&](PhysicsWorld& world, const PhysicsRayCastInfo& info, void* data) -> bool {
		checkOtherRayCasts[checkOtherRayCastsIndex] = info.contact.y;
		if (~info.shape->getBody()->getTag() & OPPOSITE_WAY_COLLISION_INDEX)
			didRayCastsHit = true;
		else return true;
		return false;
	};

	runAction(ax::Sequence::create(
		ax::DelayTime::create(1),
		ax::CallFunc::create([&] {
			addComponent((new GameUtils::CocosExt::CustomComponents::LerpPropertyActionComponent(this))
			->initFloat(&speed, .3f, .0f, 300.0f, 300.0f));
			player_body->setVelocity({ speed * playerDirection, player_body->getVelocity().y });
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
	Node::update(dt);

	cam->setPosition(camPosX, camPosY);

	//if (!rayCastDebug) {
	//	rayCastDebug = ax::DrawNode::create();
	//	Director::getInstance()->getRunningScene()->addChild(rayCastDebug, 99);
	//}

	//rayCastDebug->clear();
	//rayCastDebug->drawLine(startVec1, endVec1, Color4B::GREEN);
	//rayCastDebug->drawLine(Vec2(startVec2.x + 12, startVec2.y), Vec2(endVec2.x + 12, endVec2.y), Color4B::GREEN);
	//rayCastDebug->drawLine(Vec2(startVec2.x - 12, startVec2.y), Vec2(endVec2.x - 12, endVec2.y), Color4B::GREEN);

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

		if (playerDirection > 0)
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

	//if (keyCode == key::KEY_J)
	//	teleportPlayer = true;
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

	if (C_OR_C(ONE_WAY_COLLISION_INDEX) && player_body->getVelocity().y > 1)
	{
		lastCollisionIndex |= ONE_WAY_COLLISION_INDEX;
		return false;
	}

	if (C_OR_C(OPPOSITE_WAY_COLLISION_INDEX) && player_body->getVelocity().y < -1)
	{
		lastCollisionIndex |= OPPOSITE_WAY_COLLISION_INDEX;
		return false;
	}

	if (C_OR_C(DISABLE_TURN_COLLISION_INDEX) && player_body->getVelocity().y > -1)
	{
		lastCollisionIndex |= DISABLE_TURN_COLLISION_INDEX;
		return false;
	}

	if (C_OR_C(RIGHT_ONLY_COLLISION_INDEX) && playerDirection < 0)
	{
		lastCollisionIndex |= RIGHT_ONLY_COLLISION_INDEX;
		return false;
	}

	if (C_OR_C(LEFT_ONLY_COLLISION_INDEX) && playerDirection > 0)
	{
		lastCollisionIndex |= LEFT_ONLY_COLLISION_INDEX;
		return false;
	}

	if (C_OR_C(WALL_JUMP_COLLISION_INDEX))
	{
		lastCollisionIndex |= WALL_JUMP_COLLISION_INDEX;
		return true;
	}

	if (C_OR_C(DISABLE_JUMP_COLLISION_INDEX))
	{
		lastCollisionIndex |= DISABLE_JUMP_COLLISION_INDEX;
		return true;
	}

	lastCollisionIndex = 1;

	return true;
}

bool CatPlayer::onContactSeperate(ax::PhysicsContact& contact)
{
	lastCollisionIndex &= ~DISABLE_JUMP_COLLISION_INDEX;
	lastCollisionIndex &= ~DISABLE_TURN_COLLISION_INDEX;
	lastCollisionIndex &= ~RIGHT_ONLY_COLLISION_INDEX;
	lastCollisionIndex &= ~LEFT_ONLY_COLLISION_INDEX;

	if (isReceivingInputs)
		onGroundIndex--;
	return true;
}

void CatPlayer::physicsPreTick(ax::PhysicsWorld* world)
{
	float dt = world->deltaTime;

	auto pos = player_sprite_parent->getWorldPosition();

	f32 prevPlayerYSpeed = playerYSpeed;
	playerYSpeed = pos.y;

	startVec1 = Vec2(pos.x, pos.y);
	endVec1 = Vec2(pos.x, pos.y + 20);

	startVec2 = Vec2(pos.x, pos.y - 12);
	endVec2 = Vec2(pos.x, pos.y - 12 - clampf((prevPlayerYSpeed - playerYSpeed) * 2, 0, 99999));

	isHeadBlocked = false;
	world->rayCast(rayCastFunc1, cpShapeFilterNew(2, 1, 1), startVec1, endVec1, nullptr);

	if (!isOnGround())
	{
		checkOtherRayCastsIndex = 0;
		didRayCastsHit = false;

		checkOtherRayCasts[0] = pos.y;
		checkOtherRayCasts[1] = pos.y;

		world->rayCast(rayCastFunc2, cpShapeFilterNew(2, 1, 1), Vec2(startVec2.x + 12, startVec2.y), Vec2(endVec2.x + 12, endVec2.y), nullptr);
		checkOtherRayCastsIndex++;
		world->rayCast(rayCastFunc2, cpShapeFilterNew(2, 1, 1), Vec2(startVec2.x - 12, startVec2.y), Vec2(endVec2.x - 12, endVec2.y), nullptr);

		float contactY = 0;
		if (checkOtherRayCasts[0] < checkOtherRayCasts[1])
			contactY = checkOtherRayCasts[0];
		else contactY = checkOtherRayCasts[1];

		if (didRayCastsHit) {
			player_sprite_parent->setPositionY(contactY + 18);
			player_body->setVelocity({ speed * playerDirection, 0 });
		}
	}

	if (cam)
		cam->setPosition(0, 0);

	if (abs(player_body->getVelocity().x) < 1 && speed >= 300)
	{
		numberOfFlips += 1;
		playerDirection *= -1;
		changeAnimation("wall_turn");
	}
	
	player_body->setVelocity({ speed * playerDirection, player_body->getVelocity().y });

	actionRecoveryTime += dt;

	if (actionButtonPress)
		jump();
}

void CatPlayer::physicsPostTick(ax::PhysicsWorld* world)
{
	player_sprite->setPositionX(round(player_sprite_parent->getPositionX()));
	player_sprite->setPositionY(ceil(player_sprite_parent->getPositionY()));
}

bool CatPlayer::isOnGround()
{
	return onGroundIndex > 0;
}

void CatPlayer::jump()
{
	if ((lastCollisionIndex != 1 && ~lastCollisionIndex & WALL_JUMP_COLLISION_INDEX) || speed < 1)
		return;

	if (lastCollisionIndex & DISABLE_JUMP_COLLISION_INDEX)
		return;

	if (!isOnGround())
		return;

	if (abs(player_body->getVelocity().y) > 1 && ~lastCollisionIndex & WALL_JUMP_COLLISION_INDEX)
		return;

	if (actionRecoveryTime <= 0.05f)
		return;

	if (!isHeadBlocked) {
		player_body->setVelocity({ player_body->getVelocity().x, 0 });
		player_body->applyImpulse({ 0, 797 });
	};

	actionButtonPress = false;
	changeAnimation("jump");
	actionRecoveryTime = 0;
}
