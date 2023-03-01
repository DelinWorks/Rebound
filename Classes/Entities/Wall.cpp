#include "Wall.h"
#include "shared_scenes/GameUtils.h"
#include "2d/CCTweenFunction.h"

#include "chipmunk/chipmunk_private.h"

Wall* Wall::createEntity(ax::Vec2 size, ax::Vec2 offset)
{
	Wall* p = new Wall();
	if (p->init(size, offset))
	{
		p->autorelease();
	}
	else
	{
		AX_SAFE_DELETE(p);
	}
	return p;
}

bool Wall::init(ax::Vec2 size, ax::Vec2 offset)
{
	wall_body = ax::PhysicsBody::createBox(size, PHYSICSBODY_MATERIAL_DEFAULT, offset);
	wall_body->setDynamic(false);
	wall_body->setGroup(9);
	wall_body->setContactTestBitmask(9);

	wall_body->getFirstShape()->_cpShapes[0]->filter = cpShapeFilterNew(1, 1, 1);

	sprite = Sprite::create("player/player.png");
	sprite->setPhysicsBody(wall_body);
	sprite->setVisible(false);

	addChild(sprite);

	return true;
}

void Wall::update(f32 dt)
{
}
