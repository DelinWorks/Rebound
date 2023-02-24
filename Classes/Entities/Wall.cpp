#include "Wall.h"
#include "shared_scenes/GameUtils.h"
#include "2d/CCTweenFunction.h"

Wall* Wall::createEntity()
{
	Wall* p = new Wall();
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

bool Wall::init()
{
	wall_body = ax::PhysicsBody::createBox({ 32, 32 });
	wall_body->setDynamic(false);

	sprite = Sprite::create("player/player.png");
	sprite->setPhysicsBody(wall_body);

	addChild(sprite);

	return true;
}

void Wall::update(f32 dt)
{
}
