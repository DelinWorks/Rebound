#include <axmol.h>
#include "renderer/backend/Backend.h"
#include "Helper/short_types.h"

#ifndef __H_TILEMAPCHUNK__
#define __H_TILEMAPCHUNK__

using namespace ax;

namespace TileSystem {

class ChunkRenderMethod {
public:
    ChunkRenderMethod() {}
    bool _resize = true;
    virtual void cacheVertices(bool _resize) = 0;
};

typedef u32 TileID;

#define CHUNK_SIZE 32.0
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
        void fill() {
            if (!_isFill) {
                vertices.clear();
                vertices.reserve(CHUNK_SIZE * CHUNK_SIZE * VERTEX_SIZE_NO_ANIMATIONS * 4 /* tiles.x * tiles.y * vertex_size * vertices */);
                indices.clear(CustomCommand::IndexFormat::U_SHORT);
                for (u8 y1 = CHUNK_SIZE; y1 > 0; y1--)
                    for (u8 x1 = 0; x1 < CHUNK_SIZE; x1++) {
                        u16 startindex = vertices.size() / VERTEX_SIZE_NO_ANIMATIONS;

                        vertices.insert(vertices.end(), {
                            0,0,0,  0,0,0,0,   0,0,
                            0,0,0,  0,0,0,0,   0,0,
                            0,0,0,  0,0,0,0,   0,0,
                            0,0,0,  0,0,0,0,   0,0,
                            });

                        indices.insert<u16>(indices.size(),
                            ilist_u16_t{ startindex, u16(startindex + 3), u16(startindex + 2),
                            u16(startindex + 1), u16(startindex + 3), startindex });
                    }

                _isFill = true;
            }
        }

        const std::vector<f32>& getVertex() { fill(); return vertices; };
        const IndexArray& getIndex() { fill(); return indices; };

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

        void save() {
            if (!isSaved) {
                isSaved = true;
                saved_tl = tl;
                saved_tr = tr;
                saved_bl = bl;
                saved_br = br;
            }
        }

        void reset() {
            save();
            tl = saved_tl;
            tr = saved_tr;
            bl = saved_bl;
            br = saved_br;
            is90 = isH = isV = false;
        }

        void rotate90() {
            is90 = !is90;
            save();
            std::swap(br, bl);
            std::swap(tr, br);
            std::swap(tl, tr);
        }

        void flipH() {
            isH = !isH;
            save();
            std::swap(tl, tr);
            std::swap(bl, br);
        }

        void flipV() {
            isV = !isV;
            save();
            std::swap(tl, bl);
            std::swap(tr, br);
        }

        i8 rotation = 0;

        void cw() {
            reset();

            rotation++;

            if (rotation > 3)
                rotation = 0;

            if (rotation == 1) {
                rotate90();
                flipH();
                flipV();
            }
            else if (rotation == 2) {
                flipV();
                flipH();
            }
            else if (rotation == 3) {
                rotate90();
            }
        }

        void ccw() {
            reset();

            rotation--;

            if (rotation < 0)
                rotation = 3;

            if (rotation == 1) {
                rotate90();
                flipH();
                flipV();
            }
            else if (rotation == 2) {
                flipV();
                flipH();
            }
            else if (rotation == 3) {
                rotate90();
            }
        }

        TileID state() {
            TileID gid = 0;
            gid |= isH ? TILE_FLAG_FLIP_X : 0;
            gid |= isV ? TILE_FLAG_FLIP_Y : 0;
            gid |= is90 ? TILE_FLAG_ROTATE : 0;
            return gid;
        }

