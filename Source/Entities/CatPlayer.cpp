#include "CatPlayer.h"

CatPlayer* CatPlayer::createPhysicalEntity()
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
	//{
	//	b2BodyDef bodyDef;
	//	bodyDef.type = b2_dynamicBody;
	//	b2BodyUserData d;
	//	d.pointer = RCAST<uintptr_t>(this);
	//	bodyDef.userData = d;
	//	body = world->CreateBody(&bodyDef);
	//	b2PolygonShape staticBox;
	//	staticBox.SetAsBox(TO_B2_C(16.1), TO_B2_C(10));
	//	b2FixtureDef fixtureDef;
	//	fixtureDef.shape = &staticBox;
	//	fixtureDef.density = 1;
	//	fixtureDef.friction = 0;
	//	fixtureDef.restitution = 0;
	//	b2MassData massData;
	//	massData.center = b2Vec2_zero;
	//	massData.I = 0;
	//	massData.mass = 0;
	//	body->SetGravityScale(5);
	//	body->SetFixedRotation(true);
	//	//body->SetMassData(&massData);
	//	body->CreateFixture(&fixtureDef);
	//	body->SetEnabled(false);
	//}

	body_anchor = Node::create();
	//player_turn_dir = Node::create();
	player_sprite = Sprite::createWithSpriteFrameName("player_idle_1");
	player_shadow_sprite = Sprite::createWithSpriteFrameName("player_idle_1");
	player_shadow_sprite->setColor(Color3B::BLACK);
	player_shadow_sprite->setOpacity(30);
	addChild(body_anchor);
	addChild(player_shadow_sprite);
	addChild(player_sprite);

	jump_particles = ax::ParticleSystemQuad::createWithTotalParticles(100);
	jump_particles->setTexture(Director::getInstance()->getTextureCache()->addImage("pixel.png"));
	jump_particles->setDuration(ParticleSystem::DURATION_INFINITY);
	jump_particles->setGravity({ 0, -2479 });
	jump_particles->setEmitterMode(ParticleSystem::Mode::GRAVITY);
	jump_particles->setStartColor(Color4F(1, 1, 1, 1));
	jump_particles->setEndColor(Color4F(1, 1, 1, 0));
	jump_particles->setSpeed(400);
	jump_particles->setAngle(90);
	jump_particles->setAngleVar(45);
	jump_particles->setLife(0.25);
	jump_particles->setSpawnAngleVar(180);
	jump_particles->setStartSize(5);
	jump_particles->setEndSize(2);
	jump_particles->setEmissionRate(0);
	jump_particles->setEmissionShapes(true);
	jump_particles->setBlendAdditive(true);
	jump_particles->addEmissionShape(ParticleSystem::createRectShape({0, -20}, {24,1}));
	addChild(jump_particles);

	wall_jump_particles = ax::ParticleSystemQuad::createWithTotalParticles(100);
	wall_jump_particles->setTexture(Director::getInstance()->getTextureCache()->addImage("star.png"));
	wall_jump_particles->setDuration(ParticleSystem::DURATION_INFINITY);
	wall_jump_particles->setGravity({ 0, -2479 });
	wall_jump_particles->setEmitterMode(ParticleSystem::Mode::GRAVITY);
	wall_jump_particles->setStartColor(Color4F(1, 1, 1, 0.8));
	wall_jump_particles->setEndColor(Color4F(1, 1, 1, 0.3));
	wall_jump_particles->setSpeed(700);
	wall_jump_particles->setAngleVar(5);
	wall_jump_particles->setLife(0.25);
	wall_jump_particles->setSpawnAngleVar(180);
	wall_jump_particles->setStartSize(20);
	wall_jump_particles->setEndSize(20);
	wall_jump_particles->setEmissionRate(0);
	wall_jump_particles->setEmissionShapes(true);
	wall_jump_particles->setBlendAdditive(false);
	wall_jump_particles->setSpawnScaleIn(1);
	wall_jump_particles->addEmissionShape(ParticleSystem::createRectShape({ 0, 0 }, { 0,20 }));
	addChild(wall_jump_particles);

	//auto material = ax::PhysicsMaterial();
	//material.density = 0;
	//material.restitution = 0;
	//material.friction = 0;

	//physics_body = ax::PhysicsBody::createBox({ 7.8, 7 }, material);
	//physics_body->setPositionOffset({ 0, -5.25 });
	//physics_body->setTag(4);
	//physics_body->setRotationEnable(false);
	//physics_body->setTag(3);

	//player_turn_hitbox = ax::PhysicsBody::createBox({ 0.1, 3 }, material);
	//player_turn_hitbox->setVelocityLimit(0);
	//player_turn_hitbox->setRotationEnable(false);
	//player_turn_hitbox->setTag(2);
	//player_turn_hitbox->setGroup(4);
	//player_turn_hitbox->setContactTestBitmask(9);

	//physics_body->getFirstShape()->_cpShapes[0]->filter = cpShapeFilterNew(2, 1, 1);
	//player_turn_hitbox->getFirstShape()->_cpShapes[0]->filter = cpShapeFilterNew(2, 1, 1);

	trail = ax::MotionStreak::create(10, 10, 100, Color3B::BLACK, "streak.png");
	body_anchor->addChild(trail);

	//physics_body->setContactTestBitmask(8);

	jumpActionDelay = ax::Sequence::create(
		//ax::CallFunc::create([&] {
		//	body->SetLinearVelocity({ body->GetPosition().x, -9999 });
		//}),
		ax::DelayTime::create(.25),
		ax::CallFunc::create([&] {
			actionButtonPress = false;
		}),
		_NOTHING
	);
	jumpActionDelay->retain();

	//body->setPhysicsBody(physics_body);
	//player_turn->setPhysicsBody(player_turn_hitbox);

	currentAnim = "idle";

	idleAnimationFrames.push_back("player_idle_1");
	idleAnimationFrames.push_back("player_idle_2");
	idleAnimationFrames.push_back("player_idle_3");
	idleAnimationFrames.push_back("player_idle_2");

	wallTurnAnimationFrames.push_back("player_turn_1");
	wallTurnAnimationFrames.push_back("player_turn_2");

	jumpAnimationFrames.push_back("player_jump_1");
	jumpAnimationFrames.push_back("player_jump_2");
	jumpAnimationFrames.push_back("player_jump_3");
	
	staggerAnimationFrames.push_back("player_jump_2");
	staggerAnimationFrames.push_back("player_jump_1");

	//rayCastFunc1 = [&] (PhysicsWorld& world, const PhysicsRayCastInfo& info, void* data) -> bool {
	//	if (
	//		~info.shape->getBody()->getTag() & ONE_WAY_COLLISION_INDEX
	//		&& ~info.shape->getBody()->getTag() & DISABLE_TURN_COLLISION_INDEX
	//		&& ~info.shape->getBody()->getTag() & RIGHT_ONLY_COLLISION_INDEX
	//		&& ~info.shape->getBody()->getTag() & LEFT_ONLY_COLLISION_INDEX
	//		)
	//		isHeadBlocked = true;
	//	return false;
	//};

	//rayCastFunc2 = [&](PhysicsWorld& world, const PhysicsRayCastInfo& info, void* data) -> bool {
	//	checkOtherRayCasts[checkOtherRayCastsIndex] = info.contact.y;
	//	if (~info.shape->getBody()->getTag() & OPPOSITE_WAY_COLLISION_INDEX)
	//		didRayCastsHit = true;
	//	else return true;
	//	return false;
	//};

	//rayCastFunc3 = [&](PhysicsWorld& world, const PhysicsRayCastInfo& info, void* data) -> bool {

	//	auto layer = DCAST(ax::FastTMXLayer, info.shape->getBody()->getOwner()->getParent());
	//	if (layer) {
	//		auto prop = layer->getProperty("jump_particle_tint");
	//		if (!prop.isNull()) {
	//			auto color = Color4F(ColorConversion::hex2argb(prop.asString()));
	//			jump_particles->setStartColor(color);
	//			color.a = 0;
	//			jump_particles->setEndColor(color);
	//		}
	//		jump_particles->setBlendAdditive(IS_PROP_NOT_NULL_AND_TRUE(layer, "jump_particle_blend"));
	//	}

	//	if (info.shape->getBody()->getTag() & DISABLE_JUMP_COLLISION_INDEX)
	//		lastCollisionIndex |= DISABLE_JUMP_COLLISION_INDEX;

	//	isPlayerOnGroundRayCast = true;
	//	return false;
	//};

	//runAction(ax::Sequence::create(
	//	ax::DelayTime::create(1),
	//	ax::CallFunc::create([&] { body->SetEnabled(true); }),
	//	ax::CallFunc::create([&] {
	//		addComponent((new LerpPropertyActionComponent(this))
	//		->initFloat(&speed, .3f, .0f, 17.0f, TO_B2_C((300 * tileRatio/* / (0.3429 / 10) * 46*/))));
	//		body->SetLinearVelocity({ speed * playerDirection, body->GetLinearVelocity().y });
	//	}),
	//	ax::DelayTime::create(.3f),
	//	ax::CallFunc::create([&] { isStartUpAnimationDone = true; }),
	//	_NOTHING
	//));

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

