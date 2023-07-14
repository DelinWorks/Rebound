#pragma once

#include <axmol.h>
#include <renderer/backend/Backend.h>
#include "Helper/Logging.hpp"
#include "Helper/short_types.h"
#include <Helper/Math.h>
#include <Helper/Random.h>

using namespace ax;

namespace TileSystem {

    class ChunkRenderMethod {
    public:
        ChunkRenderMethod() {}
        bool _resize = true;
        virtual void cacheVertices(bool _resize) = 0;
    };

    typedef u32 TileID;

#define CHUNK_SIZE 64.0
#define CHUNK_BUFFER_SIZE (CHUNK_SIZE*CHUNK_SIZE)

#define TILE_FLAG_ROTATE (TileID(1) << 31)
#define TILE_FLAG_FLIP_X (TileID(1) << 30)
#define TILE_FLAG_FLIP_Y (TileID(1) << 29)
#define TILE_FLAG_ALL    (TILE_FLAG_ROTATE | TILE_FLAG_FLIP_X | TILE_FLAG_FLIP_Y)
#define TILE_FLAG_NONE   ~(TILE_FLAG_ALL)

#define USE_COLOR_BLEED_TOLERANCE 1
#define USE_VERTEX_BLEED_TOLERANCE 1

#define VERTEX_SIZE_NO_ANIMATIONS 9

#define VERTEX_SIZE_ANIMATED_2_ATRIBS 11
#define VERTEX_SIZE_ANIMATED_4_ATRIBS 13
#define VERTEX_SIZE_ANIMATED_8_ATRIBS 19

    class EmptyVertexIndexCache {
    public:
        void fill();

        const std::vector<f32>& getVertex();
        const IndexArray& getIndex();

    protected:
        bool _isFill = false;
        std::vector<f32> vertices;
        IndexArray indices;
    };

    inline ax::Node* tileMapVirtualCamera = nullptr;
    inline EmptyVertexIndexCache emptyVIC;
    inline f32 chunkMeshCreateCount;
    inline float zPositionMultiplier = 0;
    inline u32 maxDrawCallCount = 8192;

    struct UV {
        f32 U;
        f32 V;
    };

    struct TileTexCoords {
        UV tl{ 0,0 };
        UV tr{ 0,0 };
        UV bl{ 0,0 };
        UV br{ 0,0 };

        UV saved_tl{ 0,0 };
        UV saved_tr{ 0,0 };
        UV saved_bl{ 0,0 };
        UV saved_br{ 0,0 };

        bool isH = false;
        bool isV = false;
        bool is90 = false;
        bool isSaved = false;

        void save();
        void reset();
        void rotate90();
        void flipH();
        void flipV();

        void cw();
        void ccw();
        TileID state();

        i8 rotation = 0;
        bool _outOfRange = false;
    };

    class Tileset : public ax::Ref {
    public:
        u32 _firstGid = 0;
        ax::Texture2D* _texture;
        ax::Vec2 _tileSize;
        ax::Vec2 _textureSize;
        ax::Vec2 _sizeInPixels;

        Tileset(ax::Texture2D* _texture);

        ~Tileset();
    };

    class TilesetArray : public ax::Ref {
    public:
        static TilesetArray* create(ax::Vec2 tileSize);

        void addTileset(Tileset* tileset);

        void addTileset(ax::Texture2D* tex);

        void reloadTextures();

        void calculateBounds();

        TileID relativeID(u16 id, TileID gid);

        ~TilesetArray();

        i32 retainedChunks = 0;
        i32 retainedChunksI = 0;
        ax::Vec2 _tileSize;
        std::vector<Tileset*> _tileSets;
    };

    class TileArray : public ax::Ref {
    public:
        static TileArray* create(TileID* _tiles);

        TileID* getArrayPointer(bool dirty = false);

        void update();

        bool isEmpty(i32 firstGid);

        ~TileArray();

        bool isVertexCacheBuilt = false;
        TilesetArray* cachedTilesetArr = nullptr;
        bool _tileArrayDirty = false;
        std::set<i32> _emptyTilesets;
        std::unordered_map<i32, std::vector<f32>> vertexCache;
        i32 retainedChunks = 0;
        i32 retainedChunksI = 0;
        TileID* _tiles = nullptr;
    };

    class ChunkDescriptor {
    public:
        i32 _vertexSize = 0;
        bool _chunkDirty = true;
        bool _isParent = false;
        bool _isModified = false;

        ax::Mesh* _mesh = nullptr;
        TileArray* _tiles = nullptr;
        Tileset* _tileset = nullptr;
        TilesetArray* _tilesetArr = nullptr;
    };

    namespace ChunkFactory {
        static TileTexCoords calculateTileCoords(TileID id, Tileset* tileset);

        static i32 buildVertexIndex(TileArray* tileArr, Tileset* tileset, std::vector<f32>& vertices, IndexArray& indices, bool resize);

        static Mesh* buildTiledMesh(TileArray* _tileArr, Tileset* _tileset, bool _resize);

