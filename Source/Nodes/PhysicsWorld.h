#pragma once

#include "Ref.h"
#include "Types.h"
#include "Helper/Math.h"
#include "Helper/MemoryPoolVector.hpp"
#include <Helper/Logging.hpp>

namespace ReboundPhysics
{
#define VERTICAL_RESOLUTION_LEEWAY 8
#define CCD_MAX_TARGETS 65535
#define MOVE_MAX_TARGETS 65535
#define CHUNK_MAX_TARGETS UINT8_MAX
#define CCD_STEPS_TO_PERC(T) (1.0 / T)
#define NUM_SIGN(N) (N/abs(N))
#define FUZZYG(F, S, T) (F >= S - T)
#define FUZZYL(F, S, T) (F <= S + T)
#define FUZZY(F, S, T) (FUZZYG(F,S,T) && FUZZYL(F,S,T))

#define PHYS_CHUNK_TIER0_SIZE V2D(512, 512)
#define PHYS_CHUNK_TIER1_SIZE V2D(4096, 4096)
#define PHYS_CHUNK_TIER2_SIZE V2D(8192, 8192)
#define PHYS_CHUNK_TIER3_SIZE V2D(65535, 65535)
#define PHYS_CHUNK_TIER4_SIZE V2D(67107840, 67107840)

    // A class used for physics that describes dynamic/static rectangles and slopes,
    // Might probably need to be divided to reduce memory consumption.
    class CollisionShape {
    public:
        CollisionShape() {}
        CollisionShape(F32 _x, F32 _y, F32 _w, F32 _h) : x(_x), y(_y), w(_w), h(_h) {}

        F32 x = 0, y = 0;
        F32 w = 0, h = 0;

        F32 cx = 0, cy = 0;
        F32 cw = 0, ch = 0;

        F32 nx = 0.0, ny = 0.0;
        F32 dx = 0.0, dy = 0.0;
        bool isTriangle = false;
        F32 l = 0, b = 0;
        bool isTrigger = false;
        bool isMovable = false;
        bool hasObjectCollidedChunk = false;

        U8 chunkTier = -1;
        V2DH currentChunk;

        ax::Color4F debugColor = ax::Color4F::WHITE;
    };

    class DynamicCollisionShape : public CollisionShape {
    public:
        DynamicCollisionShape() {}
        DynamicCollisionShape(F32 _x, F32 _y, F32 _w, F32 _h) { x = _x; y = _y; w = _w; h = _h; }

        F32 gravity = 0;
        V2D vel = V2D::ZERO;
        V2D pref_vel = V2D::ZERO;
        V2D lerp_vel = V2D::ZERO;
        F32 internalAngle = 0.0f;
        F32 internalAngleLerp = 0.0f;
        V2D relativeMovableForce = V2D::ZERO;
        CollisionShape* movableGround = nullptr;
        V2D movableGroundPos = V2D::ZERO;
        V2D movableGroundMtv = V2D::ZERO;
        F32 timeSpentOnSlope = 0;
        CollisionShape* slopeGround = nullptr;
        bool isSlopeGroundSteep = false;
        F32 hSpeed = 0.0f;
        bool isMtvXApplied = false;
        float steepSlopeVelX = 0;
    };

    typedef std::vector<CollisionShape*> CollisionVectorRef;
    typedef std::vector<DynamicCollisionShape*> DynamicCollisionVectorRef;
    typedef MemPoolVector<CollisionShape*> CollisionMemPoolVector;
    typedef std::unordered_map<V2DH, CollisionMemPoolVector> CollisionChunkMap;

    struct RayCastResult {
        bool intersects = false;
        V2D point;
    };

    struct ResolveResult {
        bool isGrounded = false;
        bool isSlope = false;
        bool isSlopeOutsideH = false;
        bool isSlopeOutsideV = false;
        bool isSlopeGroundSteep = false;
        F32 slopeAngle = 0.0f;
        F32 verticalMTV = 0.0f;
    };

    void setChunkTier(CollisionShape* shape);
    V2D getChunkTierSize(U8 tier);

    CollisionShape* createRect(V2D pos, V2D size);

    CollisionShape* createSlope(V2D pos, F32 length, F32 base);

    DynamicCollisionShape* createRectDynamic(V2D pos, V2D size, F32 gravity);