void CatPlayer::tick(F32 dt)
{
	if (!getParent())
		return;

	Node::update(dt);

	jump_particles->setPosition(body_anchor->getPosition());
	wall_jump_particles->setPosition(body_anchor->getPosition());

	player_sprite->setScale(3 * playerDirection, 3);
	player_shadow_sprite->setScale(3.33 * playerDirection, 3.33);

	controllerJump.setValue(Rebound::getInstance()->getKeyState(ax::Controller::Key::BUTTON_A).isPressed);
	if (controllerJump.isChanged() && !actionButtonPress)
		actionButtonPress = controllerJump.getValue();

	//if (debugMode) {
	//	if (!debugDrawNode) {
	//		debugDrawNode = DrawNode::create();
	//		Director::getInstance()->getRunningScene()->addChild(debugDrawNode, 99);
	//	}

	//	debugDrawNode->clear();
	//	debugDrawNode->setLineWidth(1);
	//	debugDrawNode->drawLine(startVec1, endVec1, Color4B::GREEN);
	//	debugDrawNode->drawLine(Vec2(startVec2.x + 12, startVec2.y), Vec2(endVec2.x + 12, endVec2.y), Color4B::GREEN);
	//	debugDrawNode->drawLine(Vec2(startVec2.x - 12, startVec2.y), Vec2(endVec2.x - 12, endVec2.y), Color4B::GREEN);
	//	debugDrawNode->drawLine(Vec2(startVec3.x + 14, startVec3.y), Vec2(endVec3.x + 14, endVec3.y), Color4B::RED);
	//	debugDrawNode->drawLine(Vec2(startVec3.x - 14, startVec3.y), Vec2(endVec3.x - 14, endVec3.y), Color4B::RED);

	//	for (int i = 0; i < debugLineTraceY._list.size() - 1; i++) {
	//		auto fuzzyVec2 = Vec2(body_anchor->getPositionX(), FROM_B2_C(debugLineTraceY.at(i).y));
	//		debugDrawNode->drawDot(debugLineTraceY.at(i), 3, fuzzyVec2.fuzzyEquals(body_anchor->getPosition(), 32) ? Color4B::RED : Color4B::BLUE);
	//		debugDrawNode->drawLine(debugLineTraceY.at(i), (debugLineTraceY.at(i + 1)), Color4B::GREEN);
	//	}
	//	debugDrawNode->drawLine(debugLineTraceY.at(debugLineTraceY._list.size() - 1), body_anchor->getPosition(), Color4B::GREEN);
	//	auto fuzzyVec2 = Vec2(body_anchor->getPositionX(), debugLineTraceY.at(debugLineTraceY._list.size() - 1).y);
	//	debugDrawNode->drawDot(debugLineTraceY.at(debugLineTraceY._list.size() - 1), 3, fuzzyVec2.fuzzyEquals(body_anchor->getPosition(), 64) ? Color4B::RED : Color4B::BLUE);
	//}

	//if (!hasTouchedGround && !isOnGround()) {
	//	hasTouchedGround = true;
	//	changeAnimation("falling");
	//}

	if (currentAnim == "idle") {
		idleAnimCurrentIndex += dt * 7;

		if (idleAnimCurrentIndex >= 4)
			idleAnimCurrentIndex = 0;

		player_sprite->setSpriteFrame(idleAnimationFrames[floor(idleAnimCurrentIndex)]);
	}
	else if (currentAnim == "wall_turn") {
		idleAnimCurrentIndex += dt * 10;

		if (idleAnimCurrentIndex >= 2)
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

		//if (isOnGround() && idleAnimCurrentIndex > 1)
		//	changeAnimation("idle", true);
	}
	else if (currentAnim == "stagger") {
		idleAnimCurrentIndex += dt * 7;

		if (idleAnimCurrentIndex >= 2)
		{
			changeAnimation("idle");
		}
		else
			player_sprite->setSpriteFrame(staggerAnimationFrames[floor(idleAnimCurrentIndex)]);
	}
	else if (currentAnim == "falling") {
		player_sprite->setSpriteFrame(jumpAnimationFrames[2]);

		//if (isOnGround() && hasTouchedGround)
		//	changeAnimation("stagger", true);
	}

	//if (isOnGround()) hasTouchedGround = false;

	tickCameraSpace(dt);
}

