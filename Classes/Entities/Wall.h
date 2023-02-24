#include "Components/FollowNodeTransformComponent.h"
#include "shared_scenes/ProtectedTypes.hpp"
#include "axmol.h"

class Wall : public ax::Node {
public:
	static Wall* createEntity();
	ax::PhysicsBody* wall_body;
	ax::Sprite* sprite;
	bool init();

	void update(f32 dt);

	Wall() {

	}

};
