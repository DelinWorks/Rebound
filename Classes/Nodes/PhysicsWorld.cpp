#include "PhysicsWorld.h"

DarknessPhysicsWorld::DarknessPhysicsWorld() : b2World({0,0}) {}

DarknessPhysicsWorld::~DarknessPhysicsWorld() {}

bool DarknessPhysicsWorld::init(ax::Vec2 gravity) {
	b2Vec2 b2Gravity;
	b2Gravity.Set(gravity.x, gravity.y);
	SetGravity(b2Gravity);

	drawBox2D = g_debugDraw.GetDrawNode();
	addChild(drawBox2D, 100);
	drawBox2D->setOpacity(150);

	uint32 flags = 0;
	flags |= 1 * b2Draw::e_shapeBit;
	flags |= 0 * b2Draw::e_jointBit;
	flags |= 0 * b2Draw::e_aabbBit;
	flags |= 0 * b2Draw::e_centerOfMassBit;
	g_debugDraw.SetFlags(flags);
	g_debugDraw.mRatio = B2_PTM;
	g_debugDraw.debugNodeOffset = { 0, 0 };
	SetDebugDraw(&g_debugDraw);

	SetContactListener(this);

	preStepCallback  = [] (DarknessPhysicsWorld*, f32) -> void {};
	postStepCallback = [] (DarknessPhysicsWorld*, f32) -> void {};

	beginContactCallback = [] (b2Contact*) -> void {};
	endContactCallback   = [] (b2Contact*) -> void {};
	preSolveCallback     = [] (b2Contact*, const b2Manifold*) -> void {};
	postSolveCallback    = [] (b2Contact*, const b2Manifold*) -> void {};

	autorelease();
	return true;
}

void DarknessPhysicsWorld::update(f32 dt, int v, int p)
{
	preStepCallback(this, dt);
	Step(dt, v, p);
	postStepCallback(this, dt);
	
	// Debug draw
	if (showDebugDraw)
	{
		drawBox2D->clear();
		DebugDraw();
	}
}

void DarknessPhysicsWorld::debug(bool enabled)
{
	showDebugDraw = enabled;
	drawBox2D->clear();
}

void DarknessPhysicsWorld::BeginContact(b2Contact* contact)
{
}

void DarknessPhysicsWorld::EndContact(b2Contact* contact)
{
	endContactCallback(contact);
}

void DarknessPhysicsWorld::PreSolve(b2Contact* contact, const b2Manifold* oldManifold)
{
	// This portion of code fixes a bug that plagues every physics engine in the world,
	// This is mainly a work-around and is only suitable for fixed rotation bodies.
	// 
	// It prevents the player from colliding in the corner of another box that the player
	// has just touched only if that box happens to be non-batched or is on another layer.
	// 
	// It simply works by shifting the body of the player up by 0.006 before the collision
	// solver does anything, and that prevents the player from turning because it got
	// caught on a corner, this value is choosen such that it has a significant numerical
	// value that the solver ignores, but has little to no visual/physical impact!

	{
		b2Body* body = nullptr;
		if (contact->GetFixtureA()->GetBody()->GetType() == b2_dynamicBody)
			body = contact->GetFixtureA()->GetBody();
		if (body) {
			body->SetTransform({ body->GetPosition().x, float(body->GetPosition().y + 0.006) }, 0);
		}
	}
}

void DarknessPhysicsWorld::PostSolve(b2Contact* contact, const b2Manifold* oldManifold)
{
}
