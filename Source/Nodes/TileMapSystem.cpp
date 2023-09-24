#include "TileMapSystem.h"

using namespace TileSystem;

void EmptyVertexIndexCache::fill() {
    if (!_isFill) {
        vertices.clear();
        vertices.reserve(CHUNK_SIZE * CHUNK_SIZE * VERTEX_SIZE_NO_ANIMATIONS * 4 /* tiles.x * tiles.y * vertex_size * vertices */);
        indices.clear(CustomCommand::IndexFormat::U_SHORT);
        for (U8 y1 = CHUNK_SIZE; y1 > 0; y1--)
            for (U8 x1 = 0; x1 < CHUNK_SIZE; x1++) {
                U16 startindex = vertices.size() / VERTEX_SIZE_NO_ANIMATIONS;

                vertices.insert(vertices.end(), {
                    0,0,0,  0,0,0,0,   0,0,
                    0,0,0,  0,0,0,0,   0,0,
                    0,0,0,  0,0,0,0,   0,0,
                    0,0,0,  0,0,0,0,   0,0,
                    });

                indices.insert<U16>(indices.size(),
                    ilist_u16_t{ startindex, U16(startindex + 3), U16(startindex + 2),
                    U16(startindex + 1), U16(startindex + 3), startindex });
            }

        _isFill = true;
    }
}

const std::vector<F32>& EmptyVertexIndexCache::getVertex() { fill(); return vertices; };
const IndexArray& EmptyVertexIndexCache::getIndex() { fill(); return indices; };

void TileTexCoords::save() {
    if (!isSaved) {
        isSaved = true;
        saved_tl = tl;
        saved_tr = tr;
        saved_bl = bl;
        saved_br = br;
    }
}

void TileTexCoords::reset() {
    save();
    tl = saved_tl;
    tr = saved_tr;
    bl = saved_bl;
    br = saved_br;
    is90 = isH = isV = false;
}

void TileTexCoords::rotate90() {
    is90 = !is90;
    save();
    std::swap(br, bl);
    std::swap(tr, br);
    std::swap(tl, tr);
}

void TileTexCoords::flipH() {
    isH = !isH;
    save();
    std::swap(tl, tr);
    std::swap(bl, br);
}

void TileTexCoords::flipV() {
    isV = !isV;
    save();
    std::swap(tl, bl);
    std::swap(tr, br);
}