        bool _outOfRange = false;
    };

    class Tileset : public ax::Ref {
    public:
        u32 _firstGid = 0;
        ax::Texture2D* _texture;
        ax::Vec2 _tileSize;
        ax::Vec2 _textureSize;
        ax::Vec2 _sizeInPixels;
        MeshMaterial* _material;

        Tileset(ax::Texture2D* _texture) {
            this->_texture = _texture;
            _texture->setAliasTexParameters();
            _material = ax::MeshMaterial::createBuiltInMaterial(ax::MeshMaterial::MaterialType::QUAD_TEXTURE, false);
            _material->setTexture(_texture, ax::NTextureData::Usage::None);
            _material->setTransparent(true);
            _material->setForce2DQueue(true);
            _material->getStateBlock().setCullFace(true);
            _material->getStateBlock().setCullFaceSide(ax::CullFaceSide::NONE);
            _material->retain();
        }

        ~Tileset() {
            _material->release();
            LOG_RELEASE;
        }
    };

    class TilesetArray : public ax::Ref {
    public:
        static TilesetArray* create(ax::Vec2 tileSize) {
            auto ref = new TilesetArray();
            if (ref) {
                ref->autorelease();
                ref->_tileSize = tileSize;
                return ref;
            }
            return nullptr;
        }

        void addTileset(Tileset* tileset) {
            _tileSets.push_back(tileset);
        }

        void addTileset(ax::Texture2D* tex) {
            auto ts = new Tileset(tex);
            ts->autorelease();
            ts->retain();
            addTileset(ts);
        }

        void addTextureBleedTileset(ax::Image* image) {
            //image->getData();
            //auto bleed = new Image();
            //int nWidth = image->getWidth() + image->getWidth() / _tileSize.x * 2;
            //int nHeight = image->getHeight() + image->getHeight() / _tileSize.y * 2;
            //int dataLen = nWidth * nHeight * 4;
            //uint8_t* data = (uint8_t*)malloc(dataLen);
            //memset(data, 255, dataLen);
            //for (int x = 0; x < image->getWidth(); x++)
            //    for (int y = 0; y < image->getHeight(); y++) {
            //        data[(x * image->getWidth() + y) * 4] = 0;
            //    }
            //bleed->initWithRawData(data, dataLen, nWidth, nHeight, 0);
            //bleed->saveToFile("C:\\Users\\turky\\Desktop\\output.png", false);
            //free(data);
            ////auto ts = new Tileset();
            ////ts->autorelease();
            ////ts->retain();
            ////addTileset(ts);
        }

        void reloadTextures() {
            for (auto& _ : _tileSets) {
                auto image = new Image();
                image->autorelease();
                image->initWithImageFile(_->_texture->getPath());
                _->_texture->initWithImage(image);
            }
            retainedChunksI = retainedChunks;
        }

        void calculateBounds() {
            u16 index = 0;
            u32 total = 0;
            for (auto& _ : _tileSets)
            {
                _->_tileSize = _tileSize;
                _->_sizeInPixels = _tileSize * CHUNK_SIZE;
                _->_textureSize = Vec2(_->_texture->getPixelsWide(), _->_texture->getPixelsHigh());
                _->_firstGid = total + 1;
                total += _->_textureSize.x / _tileSize.x * (_->_textureSize.y / _tileSize.y);
            }
            retainedChunksI = retainedChunks;
        }

        TileID relativeID(u16 id, TileID gid) {
            return _tileSets[id]->_firstGid + gid;
        }

        ~TilesetArray() {
            for (auto& _ : _tileSets)
                AX_SAFE_RELEASE(_);
            _tileSets.clear();
            LOG_RELEASE;
        }

        i32 retainedChunks = 0;
        i32 retainedChunksI = 0;
        ax::Vec2 _tileSize;
        std::vector<Tileset*> _tileSets;
    };

    class TileArray : public ax::Ref {
    public:
        static TileArray* create(TileID* _tiles) {
            auto ref = new TileArray();
            if (ref) {
                ref->autorelease();
                ref->_tiles = _tiles;
                return ref;
            }
            return nullptr;
        }

        TileID* getArrayPointer(bool dirty = false) {
            if (dirty) {
                _tileArrayDirty = true;
                retainedChunksI = retainedChunks;
            }
            return _tiles;
        }

        void update() {
            if (cachedTilesetArr && _tileArrayDirty) {
                _tileArrayDirty = false;
                _emptyTilesets.clear();
                for (auto& _ : cachedTilesetArr->_tileSets) {
                    bool empty = true;
                    for (TileID i = 0; i < CHUNK_BUFFER_SIZE; i++) {
                        TileID gid = _tiles[i];
                        gid &= TILE_FLAG_NONE;
                        gid -= _->_firstGid - 1;
                        i32 maxIdRange = _->_textureSize.x / _->_tileSize.x * (_->_textureSize.y / _->_tileSize.y);
                        if (gid != 0 && gid <= maxIdRange) {
                            empty = false;
                            break;
                        }
                    }
                    if (empty)
                        _emptyTilesets.insert(_->_firstGid);
                }
            }
        }

        bool isEmpty(i32 firstGid) {
            return _emptyTilesets.find(firstGid) != _emptyTilesets.end();
        }

        ~TileArray() {
            AX_SAFE_FREE(_tiles);
            for (auto& [_, v] : vertexCache)
                v.clear();
            vertexCache.clear();
            LOG_RELEASE;
        }

        //protected:
        TilesetArray* cachedTilesetArr = nullptr;
        bool _tileArrayDirty = false;
        std::set<i32> _emptyTilesets;
        std::map<i32, std::vector<f32>> vertexCache;
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
        static TileTexCoords calculateTileCoords(TileID id, Tileset* tileset) {
            TileID flags = 0;
            flags |= id & TILE_FLAG_ALL;
            TileID gid = id;
            gid &= TILE_FLAG_NONE;
            gid -= tileset->_firstGid - 1;
            i32 maxIdRange = tileset->_textureSize.x / tileset->_tileSize.x * (tileset->_textureSize.y / tileset->_tileSize.y);
            if (gid != 0 && gid <= maxIdRange) {
                gid -= 1;
                f32 column = ((gid) % (i32)(tileset->_textureSize.x / tileset->_tileSize.x)) * tileset->_tileSize.x;
                f32 row = floor((gid) / (tileset->_textureSize.x / tileset->_tileSize.x)) * tileset->_tileSize.y;
                f32 columnM = (column + tileset->_tileSize.x);
                f32 rowM = (row + tileset->_tileSize.y);

#if USE_COLOR_BLEED_TOLERANCE == 1
                auto marginX = 1.0 / (tileset->_textureSize.x - 1.0);
                auto marginY = 1.0 / (tileset->_textureSize.y - 1.0);

                column /= tileset->_textureSize.x - marginX;
                row /= tileset->_textureSize.y - marginY;
                columnM /= tileset->_textureSize.x + marginX;
                rowM /= tileset->_textureSize.y + marginY;
#else
                column /= tileset->_textureSize.x;
                row /= tileset->_textureSize.y;
                columnM /= tileset->_textureSize.x;
                rowM /= tileset->_textureSize.y;
#endif

                row = 1.0 - row;
                rowM = 1.0 - rowM;

                TileTexCoords coord{
                    { column,  rowM },
                    { columnM, rowM },
                    { column,  row  },
                    { columnM, row  },
                };

                if (flags & TILE_FLAG_ROTATE)
                    coord.rotate90();
                if (flags & TILE_FLAG_FLIP_X)
                    coord.flipH();
                if (flags & TILE_FLAG_FLIP_Y)
                    coord.flipV();

                coord._outOfRange = false;
                return coord;
            }
            else {
                TileTexCoords t;
                t._outOfRange = true;
                return t;
            }
        }

        static i32 buildVertexIndex(TileArray* tileArr, Tileset* tileset, std::vector<f32>& vertices, IndexArray& indices, bool resize) {
            i32 vertexSize = VERTEX_SIZE_NO_ANIMATIONS;
            vertices.clear();
            vertices.reserve(CHUNK_SIZE * CHUNK_SIZE * vertexSize * 4 /* tiles.x * tiles.y * vertex_size * vertices */);
            indices.clear(CustomCommand::IndexFormat::U_SHORT);
            i32 index = 0;
            for (u8 y1 = CHUNK_SIZE; y1 > 0; y1--)
                for (u8 x1 = 0; x1 < CHUNK_SIZE; x1++)
                {
                    auto tiles = tileArr->getArrayPointer(false);

                    u16 startindex = vertices.size() / vertexSize;

#if USE_VERTEX_BLEED_TOLERANCE == 1
                    auto marginX = 1.0 / tileset->_tileSize.x;
                    auto marginY = 1.0 / tileset->_tileSize.y;

                    f32 x = (x1 * tileset->_tileSize.x) - marginX;
                    f32 y = (y1 * tileset->_tileSize.y - tileset->_tileSize.y) - marginY;
                    f32 sx = tileset->_tileSize.x + marginX * 2;
                    f32 sy = tileset->_tileSize.y + marginY * 2;
#else
                    f32 x = x1 * tileset->_tileSize.x;
                    f32 y = y1 * tileset->_tileSize.y - tileset->_tileSize.y;
                    f32 sx = tileset->_tileSize.x;
                    f32 sy = tileset->_tileSize.y;
#endif

                    auto coord = calculateTileCoords(tiles[index], tileset);

                    if (!coord._outOfRange) {
                        Color4F tc = Color4F::WHITE;
                        float zPos = Random::rangeFloat(-1000, 1000);
                        vertices.insert(vertices.end(), {
                            x, y,           zPos,  tc.r, tc.g, tc.b, tc.a,  coord.tl.U, coord.tl.V,
                            x + sx, y,      zPos,  tc.r, tc.g, tc.b, tc.a,  coord.tr.U, coord.tr.V,
                            x, y + sy,      zPos,  tc.r, tc.g, tc.b, tc.a,  coord.bl.U, coord.bl.V,
                            x + sx, y + sy, zPos,  tc.r, tc.g, tc.b, tc.a,  coord.br.U, coord.br.V,
                            });

                        indices.insert<u16>(indices.size(),
                            ilist_u16_t{ startindex, u16(startindex + 3), u16(startindex + 2),
                            u16(startindex + 1), u16(startindex + 3), startindex });
                    }
                    else if (!resize) {
                        u16 startindex = vertices.size() / vertexSize;
                        f32 x = x1 * tileset->_tileSize.x;
                        f32 y = y1 * tileset->_tileSize.y - tileset->_tileSize.y;
                        f32 sx = tileset->_tileSize.x;
                        f32 sy = tileset->_tileSize.y;
                        Color4F tc = Color4F::WHITE;

                        vertices.insert(vertices.end(), {
                            x, y,            0,  tc.r, tc.g, tc.b, tc.a,   0,0,
                            x + sx, y,       0,  tc.r, tc.g, tc.b, tc.a,   0,0,
                            x, y + sy,       0,  tc.r, tc.g, tc.b, tc.a,   0,0,
                            x + sx, y + sy,  0,  tc.r, tc.g, tc.b, tc.a,   0,0,
                            });

                        indices.insert<u16>(indices.size(),
                            ilist_u16_t{ startindex, u16(startindex + 3), u16(startindex + 2),
                            u16(startindex + 1), u16(startindex + 3), startindex });
                    }

                    index++;
                }

            //if (vertices.size() != 0)
            //    RLOG("Chunk Model Built with a Stride of {} has {} Vertices, {} Indices, Approx {}KiB Memory Taken", VERTEX_SIZE_NO_ANIMATIONS, vertices.size(), indices.size(),
            //        (vertices.size() + indices.size()) * sizeof(float) * VERTEX_SIZE_NO_ANIMATIONS);

            return vertexSize;
        }

        static Mesh* buildTiledMesh(TileArray* _tileArr, Tileset* _tileset, bool _resize) {
            std::vector<MeshVertexAttrib> attribs;
            MeshVertexAttrib att;

            att.type = backend::VertexFormat::FLOAT3;
            att.vertexAttrib = shaderinfos::VertexKey::VERTEX_ATTRIB_POSITION;
            attribs.push_back(att);

            att.type = backend::VertexFormat::FLOAT4;
            att.vertexAttrib = shaderinfos::VertexKey::VERTEX_ATTRIB_COLOR;
            attribs.push_back(att);

            att.type = backend::VertexFormat::FLOAT2;
            att.vertexAttrib = shaderinfos::VertexKey::VERTEX_ATTRIB_TEX_COORD;
            attribs.push_back(att);

            if (_resize)
            {
                std::vector<float> vertices;
                IndexArray indices;
                buildVertexIndex(_tileArr, _tileset, vertices, indices, true);
                if (vertices.size() == 0)
                    return nullptr;
                return Mesh::create(vertices, VERTEX_SIZE_NO_ANIMATIONS, indices, attribs);
            }
            else return Mesh::create(emptyVIC.getVertex(), VERTEX_SIZE_NO_ANIMATIONS, emptyVIC.getIndex(), attribs);
        }

        /* Build vertex cache for a specified TileArray* object.
        * This helps reduce build time of a chunk by directly
        updating the vertex buffer with the needed data using
        a vertex cache map that stores caches based on first gid.
        */
        static void buildVertexCache(TileArray* tiles, TilesetArray* tilesets, bool _resize) {
            if (_resize) return;

            tiles->getArrayPointer(true);
            tiles->vertexCache.clear();
            tiles->cachedTilesetArr = tilesets;

            for (auto& _ : tilesets->_tileSets) {
                std::vector<f32> vertices;
                IndexArray indices;
                ChunkFactory::buildVertexIndex(tiles, _, vertices, indices, false);
                tiles->vertexCache.emplace(_->_firstGid, vertices);
            }
        }

        // This modifies the vertex cache directly for better performance
        // Returns true if the previuous tile had a tile gid of 0
        static bool setTile(TileArray* tiles, TileID index, TileID newGid, bool _resize) {
            auto _tiles = tiles->getArrayPointer(true);
            auto prevT = _tiles[index];
            _tiles[index] = newGid;
            if (_resize) return prevT == 0;

            for (auto& _ : tiles->cachedTilesetArr->_tileSets) {
                auto& vertices = tiles->vertexCache[_->_firstGid];
                auto coord = calculateTileCoords(newGid, _);
                i32 startIndex = index * VERTEX_SIZE_NO_ANIMATIONS * 4;
                if (coord._outOfRange) coord = { 0,0,0,0 };
                vertices[(7 + startIndex) + VERTEX_SIZE_NO_ANIMATIONS * 0] = coord.tl.U;
                vertices[(8 + startIndex) + VERTEX_SIZE_NO_ANIMATIONS * 0] = coord.tl.V;
                vertices[(7 + startIndex) + VERTEX_SIZE_NO_ANIMATIONS * 1] = coord.tr.U;
                vertices[(8 + startIndex) + VERTEX_SIZE_NO_ANIMATIONS * 1] = coord.tr.V;
                vertices[(7 + startIndex) + VERTEX_SIZE_NO_ANIMATIONS * 2] = coord.bl.U;
                vertices[(8 + startIndex) + VERTEX_SIZE_NO_ANIMATIONS * 2] = coord.bl.V;
                vertices[(7 + startIndex) + VERTEX_SIZE_NO_ANIMATIONS * 3] = coord.br.U;
                vertices[(8 + startIndex) + VERTEX_SIZE_NO_ANIMATIONS * 3] = coord.br.V;
            }

            return prevT == 0;
        }
    };

    class SingleTilesetChunkRenderer : public ChunkDescriptor, public ChunkRenderMethod, public ax::MeshRenderer {
    public:
        static SingleTilesetChunkRenderer* create() {
            auto ref = new SingleTilesetChunkRenderer();
            if (ref->init()) {
                ref->autorelease();
                return ref;
            }
            AX_SAFE_DELETE(ref);
            return nullptr;
        }

        void updateVertexData() {
            AX_ASSERT(_tiles->vertexCache[_tileset->_firstGid].size() == CHUNK_BUFFER_SIZE * 36, "Vertex cache was not built.");
            auto vertices = _tiles->vertexCache[_tileset->_firstGid];
            _mesh->getVertexBuffer()->updateSubData((void*)&vertices[0], 0, vertices.size() * sizeof(vertices[0]));
        }

        ~SingleTilesetChunkRenderer() {
            //AX_SAFE_RELEASE_NULL(_mesh);
            LOG_RELEASE;
        }

        void unload() {
            removeAllMeshes();
            _mesh = nullptr;
            _chunkDirty = true;
        }

        void visit(Renderer* renderer, const Mat4& parentTransform, u32 parentFlags) override {
            if (_chunkDirty)
            {
                if (_resize) {
                    removeAllMeshes();
                    _mesh = ChunkFactory::buildTiledMesh(_tiles, _tileset, _resize);
                }
                if (_mesh && _resize || !_mesh && !_resize) {
                    if (!_resize)
                        _mesh = ChunkFactory::buildTiledMesh(_tiles, _tileset, _resize);
                    _mesh->setMaterial(_tileset->_material);
                    addMesh(_mesh);
                }
                if (_mesh && !_resize) {
                    updateVertexData();
                }
                _chunkDirty = false;
            }
            if (_mesh && getDisplayedOpacity())
                MeshRenderer::visit(renderer, parentTransform, parentFlags);
        }

        void cacheVertices(bool _resize) {
            this->_resize = _resize;
        }
    };

    class ChunkRenderer : public ChunkDescriptor, public ChunkRenderMethod, public ax::Node {
    public:
        static ChunkRenderer* create(ChunkDescriptor desc) {
            auto ref = new ChunkRenderer();
            if (ref->init()) {
                ref->autorelease();

                ref->_isParent = true;
                ref->_mesh = nullptr;

                ref->_tiles = desc._tiles;
                ref->_tilesetArr = desc._tilesetArr;
                bool _chunkDirty = true;

                desc._tiles->retainedChunks++;
                desc._tiles->retain();
                desc._tilesetArr->retainedChunks++;
                desc._tilesetArr->retain();

                ref->setCascadeOpacityEnabled(true);

                return ref;
            }
            AX_SAFE_DELETE(ref);
            return nullptr;
        }

        ~ChunkRenderer() {
            _tiles->retainedChunks--;
            AX_SAFE_RELEASE(_tiles);
            _tilesetArr->retainedChunks--;
            AX_SAFE_RELEASE(_tilesetArr);
            LOG_RELEASE;
        }

        i32 count = 0;

        i32 resizeChunkCount() {
            if (count != _tilesetArr->_tileSets.size())
            {
                for (auto& c : _chunks)
                    c->removeFromParent();
                _chunks.clear();
                for (auto& _ : _tilesetArr->_tileSets)
                {
                    auto c = SingleTilesetChunkRenderer::create();
                    c->_mesh = nullptr;
                    c->_vertexSize = VERTEX_SIZE_NO_ANIMATIONS;
                    c->_tiles = _tiles;
                    c->_tileset = _;
                    c->_chunkDirty = true;
                    c->cacheVertices(_resize);
                    addChild(c);
                    _chunks.push_back(c);
                    c->_tiles = _tiles;
                }
                count = _tilesetArr->_tileSets.size();
            }
            return count;
        }

        ax::Vec2 _pos = Vec2::ZERO;

        void setPositionInChunkSpace(ax::Vec2 pos) {
            setPositionInChunkSpace(pos.x, pos.y);
        }

        void setPositionInChunkSpace(f32 x, f32 y) {
            _pos = { x,y };
        }

        void draw(Renderer* renderer, const Mat4& parentTransform, u32 parentFlags) override {}
        void visit(Renderer* renderer, const Mat4& parentTransform, u32 parentFlags) override {}
        void visit(Renderer* renderer, const Mat4& parentTransform, u32 parentFlags, u32* renderCount) {
            auto tiles = _tiles->getArrayPointer();
            if (!resizeChunkCount())
                return;

            auto cam = tileMapVirtualCamera;
            if (!cam) return;

            auto cam_aabb = Rect(cam->getPosition().x - 640 * cam->getScale(), cam->getPosition().y - 360 * cam->getScale(),
                cam->getPosition().x + 640 * cam->getScale(), cam->getPosition().y + 360 * cam->getScale());

            auto pos = Vec2(_pos.x * _tilesetArr->_tileSets[0]->_sizeInPixels.x, _pos.y * _tilesetArr->_tileSets[0]->_sizeInPixels.y);
            auto aabb = Rect(pos.x, pos.y, pos.x + _tilesetArr->_tileSets[0]->_sizeInPixels.x, pos.y + _tilesetArr->_tileSets[0]->_sizeInPixels.y);

            if (!cam_aabb.intersectsRect(aabb)) {
                _tilesetArr->retainedChunksI--;
                _tiles->retainedChunksI--;
                //for (auto& _ : _chunks)
                //    _->unload();
                return;
            }

            _tiles->update();
            for (auto& _ : _chunks)
            {
                if (_tiles->isEmpty(_->_tileset->_firstGid)) continue;
                if (_chunkDirty || _tiles->retainedChunksI > 0 || _tilesetArr->retainedChunksI > 0)
                    _->_chunkDirty = true;
                Mat4 transform = Mat4::IDENTITY;
                transform.multiply(parentTransform);
                transform.translate(ax::Vec3(pos.x, pos.y, 0));
                transform.scale({ 1, 1, zPositionMultiplier });

                // We Round the x,y translation axis of the Matrix
                // To achieve Pixel Perfect Rendering for TileMaps
                transform.m[12] = round(transform.m[12]);
                transform.m[13] = round(transform.m[13]);
                // *  1  0  0  round(Tx)
                // *  0  1  0  round(Ty)
                // *  0  0  1  Tz
                // *  0  0  0  1

                _->visit(renderer, transform, parentFlags);
                //(*renderCount)++;
                _chunkDirty = false;
            }
            _tilesetArr->retainedChunksI--;
            _tiles->retainedChunksI--;
        }

        void cacheVertices(bool _resize) {
            this->_resize = _resize;
            for (auto& _ : _chunks)
                _->cacheVertices(_resize);
        }

        std::vector<SingleTilesetChunkRenderer*> _chunks;
    };
}

#endif
