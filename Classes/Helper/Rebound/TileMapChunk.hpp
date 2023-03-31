#include <axmol.h>
#include "renderer/backend/Backend.h"

#ifndef __H_TILEMAPCHUNK__
#define __H_TILEMAPCHUNK__

typedef uint64_t TileID;

#define CHUNK_SIZE 32
#define CHUNK_BUFFER_SIZE (CHUNK_SIZE*CHUNK_SIZE)

#define TILE_FLAG_ROTATE (TileID(1) << 63)
#define TILE_FLAG_FLIP_X (TileID(1) << 62)
#define TILE_FLAG_FLIP_Y (TileID(1) << 61)
#define TILE_FLAG_ALL    (TILE_FLAG_ROTATE | TILE_FLAG_FLIP_X | TILE_FLAG_FLIP_Y)
#define TILE_FLAG_NONE   ~(TILE_FLAG_ALL)

struct UV {
    float U;
    float V;
};

struct TileTexCoords {
    UV tl;
    UV tr;
    UV bl;
    UV br;

    UV saved_tl;
    UV saved_tr;
    UV saved_bl;
    UV saved_br;

    bool is90 = false;
    bool isH = false;
    bool isV = false;

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
};

class Tileset : public ax::Ref {
public:
    int _firstGid;
    ax::Texture2D* _texture;
    ax::Vec2 _tileSize;
    ax::Vec2 _textureSize;
    ax::Vec2 _sizeInPixels;
};

class TilesetArray : public ax::Ref {
public:
    TilesetArray(ax::Vec2 tileSize) {
        this->_tileSize = tileSize;
    }

    void addTileset(Tileset* tileset) {
        _tileSets.push_back(tileset);
    }

    void addTileset(ax::Texture2D* tex) {
        auto ts = new Tileset();
        ts->retain();
        ts->_texture = tex;
        addTileset(ts);
    }

    void reloadTextures() {
        for (auto& _ : _tileSets) {
            auto image = new Image();
            image->initWithImageFile(_->_texture->getPath());
            _->_texture->initWithImage(image);
        }
        _tilesetDirty = true;
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
        _tilesetDirty = true;
    }

    TileID relativeID(u16 id, TileID gid) {
        return _tileSets[id]->_firstGid + gid;
    }

    ~TilesetArray() {
        for (auto& _ : _tileSets)
            _->release();
    }

    bool _tilesetDirty = false;
    ax::Vec2 _tileSize;
    std::vector<Tileset*> _tileSets;
};

class ChunkDescriptor {
public:
    int _vertexSize;
    bool _chunkDirty = true;
    bool _isParent = false;

    ax::Mesh* _mesh;
    TileID* _tiles;
    Tileset* _tileset;
    TilesetArray* _tilesetArr;
};

namespace ChunkFactory {
    static int buildVertexIndex(TileID* tiles, Tileset* tileset, std::vector<float>& vertices, IndexArray& indices) {
        int vertexSize = 9;
        vertices.clear();
        vertices.reserve(CHUNK_SIZE * CHUNK_SIZE * vertexSize * 4 /* tiles.x * tiles.y * vertex_size * vertices */);
        indices.clear(CustomCommand::IndexFormat::U_SHORT);
        int index = 0;
        for (u8 y1 = CHUNK_SIZE; y1 > 0; y1--)
            for (u8 x1 = 0; x1 < CHUNK_SIZE; x1++)
            {
                TileID flags = 0;
                flags |= tiles[index] & TILE_FLAG_ALL;
                TileID gid = tiles[index];
                gid &= TILE_FLAG_NONE;
                gid -= tileset->_firstGid - 1;
                int maxIdRange = tileset->_textureSize.x / tileset->_tileSize.x * (tileset->_textureSize.y / tileset->_tileSize.y);;

                if (gid != 0 && gid <= maxIdRange) {
                    gid -= 1;
                    float column = ((gid) % (int)(tileset->_textureSize.x / tileset->_tileSize.x)) * tileset->_tileSize.x;
                    float row = floor((gid) / (tileset->_textureSize.x / tileset->_tileSize.x)) * tileset->_tileSize.y;
                    float columnM = (column + tileset->_tileSize.x);
                    float rowM = (row + tileset->_tileSize.y);

                    column /= tileset->_textureSize.x;
                    row /= tileset->_textureSize.y;
                    columnM /= tileset->_textureSize.x;
                    rowM /= tileset->_textureSize.y;

                    row = 1.0 - row;
                    rowM = 1.0 - rowM;

                    //float ptx = 0.0001;
                    //float pty = 0.0001;

                    //column += ptx;
                    //row -= pty;

                    //columnM -= ptx;
                    //rowM += pty;

                    unsigned short startindex = vertices.size() / vertexSize;
                    float x = x1 * tileset->_tileSize.x;
                    float y = y1 * tileset->_tileSize.y - tileset->_tileSize.y;
                    float sx = tileset->_tileSize.x;
                    float sy = tileset->_tileSize.y;
                    Color4F tc = Color4F::WHITE;

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

                    vertices.insert(vertices.end(), {
                        x, y,           0,  tc.r, tc.g, tc.b, tc.a,  coord.tl.U, coord.tl.V,
                        x + sx, y,      0,  tc.r, tc.g, tc.b, tc.a,  coord.tr.U, coord.tr.V,
                        x, y + sy,      0,  tc.r, tc.g, tc.b, tc.a,  coord.bl.U, coord.bl.V,
                        x + sx, y + sy, 0,  tc.r, tc.g, tc.b, tc.a,  coord.br.U, coord.br.V,
                        });

                    indices.insert<uint16_t>(indices.size(),
                        ilist_u16_t{ startindex, uint16_t(startindex + 3), uint16_t(startindex + 2),
                        uint16_t(startindex + 1), uint16_t(startindex + 3), startindex });
                }

                index++;
            }


        if (vertices.size() == 0)
        {
            vertices.insert(vertices.end(), {
                        0,0,0,  0,0,0,0,   0,0,
                        0,0,0,  0,0,0,0,   0,0,
                        0,0,0,  0,0,0,0,   0,0,
                        0,0,0,  0,0,0,0,   0,0,
                });

            indices.insert<uint16_t>(indices.size(), ilist_u16_t{ 0, 3, 2, 1, 3, 0 });
        }

        return vertexSize;
    }