void TileTexCoords::cw() {
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

void TileTexCoords::ccw() {
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

TileID TileTexCoords::state() {
    TileID gid = 0;
    gid |= isH ? TILE_FLAG_FLIP_X : 0;
    gid |= isV ? TILE_FLAG_FLIP_Y : 0;
    gid |= is90 ? TILE_FLAG_ROTATE : 0;
    return gid;
}

Tileset::Tileset(ax::Texture2D* _texture) {
    this->_texture = _texture;
    _texture->setAliasTexParameters();
}

Tileset::~Tileset() {
    LOG_RELEASE;
}

TilesetArray* TilesetArray::create(V2D tileSize) {
    auto ref = new TilesetArray();
    if (ref) {
        ref->autorelease();
        ref->_tileSize = tileSize;
        return ref;
    }
    return nullptr;
}

void TilesetArray::addTileset(Tileset* tileset) {
    _tileSets.push_back(tileset);
}

void TilesetArray::addTileset(ax::Texture2D* tex) {
    auto ts = new Tileset(tex);
    ts->autorelease();
    ts->retain();
    addTileset(ts);
}

void TilesetArray::reloadTextures() {
    for (auto& _ : _tileSets) {
        auto image = new Image();
        image->autorelease();
        image->initWithImageFile(_->_texture->getPath());
        _->_texture->initWithImage(image);
    }
    retainedChunksI = retainedChunks;
}

void TilesetArray::calculateBounds() {
    U16 index = 0;
    U32 total = 0;
    for (auto& _ : _tileSets)
    {
        _->_tileSize = _tileSize;
        _->_sizeInPixels = _tileSize * CHUNK_SIZE;
        _->_textureSize = V2D(_->_texture->getPixelsWide(), _->_texture->getPixelsHigh());
        _->_firstGid = total + 1;
        total += _->_textureSize.x / _tileSize.x * (_->_textureSize.y / _tileSize.y);
    }
    retainedChunksI = retainedChunks;
}

TileID TilesetArray::relativeID(U16 id, TileID gid) {
    return _tileSets[id]->_firstGid + gid;
}

TilesetArray::~TilesetArray() {
    for (auto& _ : _tileSets)
        RB_PROMISE_RELEASE(_);
    _tileSets.clear();
    LOG_RELEASE;
}

TileArray* TileArray::create(TileID* _tiles) {
    auto ref = new TileArray();
    if (ref) {
        ref->autorelease();
        ref->_tiles = _tiles;
        return ref;
    }
    return nullptr;
}

TileID* TileArray::getArrayPointer(bool dirty) {
    if (dirty) {
        _tileArrayDirty = true;
        retainedChunksI = retainedChunks;
    }
    return _tiles;
}

void TileArray::update() {
    if (cachedTilesetArr && _tileArrayDirty) {
        _tileArrayDirty = false;
        _emptyTilesets.clear();
        for (auto& _ : cachedTilesetArr->_tileSets) {
            bool empty = true;
            for (TileID i = 0; i < CHUNK_BUFFER_SIZE; i++) {
                TileID gid = _tiles[i];
                gid &= TILE_FLAG_NONE;
                gid -= _->_firstGid - 1;
                I32 maxIdRange = _->_textureSize.x / _->_tileSize.x * (_->_textureSize.y / _->_tileSize.y);
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

bool TileArray::isEmpty(I32 firstGid) {
    return _emptyTilesets.find(firstGid) != _emptyTilesets.end();
}

TileArray::~TileArray() {
    AX_SAFE_FREE(_tiles);
    for (auto& [_, v] : vertexCache)
        v.clear();
    vertexCache.clear();
    LOG_RELEASE;
}

TileTexCoords ChunkFactory::calculateTileCoords(TileID id, Tileset* tileset) {
    TileID flags = 0;
    flags |= id & TILE_FLAG_ALL;
    TileID gid = id;
    gid &= TILE_FLAG_NONE;
    gid -= tileset->_firstGid - 1;
    I32 maxIdRange = tileset->_textureSize.x / tileset->_tileSize.x * (tileset->_textureSize.y / tileset->_tileSize.y);
    if (gid != 0 && gid <= maxIdRange) {
        gid -= 1;
        F32 column = ((gid) % (I32)(tileset->_textureSize.x / tileset->_tileSize.x)) * tileset->_tileSize.x;
        F32 row = floor((gid) / (tileset->_textureSize.x / tileset->_tileSize.x)) * tileset->_tileSize.y;
        F32 columnM = (column + tileset->_tileSize.x);
        F32 rowM = (row + tileset->_tileSize.y);

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

I32 ChunkFactory::buildVertexIndex(TileArray* tileArr, Tileset* tileset, std::vector<F32>& vertices, IndexArray& indices, bool resize) {
    I32 vertexSize = VERTEX_SIZE_NO_ANIMATIONS;
    vertices.clear();
    vertices.reserve(CHUNK_SIZE * CHUNK_SIZE * vertexSize * 4 /* tiles.x * tiles.y * vertex_size * vertices */);
    indices.clear(CustomCommand::IndexFormat::U_SHORT);
    I32 index = 0;
    for (U8 y1 = CHUNK_SIZE; y1 > 0; y1--)
        for (U8 x1 = 0; x1 < CHUNK_SIZE; x1++)
        {
            auto tiles = tileArr->getArrayPointer(false);

            U16 startindex = vertices.size() / vertexSize;

#if USE_VERTEX_BLEED_TOLERANCE == 1
            auto marginX = 1.0 / (tileset->_textureSize.x * tileset->_tileSize.x);
            auto marginY = 1.0 / (tileset->_textureSize.y * tileset->_tileSize.y);

            F32 x = (x1 * tileset->_tileSize.x) - marginX;
            F32 y = (y1 * tileset->_tileSize.y - tileset->_tileSize.y) - marginY;
            F32 sx = tileset->_tileSize.x + marginX * 2;
            F32 sy = tileset->_tileSize.y + marginY * 2;
#else
            F32 x = x1 * tileset->_tileSize.x;
            F32 y = y1 * tileset->_tileSize.y - tileset->_tileSize.y;
            F32 sx = tileset->_tileSize.x;
            F32 sy = tileset->_tileSize.y;
#endif

            auto coord = calculateTileCoords(tiles[index], tileset);

            if (!coord._outOfRange) {
                Color4F tc = Color4F::WHITE;
                float zPos = Random::rangeFloat(-10, 10);
                vertices.insert(vertices.end(), {
                    x, y,           zPos,  tc.r, tc.g, tc.b, tc.a,  coord.tl.U, coord.tl.V,
                    x + sx, y,      zPos,  tc.r, tc.g, tc.b, tc.a,  coord.tr.U, coord.tr.V,
                    x, y + sy,      zPos,  tc.r, tc.g, tc.b, tc.a,  coord.bl.U, coord.bl.V,
                    x + sx, y + sy, zPos,  tc.r, tc.g, tc.b, tc.a,  coord.br.U, coord.br.V,
                    });

                indices.insert<U16>(indices.size(),
                    ilist_u16_t{ startindex, U16(startindex + 3), U16(startindex + 2),
                    U16(startindex + 1), U16(startindex + 3), startindex });
            }
            else if (!resize) {
                U16 startindex = vertices.size() / vertexSize;
                F32 x = x1 * tileset->_tileSize.x;
                F32 y = y1 * tileset->_tileSize.y - tileset->_tileSize.y;
                F32 sx = tileset->_tileSize.x;
                F32 sy = tileset->_tileSize.y;
                Color4F tc = Color4F::WHITE;

                vertices.insert(vertices.end(), {
                    x, y,            0,  tc.r, tc.g, tc.b, tc.a,   0,0,
                    x + sx, y,       0,  tc.r, tc.g, tc.b, tc.a,   0,0,
                    x, y + sy,       0,  tc.r, tc.g, tc.b, tc.a,   0,0,
                    x + sx, y + sy,  0,  tc.r, tc.g, tc.b, tc.a,   0,0,
                    });

                indices.insert<U16>(indices.size(),
                    ilist_u16_t{ startindex, U16(startindex + 3), U16(startindex + 2),
                    U16(startindex + 1), U16(startindex + 3), startindex });
            }

            index++;
        }

    //if (vertices.size() != 0)
    //    RLOG("Chunk Model Built with a Stride of {} has {} Vertices, {} Indices, Approx {}KiB Memory Taken", VERTEX_SIZE_NO_ANIMATIONS, vertices.size(), indices.size(),
    //        (vertices.size() + indices.size()) * sizeof(float) * VERTEX_SIZE_NO_ANIMATIONS);

    return vertexSize;
}

Mesh* ChunkFactory::buildTiledMesh(TileArray* _tileArr, Tileset* _tileset, bool _resize) {
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

void ChunkFactory::buildVertexCache(TileArray* tiles, TilesetArray* tilesets, bool _resize) {
    if (_resize) return;

    tiles->getArrayPointer(true);
    tiles->vertexCache.clear();
    tiles->cachedTilesetArr = tilesets;

    for (auto& _ : tilesets->_tileSets) {
        std::vector<F32> vertices;
        IndexArray indices;
        ChunkFactory::buildVertexIndex(tiles, _, vertices, indices, false);
        tiles->vertexCache.emplace(_->_firstGid, vertices);
    }
}

static bool ChunkFactory::setTile(TileArray* tiles, TileID index, TileID newGid, bool _resize) {
    auto _tiles = tiles->getArrayPointer(true);
    auto prevT = _tiles[index];
    _tiles[index] = newGid;
    if (_resize) return prevT == 0;

    for (auto& _ : tiles->cachedTilesetArr->_tileSets) {
        auto& vertices = tiles->vertexCache[_->_firstGid];
        auto coord = calculateTileCoords(newGid, _);
        I32 startIndex = index * VERTEX_SIZE_NO_ANIMATIONS * 4;
        if (coord._outOfRange) coord = { 0,0,0,0 };
        float zPos = Random::rangeFloat(-10, 10);
        vertices[(2 + startIndex) + VERTEX_SIZE_NO_ANIMATIONS * 0] = zPos;
        vertices[(2 + startIndex) + VERTEX_SIZE_NO_ANIMATIONS * 1] = zPos;
        vertices[(2 + startIndex) + VERTEX_SIZE_NO_ANIMATIONS * 2] = zPos;
        vertices[(2 + startIndex) + VERTEX_SIZE_NO_ANIMATIONS * 3] = zPos;

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

ChunkRenderer* ChunkRenderer::create() {
    auto ref = new ChunkRenderer();
    if (ref->init()) {
        ref->autorelease();
        ref->retain();
        return ref;
    }
    AX_SAFE_DELETE(ref);
    return nullptr;
}

void ChunkRenderer::updateVertexData() {
    AX_ASSERT(_tiles->vertexCache[_tileset->_firstGid].size() == CHUNK_BUFFER_SIZE * 36, "Vertex cache was not built.");
    auto& vertices = _tiles->vertexCache[_tileset->_firstGid];
    _mesh->getVertexBuffer()->updateSubData((void*)&vertices[0], 0, vertices.size() * sizeof(vertices[0]));
}

ChunkRenderer::~ChunkRenderer() {
    LOG_RELEASE;
}

void ChunkRenderer::unload() {
    removeAllMeshes();
    _mesh = nullptr;
    _chunkDirty = true;
}

void ChunkRenderer::visit(Renderer* renderer, const Mat4& parentTransform, U32 parentFlags) {}

void TileSystem::ChunkRenderer::visit(Renderer* renderer, const Mat4& parentTransform, U32 parentFlags, ax::MeshMaterial* mat)
{
    if (_chunkDirty)
    {
        if (_resize) {
            removeAllMeshes();
            _mesh = ChunkFactory::buildTiledMesh(_tiles, _tileset, _resize);
        }
        if (_mesh && _resize || !_mesh && !_resize) {
            if (!_resize)
                _mesh = ChunkFactory::buildTiledMesh(_tiles, _tileset, _resize);
            _mesh->setMaterial(mat);
            addMesh(_mesh);
        }
        if (_mesh && !_resize) {
            updateVertexData();
        }
        _chunkDirty = false;
    }
    if (_mesh && getDisplayedOpacity()) {
        mat->setTexture(_tileset->_texture, ax::NTextureData::Usage::None);
        MeshRenderer::visit(renderer, parentTransform, parentFlags);
    }
}

void ChunkRenderer::cacheVertices(bool _resize) {
    this->_resize = _resize;
}

Chunk* Chunk::create(ChunkDescriptor desc) {
    auto ref = new Chunk();
    if (ref->init()) {
        ref->autorelease();
        ref->retain();

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

Chunk::~Chunk() {
    _tiles->retainedChunks--;
    RB_PROMISE_RELEASE(_tiles);
    _tilesetArr->retainedChunks--;
    RB_PROMISE_RELEASE(_tilesetArr);
    for (auto& _ : _chunks)
        RB_PROMISE_RELEASE(_);
    LOG_RELEASE;
}

I32 Chunk::resizeChunkCount() {
    if (count != _tilesetArr->_tileSets.size())
    {
        for (auto& c : _chunks)
            c->removeFromParent();
        _chunks.clear();
        for (auto& _ : _tilesetArr->_tileSets)
        {
            auto c = ChunkRenderer::create();
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

void Chunk::setPositionInChunkSpace(V2D pos) {
    setPositionInChunkSpace(pos.x, pos.y);
}

void Chunk::setPositionInChunkSpace(F32 x, F32 y) {
    _pos = { x,y };
}

void Chunk::visit(Renderer* renderer, const Mat4& parentTransform, U32 parentFlags, U32* renderCount, ax::MeshMaterial* mat, const ax::BlendFunc& blendFunc) {
    auto tiles = _tiles->getArrayPointer();
    if (!resizeChunkCount())
        return;

    auto cam = tileMapVirtualCamera;
    if (!cam) return;

    auto camScaling = cam->getScale();
    auto cam_aabb = Rect(cam->getPosition().x - 640 * camScaling, cam->getPosition().y - 360 * camScaling,
        cam->getPosition().x + 640 * camScaling, cam->getPosition().y + 360 * camScaling);

    auto pos = V2D(_pos.x * _tilesetArr->_tileSets[0]->_sizeInPixels.x, _pos.y * _tilesetArr->_tileSets[0]->_sizeInPixels.y);
    auto aabb = Rect(pos.x, pos.y, pos.x + _tilesetArr->_tileSets[0]->_sizeInPixels.x, pos.y + _tilesetArr->_tileSets[0]->_sizeInPixels.y);

    if (!Math::rectIntersectsRectOffOrigin(cam_aabb, aabb)) {
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

        _->setBlendFunc(blendFunc);
        _->visit(renderer, transform, parentFlags, mat);
        //(*renderCount)++;
        _chunkDirty = false;
    }
    _tilesetArr->retainedChunksI--;
    _tiles->retainedChunksI--;
}

void Chunk::cacheVertices(bool _resize) {
    this->_resize = _resize;
    for (auto& _ : _chunks)
        _->cacheVertices(_resize);
}

TileSystem::Layer* TileSystem::Layer::create(std::string_view name) {
    auto ref = new Layer();
    if (ref) {
        ref->_layerName = name;
        ref->_layerColor = Color4F::WHITE;
        ref->setBlendFunc(BlendFunc::ALPHA_NON_PREMULTIPLIED);

        ref->autorelease();

        ref->_material = ax::MeshMaterial::createBuiltInMaterial(ax::MeshMaterial::MaterialType::QUAD_TEXTURE, false);
        ref->_material->setTransparent(true);
        ref->_material->setForce2DQueue(true);
        ref->_material->getStateBlock().setCullFace(true);
        ref->_material->getStateBlock().setCullFaceSide(ax::CullFaceSide::BACK);
        ref->_material->retain();

        ref->setCascadeOpacityEnabled(true);

        return ref;
    }
    AX_SAFE_DELETE(ref);
    return nullptr;
}

TileSystem::Layer::~Layer() {
    for (auto& _ : _chunks)
        RB_PROMISE_RELEASE(_.second);
    AX_SAFE_RELEASE(_material);
    LOG_RELEASE;
}

Chunk* TileSystem::Layer::getChunkAtPos(V2D pos, TileID hintGid) {
    auto iter = _chunks.find(pos);
    if (iter == _chunks.end()) {
        if (hintGid == 0) return nullptr;
        TileID* tiles = (TileID*)malloc(CHUNK_BUFFER_SIZE * sizeof(TileID));
        memset(tiles, 0, CHUNK_BUFFER_SIZE * sizeof(TileID));
        auto tilesArr = TileArray::create(tiles);
        ChunkFactory::buildVertexCache(tilesArr, _tilesetArr, _resize);
        ChunkDescriptor d{};
        d._tiles = tilesArr;
        d._tilesetArr = _tilesetArr;
        auto c = Chunk::create(d);
        c->setPositionInChunkSpace(pos);
        c->_isModified = true;
        c->cacheVertices(_resize);
        _chunks.emplace(pos, c);
        addChild(c);
        return c;
    }
    else {
        iter->second->_isModified = true;
        return iter->second;
    }
}

void TileSystem::Layer::visit(Renderer* renderer, const Mat4& parentTransform, uint32_t parentFlags)
{
    if (!isVisible()) return;

    currentMaxDrawCallCount = 0;
    for (auto& [unused, _] : _chunks) {
        if (_->_isModified) {
            auto tiles = _->_tiles->getArrayPointer();
            int sum = 0;
            for (int i = 0; i < CHUNK_BUFFER_SIZE; i++)
                sum |= tiles[i];
            if (sum == 0) {
                _chunksToRemove.push_back(_->_pos);
                _->removeFromParent();
                continue;
            }
            _->_isModified = false;
        }

        _->visit(renderer, parentTransform, parentFlags, &currentMaxDrawCallCount, _material, blend);

        if (currentMaxDrawCallCount > maxDrawCallCount)
            break;
    }
    for (auto& _ : _chunksToRemove) {
        _chunks.erase(_);
    }
    _chunksToRemove.clear();
}

void TileSystem::Layer::cacheVertices(bool _resize)
{
    this->_resize = _resize;
    for (auto& _ : _chunks)
        _.second->cacheVertices(_resize);
}

void TileSystem::Layer::setBlendFunc(const BlendFunc& blendFunc)
{
    blend = blendFunc;
}

const BlendFunc& TileSystem::Layer::getBlendFunc() const
{
    return blend;
}

TileSystem::Map* TileSystem::Map::create(V2D _tileSize, I32 _contentScale, V2D _mapSize)
{
    auto ref = new TileSystem::Map();
    if (ref) {
        ref->_tileSize = _tileSize;
        ref->_contentScale = _contentScale;
        ref->_mapSize = _mapSize;
        ref->_chunkSize = CHUNK_SIZE;
        ref->_chunkSizeInPixels = ref->_tileSize * ref->_chunkSize;
        ref->_gridSize = CHUNK_SIZE * 6;

        ref->_editorLayer = Layer::create("EDITOR_LAYER_HIDDEN");
        ref->addChild(ref->_editorLayer);
        ref->_editorLayer->setOpacity(100);

        ref->autorelease();

        ref->setCascadeOpacityEnabled(true);

        return ref;
    }
    AX_SAFE_DELETE(ref);
    return nullptr;
}

TileSystem::Map::~Map()
{
    tileMapVirtualCamera = nullptr;
    RB_PROMISE_RELEASE(_tilesetArr);
    LOG_RELEASE;
}

void TileSystem::Map::addLayer(Layer* _layer)
{
    _layers.push_back(_layer);
    _layer->cacheVertices(_resize);
    addChild(_layer);
    _layer->_tilesetArr = _tilesetArr;
}

void TileSystem::Map::addLayer(std::string name)
{
    auto l = TileSystem::Layer::create(name);
    addLayer(l);
}

void TileSystem::Map::bindLayer(U32 idx)
{
    if (idx >= _layers.size()) return;

    _layerIdx = idx;
    _layerBind = _layers[idx];
}

void TileSystem::Map::setTilesetArray(TilesetArray* _tilesetArr)
{
    this->_tilesetArr = _tilesetArr;
    for (auto& _ : _layers)
        _->_tilesetArr = _tilesetArr;
    _editorLayer->_tilesetArr = _tilesetArr;
    _tileSize = _tilesetArr->_tileSize;
    _tilesetArr->calculateBounds();
    _tilesetArr->retain();
}

void TileSystem::Map::reload()
{
    if (_tilesetArr) {
        _tilesetArr->reloadTextures();
        _tilesetArr->calculateBounds();
    }
}

TileTransform TileSystem::Map::getTileTransform(const V2D& pos)
{
    TileTransform t{};
    t.chunk.x = floor(pos.x / CHUNK_SIZE);
    t.chunk.y = floor(pos.y / CHUNK_SIZE);
    float _x = fmod(pos.x, CHUNK_SIZE);
    float _y = fmod(pos.y, CHUNK_SIZE);
    if (_x >= 0) {
        _x++;
        t.id = int((CHUNK_SIZE - _y - 1) * CHUNK_SIZE + (_x - 1)) % int(CHUNK_BUFFER_SIZE);
        return t;
    }
    t.id = int((CHUNK_SIZE - _y) * CHUNK_SIZE + _x) % int(CHUNK_BUFFER_SIZE);
    return t;
}

void TileSystem::Map::setTileAt(Layer* _layer, const V2D& pos, TileID gid)
{
    if (!_layer || pos.x >= _mapSize.x || pos.x < -_mapSize.x || pos.y >= _mapSize.y || pos.y < -_mapSize.y)
        return;
    TileArray* tilesArr;
    TileTransform i = getTileTransform(pos);
    cachedChunk = _layer->getChunkAtPos(i.chunk, gid);
    cachedChunkPosition = i.chunk;
    if (!cachedChunk) return;
    tilesArr = cachedChunk->_tiles;
    auto cond = ChunkFactory::setTile(tilesArr, i.id, gid, _resize);
    _tileCount += (gid == 0 ? (!cond ? -1 : 0) : (cond ? 1 : 0));
}

void TileSystem::Map::setTileAt(const V2D& pos, TileID gid) {
    setTileAt(_layerBind, pos, gid);
}

TileID TileSystem::Map::getTileAt(const V2D& pos) {
    if (!_layerBind || pos.x >= _mapSize.x || pos.x < -_mapSize.x || pos.y >= _mapSize.y || pos.y < -_mapSize.y)
        return 0;
    TileTransform i = getTileTransform(pos);
    auto chunk = _layerBind->getChunkAtPos(i.chunk, 0);
    if (!chunk) return 0;
    return chunk->_tiles->getArrayPointer()[i.id];
}

TileID TileSystem::Map::getTileGIDAt(const V2D& pos) {
    auto tile = getTileAt(pos);
    tile &= TILE_FLAG_NONE;
    return tile;
}

void TileSystem::Map::visit(Renderer* renderer, const Mat4& parentTransform, uint32_t parentFlags) {
    chunkMeshCreateCount = 0;
    cachedChunk = nullptr;
    cachedChunkPosition = { FLT_MAX, FLT_MAX };
    for (auto& _ : _layers)
        _->visit(renderer, parentTransform, parentFlags);
    _editorLayer->visit(renderer, parentTransform, parentFlags);
}

void TileSystem::Map::cacheVertices(bool _resize) {
    this->_resize = _resize;
    for (auto& _ : _layers)
        _->cacheVertices(_resize);
}
