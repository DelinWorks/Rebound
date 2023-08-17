#ifndef __H_CATPLAYER__
#define __H_CATPLAYER__

#include <axmol.h>
#include "shared_scenes/ProtectedTypes.hpp"
#include "Helper/ChangeValue.h"
#include "shared_scenes/GameUtils.h"
#include "2d/TweenFunction.h"
#include "Components/Components.h"

#define WALL_JUMP_COLLISION_INDEX			(1 << 4)
#define ONE_WAY_COLLISION_INDEX				(1 << 5)
#define OPPOSITE_WAY_COLLISION_INDEX		(1 << 6)
#define RIGHT_ONLY_COLLISION_INDEX			(1 << 7)
#define LEFT_ONLY_COLLISION_INDEX			(1 << 8)
#define DISABLE_JUMP_COLLISION_INDEX		(1 << 9)
#define DISABLE_TURN_COLLISION_INDEX		(1 << 10)
#define ACTION_HAS_JUMPED_THIS_FRAME		(1 << 11)

#define C_OR_C(INDEX) (contact.getShapeA()->getBody()->getTag() & INDEX || contact.getShapeB()->getBody()->getTag() & INDEX)

#define IS_PROP_NOT_NULL_AND_FALSE(L,P) (!L->getProperty(P).isNull() && !L->getProperty(P).asBool())
#define IS_PROP_NOT_NULL_AND_TRUE(L,P) (!L->getProperty(P).isNull() && L->getProperty(P).asBool())

class CatPlayer;

//class RayCastJumpCallback : public b2RayCastCallback
//{
//public:
//	RayCastJumpCallback(CatPlayer* player) : player(player) {}
//
//	float ReportFixture(b2Fixture* fixture, const b2Vec2& point, const b2Vec2& normal, float fraction) override;
//
//	CatPlayer* player;
//};
//
//class RayCastTeleportCallback : public b2RayCastCallback
//{
//public:
//	RayCastTeleportCallback(CatPlayer* player) : player(player) {}
//
//	float ReportFixture(b2Fixture* fixture, const b2Vec2& point, const b2Vec2& normal, float fraction) override;
//
//	CatPlayer* player;
//};

class CatPlayer : public ax::Node {
public:
	static CatPlayer* createPhysicalEntity();

	//b2Body* body;
	ax::Node* body_anchor;
	ax::Sprite* player_sprite;
	ax::Sprite* player_shadow_sprite;
	ax::ParticleSystem* jump_particles;
	ax::ParticleSystem* wall_jump_particles;

	bool isTurnLocked = false;
	ax::MotionStreak* trail;

	std::string currentAnim;

	float idleAnimCurrentIndex = 0;
	std::vector<std::string> idleAnimationFrames;
	std::vector<std::string> wallTurnAnimationFrames;
	std::vector<std::string> jumpAnimationFrames;
	std::vector<std::string> staggerAnimationFrames;

	I8 lastValidDirection = 0;
	ax::Vec2 lastValidPosition = {0, 0};

	ax::Camera* cam;
	bool init();
	void attachCamera(ax::Camera* camera);

	bool debugMode = false;

	float tileRatio = 32;

	bool isBodyAnimated = false;

	void tick(F32 dt);
	void tickCameraSpace(F32 dt);

	ax::Vec2 camPos, camWobbleSpeed, camWobbleAmount;
	F32 camWobbleTime = 0;
	ax::Vec2 camDisplaceVector = ax::Vec2::ZERO;
	ax::Vec2 camSnapPixelVector = ax::Vec2::ZERO;
	ax::Vec2 camSnapLerpVector = ax::Vec2::ZERO;
	ax::Vec2 camSnapLerpType = ax::Vec2::ZERO;
	ax::Vec2 camSnapBorderVector = ax::Vec2::ZERO;
	float camZoomValue = 1;
	F32 speed;
	I8 playerDirection;
	F32 curZoom;
	F32 zoomAmount;
	I8 zoomDir;
	F32 zoomSnapTimer = 0;
	bool isReceivingInputs;
	ax::Vec2 movementDirection;
	ax::Vec2 playerSnapPlane;
	ax::Vec2 playerSnapPlaneLerp;
	bool isMovingLeft, isMovingRight, isMovingUp, isMovingDown;
	PROTECTED(F32) playerMoveBeginEase, playerMoveStopEase;
	bool actionButtonPress = false;
	PROTECTED(int) numberOfFlips;
	ax::Sequence* jumpActionDelay;
	bool isHeadBlocked = false;
	float playerStuckDuration = 0;
	ax::Vec2 contactDebug[4];
	MaxPushList<ax::Vec2> debugLineTraceY;
	ChangeValue<bool> controllerJump;

	CatPlayer()/* : jumpCallback(this), teleportCallback(this)*/ {
		debugLineTraceY = MaxPushList<ax::Vec2>(I8(10));
		camPos = ax::Vec2::ZERO;
		speed = 0;
		playerDirection = 1;
		curZoom = 1.0f;
		zoomAmount = 1;
		zoomDir = 0;
		isReceivingInputs = false;
		movementDirection = ax::Vec2::ZERO;
		isMovingLeft = isMovingRight = isMovingUp = isMovingDown = false;
		playerMoveBeginEase = 4;
		playerMoveStopEase = 3.5;
		numberOfFlips = 0;
	}

	~CatPlayer();

	void setInputState(bool isReceivingInputs);
	void onKeyPressed(ax::EventKeyboard::KeyCode keyCode);
	void onKeyReleased(ax::EventKeyboard::KeyCode keyCode);
	void onMouseDown(ax::Event* event);
	void onMouseUp(ax::Event* event);
	void onMouseScroll(ax::Event* event);
	bool onTouchBegan(ax::Touch* touch, ax::Event* event);
	void onTouchEnded(ax::Touch* touch, ax::Event* event);

	void changeAnimation(std::string_view name, bool enforce = false);

	//bool onContactBegin(ax::PhysicsContact& contact);
	//bool onContactSeperate(ax::PhysicsContact& contact);

	float actionRecoveryTime = 0;

	bool hasTouchedGround = false;
	//bool isOnGround();

	//b2World* world;
	//void physicsPreStep(DarknessPhysicsWorld* world, F32 dt);
	//void physicsPostStep(DarknessPhysicsWorld* world, F32 dt);

	//virtual void BeginContact(b2Contact* contact);
	//virtual void EndContact(b2Contact* contact);
	//virtual void PreSolve(b2Contact* contact, const b2Manifold* oldManifold);
	//virtual void PostSolve(b2Contact* contact, const b2Manifold* oldManifold);

	//void jump(F32 dt, bool noEffect = false);

	ax::Vec2 startVec1, endVec1;
	ax::Vec2 startVec2, endVec2;
	ax::Vec2 startVec3, endVec3;
	ax::DrawNode* debugDrawNode = nullptr;
	//ax::PhysicsRayCastCallbackFunc rayCastFunc1;
	//ax::PhysicsRayCastCallbackFunc rayCastFunc2;
	//ax::PhysicsRayCastCallbackFunc rayCastFunc3;
	//RayCastJumpCallback jumpCallback;
	//RayCastTeleportCallback teleportCallback;
	F32 playerYSpeed;
	bool teleportPlayer = false;
	bool isPlayerOnGroundRayCast = false;
	bool isStartUpAnimationDone = false;

	bool didRayCastsHit = false;
	int checkOtherRayCastsIndex = 0;
	F32 checkOtherRayCasts[2];

	int lastCollisionIndex = 1;
};

#endif
