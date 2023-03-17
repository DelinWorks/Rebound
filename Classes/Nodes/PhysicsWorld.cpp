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

	preStepCallback  = [] (DarknessPhysicsWorld * world, f32 dt) -> void {};
	postStepCallback = [] (DarknessPhysicsWorld * world, f32 dt) -> void {};

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
}

void DarknessPhysicsWorld::PreSolve(b2Contact* contact, const b2Manifold* oldManifold)
{
	b2Body* body = nullptr;

	if (contact->GetFixtureA()->GetBody()->GetType() == b2_dynamicBody)
		body = contact->GetFixtureA()->GetBody();

	if (body) {
		body->SetTransform({body->GetPosition().x, float(body->GetPosition().y + 0.001)}, 0);
	}
}

void DarknessPhysicsWorld::PostSolve(b2Contact* contact, const b2Manifold* oldManifold)
{
}
