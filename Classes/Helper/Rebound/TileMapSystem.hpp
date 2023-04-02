#include <axmol.h>
#include "renderer/backend/Backend.h"
#include "TileMapLayer.hpp"

#ifndef __H_TILEMAPSYSTEM__
#define __H_TILEMAPSYSTEM__

using namespace ax;

namespace TileSystem {

    class Map : public Node {
    public:
        static Map* create(Vec2 _tileSize, i32 _contentScale, Vec2 _mapSize)
        {
            auto ref = new Map();
            if (ref) {
                ref->_tileSize = _tileSize;
                ref->_contentScale = _contentScale;
                ref->_mapSize = _mapSize;
                ref->_chunkSize = CHUNK_SIZE;
                ref->_chunkSizeInPixels = ref->_tileSize * ref->_chunkSize;
                ref->_gridSize = CHUNK_SIZE * 5;
                
                ref->autorelease();
                return ref;
            }
            AX_SAFE_DELETE(ref);
            return nullptr;
        }

        ~Map() {
            AX_SAFE_RELEASE(_tilesetArr);
        }

        void addLayer(Layer* _layer) {
            _layers.push_back(_layer);
            addChild(_layer);
            _layer->_tilesetArr = _tilesetArr;
        }

        void addLayer(std::string name) {
            auto l = TileSystem::Layer::create(name);
            addLayer(l);
        }

        void bindLayer(i16 idx) {
            _layerBind = _layers[idx];
        }

        void setTilesetArray(TilesetArray* _tilesetArr) {
            this->_tilesetArr = _tilesetArr;
            for (auto& _ : _layers)
                _->_tilesetArr = _tilesetArr;
            _tileSize = _tilesetArr->_tileSize;
            _tilesetArr->calculateBounds();
            _tilesetArr->retain();
        }

        void reload() {
            if (_tilesetArr) {
                _tilesetArr->reloadTextures();
                _tilesetArr->calculateBounds();
            }
        }

        /* Given a tile position and a chunk reference, it will calculate the chunk
           position that the tile lies on and set that value to the chunk reference,
           it will then calculate which index in the array the tile's at and return it.
        */
        int getTileTransform(const ax::Vec2& pos, ax::Vec2& chunk) {
            chunk.x = floor(pos.x / CHUNK_SIZE);
            chunk.y = floor(pos.y / CHUNK_SIZE);
            float _x = fmod(pos.x, CHUNK_SIZE);
            float _y = fmod(pos.y, CHUNK_SIZE);
            if (_x >= 0) {
                _x++;
                return int((CHUNK_SIZE - _y - 1) * CHUNK_SIZE + (_x - 1)) % int(CHUNK_BUFFER_SIZE);
            }
            return int((CHUNK_SIZE - _y) * CHUNK_SIZE + _x) % int(CHUNK_BUFFER_SIZE);
        }

        void setTileAt(const ax::Vec2& pos, TileID gid) {
            if (!_layerBind || pos.x >= _mapSize.x || pos.x < -_mapSize.x || pos.y >= _mapSize.y || pos.y < -_mapSize.y)
                return;
              Vec2 chunkPos;
            auto index = getTileTransform(pos, chunkPos);
            auto chunk = _layerBind->getChunkAtPos(chunkPos, gid);
            if (!chunk) return;
            auto tilesArr = chunk->_tiles;
            ChunkFactory::setTile(tilesArr, index, gid);
        }

        TileID getTileAt(const ax::Vec2& pos) {
            if (!_layerBind || pos.x >= _mapSize.x || pos.x < -_mapSize.x || pos.y >= _mapSize.y || pos.y < -_mapSize.y)
                return 0;
            Vec2 chunkPos;
            auto index = getTileTransform(pos, chunkPos);
            auto chunk = _layerBind->getChunkAtPos(chunkPos, 0);
            if (!chunk) return 0;
            return chunk->_tiles->getArrayPointer()[index];
        }

        TileID getTileGIDAt(const ax::Vec2& pos) {
            auto tile = getTileAt(pos);
            tile &= TILE_FLAG_NONE;
            return tile;
        }

        void draw(Renderer* renderer, const Mat4& parentTransform, uint32_t parentFlags) override {}
        void visit(Renderer* renderer, const Mat4& parentTransform, uint32_t parentFlags) override {
            chunkMeshCreateCount = 0;
            for (auto& _ : _layers)
                _->visit(renderer, parentTransform, parentFlags);
        }

        TilesetArray* _tilesetArr;
        Layer* _layerBind;
        std::vector<Layer*> _layers;

        Vec2 _tileSize;
        i32 _contentScale = 1;
        Vec2 _mapSize;
        i32 _chunkSize;
        Vec2 _chunkSizeInPixels;
        i32 _gridSize;
    };
}

#endif
