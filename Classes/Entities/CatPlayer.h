#include "Components/FollowNodeTransformComponent.h"
#include "shared_scenes/ProtectedTypes.hpp"
#include "Wall.h"
#include "axmol.h"

#ifndef __H_CATPLAYER__
#define __H_CATPLAYER__

#define WALL_JUMP_COLLISION_INDEX			(1 << 4)
#define ONE_WAY_COLLISION_INDEX				(1 << 5)
#define OPPOSITE_WAY_COLLISION_INDEX		(1 << 6)
#define RIGHT_ONLY_COLLISION_INDEX			(1 << 7)
#define LEFT_ONLY_COLLISION_INDEX			(1 << 8)
#define DISABLE_JUMP_COLLISION_INDEX		(1 << 9)
#define DISABLE_TURN_COLLISION_INDEX		(1 << 10)

#define C_OR_C(INDEX) (contact.getShapeA()->getBody()->getTag() & INDEX || contact.getShapeB()->getBody()->getTag() & INDEX)

class CatPlayer : public ax::Node {
public:
	static CatPlayer* createEntity();
	ax::Node* player_sprite_parent;
	ax::Node* player_turn_dir;
	ax::Node* player_turn;
	ax::Sprite* player_sprite;
	ax::PhysicsBody* player_body;
	ax::PhysicsBody* player_turn_hitbox;
	bool isTurnLocked = false;
	ax::MotionStreak* trail;
	ax::EventListenerPhysicsContactWithGroup* contactor;

	std::string currentAnim;

	float idleAnimCurrentIndex = 0;
	int idleAnimDir = 1;
	std::vector<std::string> idleRightAnimationFrames;
	std::vector<std::string> idleLeftAnimationFrames;
	std::vector<std::string> wallTurnAnimationFrames;
	std::vector<std::string> jumpAnimationFrames;

	ax::Camera* cam;
	bool init();
	void attachCamera(ax::Camera* camera);

	void update(f32 dt);
	f32 camPosX, camPosY;
	f32 speed;
	i8 playerDirection;
	f32 curZoom;
	f32 zoomAmount;
	i8 zoomDir;
	f32 zoomSnapTimer = 0;
	bool isReceivingInputs;
	ax::Vec2 movementDirection;
	bool isMovingLeft, isMovingRight, isMovingUp, isMovingDown;
	PROTECTED(f32) playerMoveBeginEase, playerMoveStopEase;
	bool actionButtonPress = false;
	PROTECTED(int) numberOfFlips;

	ax::Sequence* jumpActionDelay;

	bool isHeadBlocked = false;

	float playerStuckDuration = 0;

	CatPlayer() {
		camPosX = 0;
		camPosY = 0;
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

	void changeAnimation(std::string_view name, bool enforce = false);

	bool onContactBegin(ax::PhysicsContact& contact);
	bool onContactSeperate(ax::PhysicsContact& contact);

	float actionRecoveryTime = 0;

	int onGroundIndex = 0;
	bool isOnGround();

	void physicsPreTick(ax::PhysicsWorld* world);
	void physicsPostTick(ax::PhysicsWorld* world);

	void jump();

	ax::Vec2 startVec1, endVec1;
	ax::Vec2 startVec2, endVec2;
	ax::DrawNode* rayCastDebug = nullptr;
	ax::PhysicsRayCastCallbackFunc rayCastFunc1;
	ax::PhysicsRayCastCallbackFunc rayCastFunc2;
	f32 playerYSpeed;
	bool teleportPlayer = false;

	bool didRayCastsHit = false;
	int checkOtherRayCastsIndex = 0;
	f32 checkOtherRayCasts[2];

	int lastCollisionIndex = 1;
};

#endif
