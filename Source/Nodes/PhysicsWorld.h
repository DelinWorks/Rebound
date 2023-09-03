#pragma once

#include "Ref.h"
#include "Types.h"
#include "Helper/Math.h"
#include "Helper/QuadTree.hpp"
#include <Helper/Logging.hpp>

namespace ReboundPhysics
{
#define VERTICAL_RESOLUTION_LEEWAY 8
#define CCD_MAX_TARGETS 1024
#define MOVE_MAX_TARGETS 1024
#define CCD_STEPS_TO_PERC(T) (1.0 / T)
#define NUM_SIGN(N) (N/abs(N))
#define FUZZYG(F, S, T) (F >= S - T)
#define FUZZYL(F, S, T) (F <= S + T)
#define FUZZY(F, S, T) (FUZZYG(F,S,T) && FUZZYL(F,S,T))

    // A class used for physics that describes dynamic/static rectangles and slopes,
    // Might probably need to be divided to reduce memory consumption.
    class CollisionShape : public ax::Ref {
    public:
        CollisionShape() {}
        CollisionShape(F32 _x, F32 _y, F32 _w, F32 _h) : x(_x), y(_y), w(_w), h(_h) {}

        F32 x, y;
        F32 nx = 0.0, ny = 0.0;
        F32 w, h;
        bool isTriangle = false;
        F32 l, b;
        bool isTrigger = false;
        F32 gravity;
        V2D vel = V2D::ZERO;
        V2D pref_vel = V2D::ZERO;
        V2D lerp_vel = V2D::ZERO;
        bool isMovable = false;
        F32 internalAngle = 0.0f;
        F32 internalAngleLerp = 0.0f;
        bool isMovableLerpApplied = false;
        CollisionShape* movableGround = nullptr;
        V2D movableGroundPos;
        V2D movableGroundMtv;
        CollisionShape* slopeGround = nullptr;

        ax::Color4F debugColor = ax::Color4F::RED;
    };

    struct RayCastResult {
        bool intersects = false;
        V2D point;
    };

    struct ResolveResult {
        bool isGrounded = false;
        bool isSlope = false;
        bool isSlopeOutsideH = false;
        bool isSlopeOutsideV = false;
        F32 slopeAngle = 0.0f;
        F32 verticalMTV = 0.0f;
    };

    CollisionShape* createRect(V2D pos, V2D size);

    CollisionShape* createSlope(V2D pos, F32 length, F32 base);

    CollisionShape* createRectDynamic(V2D pos, V2D size, F32 gravity);

    bool getCollisionShapeIntersect(const CollisionShape& s1, const CollisionShape& s2);

    V2D calculateRect2RectMTV(const CollisionShape& rect1, const CollisionShape& rect2);

    I32 getLineOrientation(const V2D& a, const V2D& b, const V2D& c);

    bool isLineSegment(const V2D& a, const V2D& c, const V2D& b);

    V2D getIntersectionPoint(const V2D& a, const V2D& b, const V2D& c, const V2D& d);

    RayCastResult doLinesIntersect(const V2D& a, const V2D& b, const V2D& c, const V2D& d);

    // TODO: this is fucking ugly and slow,
    // please use ray orientation to solve this problem efficiently.
    RayCastResult doLineIntersectsRect(const V2D& p1, const V2D& p2, const CollisionShape& rect);

    RayCastResult doLineIntersectsRects(const V2D& p1, const V2D& p2, std::vector<CollisionShape>& _shapeCollection);

    bool getCollisionTriangleIntersect(const CollisionShape& r, const CollisionShape& t);

    ResolveResult resolveCollisionRect(CollisionShape& _, CollisionShape& __, const V2D& mtv, bool ignoreVL);

    CollisionShape getTriangleEnvelop(const CollisionShape& triangle);

    ResolveResult resolveCollisionSlope(CollisionShape& r, CollisionShape& t, bool isJumping, F32 verticalMtv, F32 delta);

    CollisionShape getRectSweepEnvelope(const CollisionShape& rect1, const CollisionShape& rect2, F32 oExtend);

    // OBSOLETE
    I32 getCCDPrecessionSteps(F32 velocityMagnitude);

    void stepDynamic(CollisionShape& s, F32 delta, F32 fraction);

    void setBodyPosition(CollisionShape& s, V2D newPos, bool sweep);

    namespace Chunking
    {
        using namespace SpatialDataStructures;

        inline void check() {

            auto q = new QuadTree<CollisionShape*>(QTBounds(-1073741823, -1073741823, 1073741823, 1073741823));

            std::unordered_map<V2DH, CollisionShape*> _chunks;
            BENCHMARK_SECTION_BEGIN("Add to quadtree");
            for (int i = 0; i < 100000; i++)
                _chunks.emplace(V2D(random(), i * 2), (CollisionShape*)1);
            BENCHMARK_SECTION_END();
            BENCHMARK_SECTION_BEGIN("find to quadtree");
            for (int i = 0; i < 100000; i++)
                _chunks[V2D(i, i * 2)];
            BENCHMARK_SECTION_END();


        }
    };

    class PhysicsWorld : public ax::Node {
    private:
        PhysicsWorld()
        {
            _physicsDebugNode = nullptr;

            _moveTargets = new CollisionShape*[MOVE_MAX_TARGETS];
        }

        ~PhysicsWorld()
        {
            delete[] _moveTargets;
        }

        F64 lastPhysicsDt = 0;
        F64 currentPhysicsDt = 0;
        void step(F64 delta);

        ax::DrawNode* _physicsDebugNode;

    public:
        // ---------- HEAP ----------
        U32 _moveTargetCount = 0;
        CollisionShape** _moveTargets;
        // --------------------------

        bool isJumping = false;
        bool isGrounded = false;

        CollisionShape* modifySlope;
        CollisionShape* movingPlat;

        ax::Vector<CollisionShape*> _dynamicShapes;
        ax::Vector<CollisionShape*> _staticShapes;

        static PhysicsWorld* create();
        void update(F32 delta) override;
    };

    void setShapePosition(PhysicsWorld* _worldToRegisterSweep, CollisionShape& _shape, const V2D& newPos);
}