        /* Build vertex cache for a specified TileArray* object.
        * This helps reduce build time of a chunk by directly
        updating the vertex buffer with the needed data using
        a vertex cache map that stores caches based on first gid.
        */
        static void buildVertexCache(TileArray* tiles, TilesetArray* tilesets, bool _resize);

        // This modifies the vertex cache directly for better performance
        // Returns true if the previous tile had a gid of 0
        static bool setTile(TileArray* tiles, TileID index, TileID newGid, bool _resize);
    };

    class SingleTilesetChunkRenderer : public ChunkDescriptor, public ChunkRenderMethod, public ax::MeshRenderer {
    public:
        static SingleTilesetChunkRenderer* create();

        void updateVertexData();

        ~SingleTilesetChunkRenderer();

        void unload();

        void visit(Renderer* renderer, const Mat4& parentTransform, u32 parentFlags) override;
        void visit(Renderer* renderer, const Mat4& parentTransform, u32 parentFlags, ax::MeshMaterial* mat);

        void cacheVertices(bool _resize);
    };

    class ChunkRenderer : public ChunkDescriptor, public ChunkRenderMethod, public ax::Node {
    public:
        static ChunkRenderer* create(ChunkDescriptor desc);

        ~ChunkRenderer();

        i32 count = 0;

        i32 resizeChunkCount();

        ax::Vec2 _pos = Vec2::ZERO;

        void setPositionInChunkSpace(ax::Vec2 pos);

        void setPositionInChunkSpace(f32 x, f32 y);

        void draw(Renderer* renderer, const Mat4& parentTransform, u32 parentFlags) override {}
        void visit(Renderer* renderer, const Mat4& parentTransform, u32 parentFlags) override {}
        void visit(Renderer* renderer, const Mat4& parentTransform, u32 parentFlags, u32* renderCount, ax::MeshMaterial* mat, const ax::BlendFunc& blendFunc);

        void cacheVertices(bool _resize);

        std::vector<SingleTilesetChunkRenderer*> _chunks;
    };

    class Layer : public ChunkRenderMethod, public ax::BlendProtocol, public ax::Node {
    public:
        static Layer* create(std::string_view name);

        ~Layer();

        ChunkRenderer* getChunkAtPos(Vec2 pos, TileID hintGid = -1);

        void draw(Renderer* renderer, const Mat4& parentTransform, uint32_t parentFlags) override {}
        void visit(Renderer* renderer, const Mat4& parentTransform, uint32_t parentFlags) override;

        void cacheVertices(bool _resize);

        void setBlendFunc(const BlendFunc& blendFunc);
        const BlendFunc& getBlendFunc() const;

        ax::BlendFunc blend;
        u32 currentMaxDrawCallCount;

        ax::MeshMaterial* _material;
        std::string _layerName = "";
        Color4F _layerColor = Color4F::WHITE;
        TilesetArray* _tilesetArr = nullptr;
        std::vector<Vec2> _chunksToRemove;
        std::unordered_map<Vec2Hashable, ChunkRenderer*> _chunks;
    };

    struct TileTransform {
        ax::Vec2 chunk;
        TileID id;

        TileTransform() : chunk(ax::Vec2::ZERO), id(0) {}
        TileTransform(ax::Vec2 _chunk, TileID _id) : chunk(_chunk), id(_id) {}
    };

    class Map : public ChunkRenderMethod, public ax::Node {
    public:
        static Map* create(Vec2 _tileSize, i32 _contentScale, Vec2 _mapSize);

        ~Map();

        void addLayer(Layer* _layer);

        void addLayer(std::string name);

        void bindLayer(u16 idx);

        void setTilesetArray(TilesetArray* _tilesetArr);

        void reload();

        /* Given a tile position and a chunk reference, it will calculate the chunk
           position that the tile lies on and set that value to the chunk reference,
           it will then calculate which index in the array the tile's at and return it.
        */
        TileTransform getTileTransform(const ax::Vec2& pos);

        void setTileAt(Layer* _layer, const ax::Vec2& pos, TileID gid);

        void setTileAt(const ax::Vec2& pos, TileID gid);

        TileID getTileAt(const ax::Vec2& pos);

        TileID getTileGIDAt(const ax::Vec2& pos);

        void draw(Renderer* renderer, const Mat4& parentTransform, uint32_t parentFlags) override {}
        void visit(Renderer* renderer, const Mat4& parentTransform, uint32_t parentFlags) override;

        TilesetArray* _tilesetArr;
        Layer* _layerBind;
        u16 _layerIdx;
        std::vector<Layer*> _layers;
        Layer* _editorLayer;

        void cacheVertices(bool _resize);

        ChunkRenderer* cachedChunk = nullptr;
        Vec2 cachedChunkPosition = { FLT_MAX, FLT_MAX };
        u32 _tileCount = 0, _chunkCount = 0;
        Vec2 _tileSize = Vec2::ZERO;
        i32 _contentScale = 1;
        Vec2 _mapSize = Vec2::ZERO;
        i32 _chunkSize = 0;
        Vec2 _chunkSizeInPixels = Vec2::ZERO;
        i32 _gridSize = 0;
    };
}