void CatPlayer::tickCameraSpace(F32 dt)
{
	camWobbleTime += dt;
	Vec2 camPosW = Vec2(camPos.x + std::cos(camWobbleTime * camWobbleSpeed.x) * camWobbleAmount.x,
		camPos.y + std::sin(camWobbleTime * camWobbleSpeed.y) * camWobbleAmount.y);
	Vec2 fCamPos = camPosW + (body_anchor->getPosition() - camPos) * (camDisplaceVector / 100.0);
	bool condX = (body_anchor->getPosition().x > (fCamPos + playerSnapPlane).x + camSnapBorderVector.x
		|| body_anchor->getPosition().x < (fCamPos + playerSnapPlane).x + -camSnapBorderVector.x)
		&& camSnapBorderVector != Vec2::ZERO;
	bool condY = (body_anchor->getPosition().y > (fCamPos + playerSnapPlane).y + camSnapBorderVector.y
		|| body_anchor->getPosition().y < (fCamPos + playerSnapPlane).y + -camSnapBorderVector.y)
		&& camSnapBorderVector != Vec2::ZERO;
	playerSnapPlane = Vec2(condX ? Math::snap(body_anchor->getPositionX(), camSnapPixelVector.x) : playerSnapPlane.x,
		condY ? Math::snap(body_anchor->getPositionY(), camSnapPixelVector.y) : playerSnapPlane.y);
	playerSnapPlaneLerp = Vec2(LERP(playerSnapPlaneLerp.x, playerSnapPlane.x, camSnapLerpVector.x * dt),
		LERP(playerSnapPlaneLerp.y, playerSnapPlane.y, camSnapLerpVector.y * dt));
	cam->setZoom(camZoomValue * tileRatio);
	cam->setPosition(fCamPos + playerSnapPlaneLerp);
}

