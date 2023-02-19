#include "Components/FollowNodeTransformComponent.h"
#include "shared_scenes/ProtectedTypes.hpp"
#include "axmol.h"

class Player : public ax::Node {
public:
	static Player* createPlayer();
	ax::Node* chaseNode;
	ax::Node* base;
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


	Player() {
		playerPosX = 0;
		playerPosY = 0;
		speed = 400;
		curZoom = 1.0f;
		zoomAmount = 1;
		zoomDir = 0;
		isReceivingInputs = false;
		movementDirection = ax::Vec2::ZERO;
		isMovingLeft = isMovingRight = isMovingUp = isMovingDown = false;
		playerMoveBeginEase = 0.5 * 5;
		playerMoveStopEase = 0.7 * 5;
	}

	void setInputState(bool isReceivingInputs);
	void onKeyPressed(ax::EventKeyboard::KeyCode keyCode);
	void onKeyReleased(ax::EventKeyboard::KeyCode keyCode);
	void onMouseDown(ax::Event* event);
	void onMouseUp(ax::Event* event);
	void onMouseScroll(ax::Event* event);

};
