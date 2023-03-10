#include "Wall.h"
#include "shared_scenes/GameUtils.h"
#include "2d/CCTweenFunction.h"

#include "chipmunk/chipmunk_private.h"

Wall* Wall::createEntity(ax::Vec2 size, ax::Vec2 offset, int collision)
{
	Wall* p = new Wall();
	if (p->init(size, offset, collision))
	{
		p->autorelease();
	}
	else
	{
		AX_SAFE_DELETE(p);
	}
	return p;
}

bool Wall::init(ax::Vec2 size, ax::Vec2 offset, int collision)
{
	wall_body = ax::PhysicsBody::createBox(size, { 0, 0, 0 }, offset);
	wall_body->setDynamic(false);
	wall_body->setTag(collision);
	wall_body->setGroup(9);
	wall_body->setContactTestBitmask(9);

	wall_body->getFirstShape()->_cpShapes[0]->filter = cpShapeFilterNew(1, 1, 1);
	cpBodySetType(wall_body->getCPBody(), CP_BODY_TYPE_STATIC);

	setPhysicsBody(wall_body);

	return true;
}

void Wall::update(f32 dt)
{
}