    static ChunkDescriptor buildTiledMesh(TileID* tiles, Tileset* tileset) {
        std::vector<float> vertices;
        IndexArray indices;

        int vertexSize = buildVertexIndex(tiles, tileset, vertices, indices);

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

        auto mesh = Mesh::create(vertices, vertexSize, indices, attribs);

        ChunkDescriptor m;

        m._mesh = mesh;
        m._vertexSize = vertexSize;
        m._tiles = tiles;

        return m;
    }
};

class SingleTilesetChunkRenderer : public ChunkDescriptor, public ax::MeshRenderer {
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
        std::vector<float> vertices;
        IndexArray indices;
        ChunkFactory::buildVertexIndex(_tiles, _tileset, vertices, indices);
        _mesh->getVertexBuffer()->updateData((void*)&vertices[0], vertices.size() * sizeof(vertices[0]));
        _mesh->getIndexBuffer()->updateData((void*)indices.data(), indices.bsize());
    }

    void visit(Renderer* renderer, const Mat4& parentTransform, uint32_t parentFlags) override {
        if (_chunkDirty)
        {
            updateVertexData();
            _chunkDirty = false;
        }
        MeshRenderer::visit(renderer, parentTransform, parentFlags);
    }
};

namespace ChunkFactory {
    static SingleTilesetChunkRenderer* createChunkMeshRenderer(Tileset* tileset, ChunkDescriptor desc) {
        auto mat = ax::MeshMaterial::createBuiltInMaterial(MeshMaterial::MaterialType::QUAD_TEXTURE, false);
        tileset->_texture->setAliasTexParameters();
        mat->setTexture(tileset->_texture, ax::NTextureData::Usage::None);
        mat->setTransparent(true);
        mat->setForce2DQueue(true);
        desc._mesh->setMaterial(mat);
        auto renderer = SingleTilesetChunkRenderer::create();
        renderer->_mesh = desc._mesh;
        renderer->_tileset = tileset;
        renderer->addMesh(desc._mesh);
        return renderer;
    }

};

class ChunkRenderer : public ChunkDescriptor, public ax::Node {
public:
    static ChunkRenderer* create(ChunkDescriptor desc) {
        auto ref = new ChunkRenderer();
        if (ref->init()) {
            ref->_isParent = true;
            ref->_mesh = nullptr;

            ref->_tiles = desc._tiles;
            ref->_tilesetArr = desc._tilesetArr;
            bool _chunkDirty = true;

            bool _isParent = false;
            std::vector<TilesetArray*> _tileSets;

            ref->autorelease();
            return ref;
        }
        AX_SAFE_DELETE(ref);
        return nullptr;
    }

    ~ChunkRenderer() {
        AX_SAFE_FREE(_tiles);
    }

    int count = 0;

    void resizeChunkCount() {
        if (count != _tilesetArr->_tileSets.size())
        {
            for (auto& c : _chunks)
                c->removeFromParent();
            _chunks.clear();
            for (auto& _ : _tilesetArr->_tileSets)
            {
                auto cd = ChunkFactory::buildTiledMesh(_tiles, _);
                auto c = ChunkFactory::createChunkMeshRenderer(_, cd);
                c->_tiles = _tiles;
                _chunks.push_back(c);
            }
            count = _tilesetArr->_tileSets.size();
        }
    }

    void visit(Renderer* renderer, const Mat4& parentTransform, uint32_t parentFlags) override {
        resizeChunkCount();
        int sum = 0;
        for (TileID id = 0; id < CHUNK_BUFFER_SIZE; id++)
            sum |= _tiles[id];
        if (count == 0 || sum == 0)
            return;

        auto cam = Camera::getVisitingCamera();
        auto cam_aabb = Rect(cam->getPosition().x - 640 * cam->getZoom(), cam->getPosition().y - 360 * cam->getZoom(),
            cam->getPosition().x + 640 * cam->getZoom(), cam->getPosition().y + 360 * cam->getZoom());

        auto pos = getPosition();
        auto aabb = Rect(pos.x, pos.y, pos.x + _tilesetArr->_tileSets[0]->_sizeInPixels.x, pos.y + _tilesetArr->_tileSets[0]->_sizeInPixels.y);

        if (!cam_aabb.intersectsRect(aabb))
            return;

        for (auto& _ : _chunks)
        {
            if (_->getParent() != this)
                addChild(_);
            if (_chunkDirty || _tilesetArr->_tilesetDirty)
                _->_chunkDirty = true;
            _->visit(renderer, parentTransform, parentFlags);
            _tilesetArr->_tilesetDirty = false;
            _chunkDirty = false;
        }
    }

    std::vector<SingleTilesetChunkRenderer*> _chunks;
};

#endif
