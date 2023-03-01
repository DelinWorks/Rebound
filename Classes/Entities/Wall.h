#include "Components/FollowNodeTransformComponent.h"
#include "shared_scenes/ProtectedTypes.hpp"
#include "axmol.h"

#ifndef __H_WALL__
#define __H_WALL__

class Wall : public ax::Node {
public:
	static Wall* createEntity(ax::Vec2 size, ax::Vec2 offset);
	ax::PhysicsBody* wall_body;
	ax::Sprite* sprite;
	bool init(ax::Vec2 size, ax::Vec2 offset);

	void update(f32 dt);

	Wall() {

	}

};

#endif