    bool getCollisionShapeIntersect(const CollisionShape& s1, const CollisionShape& s2);

    V2D calculateRect2RectMTV(const DynamicCollisionShape& rect1, const CollisionShape& rect2, bool both, F32 delta);

    I32 getLineOrientation(const V2D& a, const V2D& b, const V2D& c);

    bool isLineSegment(const V2D& a, const V2D& c, const V2D& b);

    V2D getIntersectionPoint(const V2D& a, const V2D& b, const V2D& c, const V2D& d);

    RayCastResult doLinesIntersect(const V2D& a, const V2D& b, const V2D& c, const V2D& d);

    // TODO: this is fucking ugly and slow,
    // please use ray orientation to solve this problem efficiently.
    RayCastResult doLineIntersectsRect(const V2D& p1, const V2D& p2, const CollisionShape& rect);

    RayCastResult doLineIntersectsRects(const V2D& p1, const V2D& p2, std::vector<CollisionShape>& _shapeCollection);

    bool getCollisionTriangleIntersect(const CollisionShape& r, const CollisionShape& t);

    ResolveResult resolveCollisionRect(DynamicCollisionShape& _, CollisionShape& __, F32 delta, const V2D& mtv, bool ignoreVL);

    CollisionShape getTriangleEnvelop(const CollisionShape& triangle);

    ResolveResult resolveCollisionSlope(DynamicCollisionShape& r, CollisionShape& t, bool isJumping, F32 verticalMtv, F32 delta);

    CollisionShape getRectSweepEnvelope(const CollisionShape& rect1, const CollisionShape& rect2, F32 oExtend);

    // OBSOLETE
    I32 getCCDPrecessionSteps(F32 velocityMagnitude);

    void stepDynamic(DynamicCollisionShape& s, F32 fraction);

    void setBodyPosition(CollisionShape& s, V2D newPos, bool sweep);

    // static pre-allocated arrays are used for performance
    I32 chunkGetCoverArea(V2DH* array, CollisionShape s, bool oneChunk);

    class PhysicsWorld : public ax::Node {
    private:
        PhysicsWorld()
        {
            _physicsDebugNode = nullptr;

            _moveTargets = new CollisionShape*[MOVE_MAX_TARGETS];

            _coveredChunksBuffers = new V2DH*[2];
            for (int i = 0; i < 2; i++)
                _coveredChunksBuffers[i] = new V2DH[CHUNK_MAX_TARGETS];
        }

        ~PhysicsWorld()
        {
            delete[] _moveTargets;

            for (int i = 0; i < 2; i++)
                delete[] _coveredChunksBuffers[i];
            delete[] _coveredChunksBuffers;
        }

        void move(F64 subdt);
        void step(F64 delta);

        ax::DrawNode* _physicsDebugNode;

    public:
        F64 lastPhysicsDt = 0;
        F64 currentPhysicsDt = 0;
        F64 stepElapsedDelta = 0;

        // ---------- HEAP ----------
        U32 _moveTargetCount = 0;
        CollisionShape** _moveTargets;
        // --------------------------

        // ---- PERFORMANCE HEAP ----
        V2DH** _coveredChunksBuffers;
        // --------------------------

        bool isJumping = false;
        bool isGrounded = false;

        CollisionShape* modifySlope;
        std::vector<CollisionShape*> movingPlat;
        CollisionShape* ground;

        CollisionVectorRef _staticShapes;

        DynamicCollisionVectorRef _dynamicShapes;
        std::map<void*, std::map<void*, uint32_t>> chunks;

        void partition();

        CollisionShape envelope;

        CollisionChunkMap _staticShapeChunksTier0;
        CollisionChunkMap _staticShapeChunksTier1;
        CollisionChunkMap _staticShapeChunksTier2;
        CollisionChunkMap _staticShapeChunksTier3;
        CollisionChunkMap _staticShapeChunksTier4;
        CollisionChunkMap& getChunkTierMap(U8 tier);
         
        struct ExtraDrawData
        {
            ax::Vec2 orig;
            ax::Vec2 dest;
            ax::Color4B col;
        };

        std::vector<ExtraDrawData> _extraDraw;

        static PhysicsWorld* create();
        void update(F32 delta) override;
    };

    void setShapePosition(PhysicsWorld* _worldToRegisterSweep, CollisionShape& _shape, const V2D& newPos);
}