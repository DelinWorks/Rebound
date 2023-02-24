#include "Components/FollowNodeTransformComponent.h"
#include "shared_scenes/ProtectedTypes.hpp"
#include "axmol.h"

class CatPlayer : public ax::Node {
public:
	static CatPlayer* createEntity();
	ax::Node* player_sprite_parent;
	ax::Sprite* player_sprite;
	ax::PhysicsBody* player_body;

	std::string currentAnim;

	float idleAnimCurrentIndex = 0;
	int idleAnimDir = 1;
	std::vector<std::string> idleAnimationFrames;

	ax::Camera* cam;
	bool init();
	void attachCamera(ax::Camera* camera);

	void update(f32 dt);
	PROTECTED(f32) playerPosX, playerPosY;
	PROTECTED(f32) speed;
	f32 curZoom;
	f32 zoomAmount;
	i8 zoomDir;
	f32 zoomSnapTimer = 0;
	bool isReceivingInputs;
	ax::Vec2 movementDirection;
	bool isMovingLeft, isMovingRight, isMovingUp, isMovingDown;
	PROTECTED(f32) playerMoveBeginEase, playerMoveStopEase;
	bool actionButtonPress = false;
	int numberOfFlips = 0;

	CatPlayer() {
		playerPosX = 0;
		playerPosY = 0;
		speed = 300;
		curZoom = 1.0f;
		zoomAmount = 1;
		zoomDir = 0;
		isReceivingInputs = false;
		movementDirection = ax::Vec2::ZERO;
		isMovingLeft = isMovingRight = isMovingUp = isMovingDown = false;
		playerMoveBeginEase = 4;
		playerMoveStopEase = 3.5;
	}

	void setInputState(bool isReceivingInputs);
	void onKeyPressed(ax::EventKeyboard::KeyCode keyCode);
	void onKeyReleased(ax::EventKeyboard::KeyCode keyCode);
	void onMouseDown(ax::Event* event);
	void onMouseUp(ax::Event* event);
	void onMouseScroll(ax::Event* event);

	void jump(bool ignoreCond = false);
};
