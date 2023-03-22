#ifndef _PHYSICS_WORLD_H_
#define _PHYSICS_WORLD_H_

#include "axmol.h"
#include "box2d/box2d.h"
#include "extensions/cocos-ext.h"
#include "Helper/short_types.h"

#define B2_PTM float(64.0)

#define TO_B2_C(R) float(R / B2_PTM)
#define FROM_B2_C(R) float(R * B2_PTM)

#define VEC2_2_B2(vec2) b2Vec2(vec2.x, vec2.y)
#define B2_2_VEC2(b2) ax::Vec2(b2.x, b2.y)

#define TO_B2_VEC2(vec2) ax::Vec2(TO_B2_C(vec2.x), TO_B2_C(vec2.y))
#define TO_B2_B2(vec2) b2Vec2(TO_B2_C(vec2.x), TO_B2_C(vec2.y))

#define FROM_B2_VEC2(vec2) ax::Vec2(FROM_B2_C(vec2.x), FROM_B2_C(vec2.y))
#define FROM_B2_B2(vec2) b2Vec2(FROM_B2_C(vec2.x), FROM_B2_C(vec2.y))

#define PTM_VEC2_2_B2(vec2) b2Vec2(TO_B2_C(vec2.x), TO_B2_C(vec2.y))
#define PTM_B2_2_VEC2(b2) ax::Vec2(FROM_B2_C(b2.x), FROM_B2_C(b2.y))

class DarknessPhysicsWorld : public ax::Node, public b2World, public b2ContactListener {
public:
	DarknessPhysicsWorld();
	~DarknessPhysicsWorld();

	bool init(ax::Vec2 gravity);

	void update(f32 dt, int v = 1, int p = 1);

	void debug(bool enabled);

	std::function<void(DarknessPhysicsWorld*, f32)> preStepCallback;
	std::function<void(DarknessPhysicsWorld*, f32)> postStepCallback;

	std::function<void(b2Contact*)> beginContactCallback;
	std::function<void(b2Contact*)> endContactCallback;
	std::function<void(b2Contact*, const b2Manifold*)> preSolveCallback;
	std::function<void(b2Contact*, const b2Manifold*)> postSolveCallback;

protected:
	virtual void BeginContact(b2Contact* contact);
	virtual void EndContact(b2Contact* contact);
	virtual void PreSolve(b2Contact* contact, const b2Manifold* oldManifold);
	virtual void PostSolve(b2Contact* contact, const b2Manifold* oldManifold);

	ax::DrawNode* drawBox2D;
	ax::extension::PhysicsDebugNodeBox2D g_debugDraw;
	bool showDebugDraw = false;
};

#endif