void CatPlayer::setInputState(bool isReceivingInputs)
{
	Rebound::getInstance()->setupController();
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

	if (keyCode == key::KEY_J)
		teleportPlayer = true;
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

bool CatPlayer::onTouchBegan(cocos2d::Touch* touch, cocos2d::Event* event)
{
	stopAction(jumpActionDelay);
	actionButtonPress = true;

	return true;
}

void CatPlayer::onTouchEnded(cocos2d::Touch* touch, cocos2d::Event* event)
{
	stopAction(jumpActionDelay);
	runAction(jumpActionDelay);
}

void CatPlayer::changeAnimation(std::string_view name, bool enforce)
{
	if (currentAnim == "jump" && !enforce)
		return;
	currentAnim = name;
	idleAnimCurrentIndex = 0;
}

#define DISCARD { return false; }
#define COLLIDE { return true;  }

//bool CatPlayer::onContactBegin(ax::PhysicsContact& contact)
//{
//	ax::Vec2 avg{};
//	auto points = contact.getContactData()->points;
//	for (size_t i = 0; i < 2; i++)
//		contactDebug[i] = points[i];
//
//	Math::getVec2Average(avg, contactDebug, 2);
//	avg.x = body_anchor->getPositionX();
//
//	if (debugMode)
//		debugLineTraceY.push_back(avg);
//
//	if (C_OR_C(ONE_WAY_COLLISION_INDEX))
//	{
//		lastCollisionIndex |= ONE_WAY_COLLISION_INDEX;
//
//		if (avg.fuzzyEquals(body_anchor->getPosition(), 16) && body->GetLinearVelocity().y > 1)
//			DISCARD else if (!avg.fuzzyEquals(body_anchor->getPosition(), 16)) COLLIDE else DISCARD
//	}
//
//	if (C_OR_C(OPPOSITE_WAY_COLLISION_INDEX))
//	{
//		lastCollisionIndex |= OPPOSITE_WAY_COLLISION_INDEX;
//
//		if (avg.fuzzyEquals(body_anchor->getPosition(), 20) && body->GetLinearVelocity().y < -1)
//			DISCARD else COLLIDE
//	}
//
//	if (C_OR_C(RIGHT_ONLY_COLLISION_INDEX) && playerDirection < 0)
//	{
//		lastCollisionIndex |= RIGHT_ONLY_COLLISION_INDEX; DISCARD
//	}
//
//	if (C_OR_C(LEFT_ONLY_COLLISION_INDEX) && playerDirection > 0)
//	{
//		lastCollisionIndex |= LEFT_ONLY_COLLISION_INDEX; DISCARD
//	}
//
//	if (C_OR_C(DISABLE_JUMP_COLLISION_INDEX))
//	{
//		lastCollisionIndex |= DISABLE_JUMP_COLLISION_INDEX; COLLIDE
//	}
//
//	if (C_OR_C(DISABLE_TURN_COLLISION_INDEX))
//	{
//		lastCollisionIndex |= DISABLE_TURN_COLLISION_INDEX;
//
//		if (avg.fuzzyEquals(body_anchor->getPosition(), 16))
//			DISCARD else COLLIDE
//	}
//
//	if (C_OR_C(WALL_JUMP_COLLISION_INDEX))
//	{
//		lastCollisionIndex |= WALL_JUMP_COLLISION_INDEX; COLLIDE
//	}
//
//	COLLIDE
//}
//
//bool CatPlayer::onContactSeperate(ax::PhysicsContact& contact)
//{
//	//if (C_OR_C(OPPOSITE_WAY_COLLISION_INDEX))
//	lastCollisionIndex &= ~DISABLE_JUMP_COLLISION_INDEX;
//	lastCollisionIndex &= ~WALL_JUMP_COLLISION_INDEX;
//	lastCollisionIndex &= ~DISABLE_TURN_COLLISION_INDEX;
//	lastCollisionIndex &= ~RIGHT_ONLY_COLLISION_INDEX;
//	lastCollisionIndex &= ~LEFT_ONLY_COLLISION_INDEX;
//
//	lastValidDirection = playerDirection * -1;
//	lastValidPosition = PTM_B2_2_VEC2(body->GetPosition());
//
//	return true;
//}

//void CatPlayer::physicsPreStep(ReboundPhysicsWorld* world, f32 dt)
//{
//	lastCollisionIndex &= ~ACTION_HAS_JUMPED_THIS_FRAME;
//
//	auto pos = body_anchor->getWorldPosition();
//
//	f32 prevPlayerYSpeed = playerYSpeed;
//	playerYSpeed = pos.y;
//
//	//startVec1 = Vec2(pos.x, pos.y);
//	//endVec1 = Vec2(pos.x, pos.y + 20);
//
//	//startVec2 = Vec2(pos.x, pos.y - 12);
//	//endVec2 = Vec2(pos.x, pos.y - 12 - clampf((prevPlayerYSpeed - playerYSpeed) * 2, 0, 99999));
//
//	startVec3 = Vec2(pos.x, pos.y);
//	endVec3 = Vec2(pos.x, pos.y - 16);
//
//	//if (body->GetLinearVelocity().y < -4000)
//	//{
//	//	playerDirection = lastValidDirection;
//	//	body->SetLinearVelocity(b2Vec2(body->GetLinearVelocity().x, 0));
//	//	body->setPosition(lastValidPosition);
//	//}
//
//	//if (!isOnGround())
//	//{
//	//	checkOtherRayCastsIndex = 0;
//	//	didRayCastsHit = false;
//
//	//	checkOtherRayCasts[0] = pos.y;
//	//	checkOtherRayCasts[1] = pos.y;
//
//	//	world->rayCast(rayCastFunc2, cpShapeFilterNew(2, 1, 1), Vec2(startVec2.x + 12, startVec2.y), Vec2(endVec2.x + 12, endVec2.y), nullptr);
//	//	checkOtherRayCastsIndex++;
//	//	world->rayCast(rayCastFunc2, cpShapeFilterNew(2, 1, 1), Vec2(startVec2.x - 12, startVec2.y), Vec2(endVec2.x - 12, endVec2.y), nullptr);
//
//	//	float contactY = 0;
//	//	if (checkOtherRayCasts[0] < checkOtherRayCasts[1])
//	//		contactY = checkOtherRayCasts[0];
//	//	else contactY = checkOtherRayCasts[1];
//
//	//	if (didRayCastsHit) {
//	//		body_anchor->setPositionY(contactY + 18);
//	//		physics_body->setVelocity({ speed * playerDirection, 0 });
//	//	}
//	//}
//
//	if (teleportPlayer) {
//
//		auto pos = body->GetPosition();
//		b2Vec2 p1 = b2Vec2(pos.x, pos.y);
//		b2Vec2 p2 = b2Vec2(pos.x, pos.y - TO_B2_C(FLT_MAX));
//
//		world->RayCast(&teleportCallback, p1, p2);
//
//		//body->SetLinearVelocity({ body->GetLinearVelocity().x, float(-123123123) });
//
//		teleportPlayer = false;
//	}
//	
//	if (cam)
//		cam->setPosition(0, 0);
//
//	if (abs(body->GetLinearVelocity().x) < 1 && isStartUpAnimationDone)
//		changeAnimation("wall_turn");
//	if (abs(body->GetLinearVelocity().x) == 0 && isStartUpAnimationDone)
//	{
//		numberOfFlips += 1;
//		playerDirection *= -1;
//	}
//	
//	body->SetLinearVelocity({ speed * playerDirection, body->GetLinearVelocity().y });
//
//	actionRecoveryTime += dt;
//
//	isHeadBlocked = false;
//	//world->rayCast(rayCastFunc1, cpShapeFilterNew(2, 1, 1), startVec1, endVec1, nullptr);
//
//	if (actionButtonPress) jump(dt);
//}

//void CatPlayer::physicsPostStep(ReboundPhysicsWorld* world, f32 dt)
//{
//	body_anchor->setPosition(PTM_B2_2_VEC2(body->GetPosition()));
//	body_anchor->setPositionY(body_anchor->getPositionY() + 7);
//
//	player_shadow_sprite->setSpriteFrame(player_sprite->getSpriteFrame());
//	player_shadow_sprite->setPosition(player_sprite->getPosition());
//
//	player_sprite->setPositionX(round(body_anchor->getPositionX()));
//	player_sprite->setPositionY(ceil(body_anchor->getPositionY()));
//}
//
//void CatPlayer::EndContact(b2Contact* contact)
//{
//	//if (contact->GetFixtureB()->GetBody() == body && body->GetLinearVelocity().y < 1)
//	//	teleportPlayer = true;
//}
//
//bool CatPlayer::isOnGround()
//{
//	auto pos = body->GetPosition();
//	b2Vec2 p1 = b2Vec2(pos.x, pos.y);
//	b2Vec2 p2 = b2Vec2(pos.x, pos.y - TO_B2_C(12));
//
//	isPlayerOnGroundRayCast = false;
//	world->RayCast(&jumpCallback, p1, b2Vec2(p2.x + TO_B2_C(16), p2.y));
//	if (!isPlayerOnGroundRayCast)
//		world->RayCast(&jumpCallback, p1, b2Vec2(p2.x - TO_B2_C(16), p2.y));
//	return isPlayerOnGroundRayCast;
//}
//
//void CatPlayer::jump(f32 dt, bool noEffect)
//{
//	if (speed == 0)
//		return;
//
//	if (lastCollisionIndex & RIGHT_ONLY_COLLISION_INDEX)
//		return;
//
//	if (lastCollisionIndex & LEFT_ONLY_COLLISION_INDEX)
//		return;
//
//	if (lastCollisionIndex & DISABLE_JUMP_COLLISION_INDEX)
//		return;
//
//	if (!isOnGround() && ~lastCollisionIndex & WALL_JUMP_COLLISION_INDEX)
//		return;
//
//	if (!isOnGround() && lastCollisionIndex & WALL_JUMP_COLLISION_INDEX) {
//		for (i8 i = 0; i < 2; i++)
//		{
//			wall_jump_particles->setAngle((60 + (playerDirection > 0 ? 60 : 0)) + (i == 0 ? -10 : 10));
//			wall_jump_particles->addParticles(1);
//		}
//		noEffect = true;
//	}
//
//	if (actionRecoveryTime <= 0.05f)
//		return;
//
//	if (!isHeadBlocked) {
//		body->SetLinearVelocity({ body->GetLinearVelocity().x, TO_B2_C(830)});
//	};
//
//	actionButtonPress = false;
//	changeAnimation("jump");
//	if (!noEffect)
//		jump_particles->addParticles(6);
//	actionRecoveryTime = 0;
//}
//
//float RayCastJumpCallback::ReportFixture(b2Fixture* fixture, const b2Vec2& point, const b2Vec2& normal, float fraction)
//{
//	if (fixture->GetBody() != player->body) {
//		player->isPlayerOnGroundRayCast = true;
//		return 0;
//	} else
//		return 1;
//}
//
//float RayCastTeleportCallback::ReportFixture(b2Fixture* fixture, const b2Vec2& point, const b2Vec2& normal, float fraction)
//{
//	//player->body_anchor->stopAllActionsByTag(9);
//	//auto vec = FROM_B2_VEC2(b2Vec2(point.x, point.y + TO_B2_C(17)));
//	//auto seq = Sequence::create(
//	//	CallFunc::create([&] {
//	//		player->isBodyAnimated = true;
//	//		}),
//
//	//	MoveTo::create(0.1, vec),
//
//	//			CallFunc::create([&] {
//	//			player->isBodyAnimated = false;
//	//				}),
//
//	//			_NOTHING
//	//					);
//	//seq->setTag(9);
//	//player->body_anchor->runAction((seq));
//	////player->body->SetTransform(, 0);
//	//player->body->SetLinearVelocity({ player->body->GetLinearVelocity().x, 0 });
//	//return fraction;
//
//	player->body->SetTransform(b2Vec2(point.x, point.y + TO_B2_C(10)), 0);
//	player->body->SetLinearVelocity({ player->body->GetLinearVelocity().x, 0 });
//	return fraction;
//}
