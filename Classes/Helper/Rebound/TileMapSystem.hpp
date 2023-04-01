#include <axmol.h>
#include "renderer/backend/Backend.h"
#include "TileMapLayer.hpp"

#ifndef __H_TILEMAPSYSTEM__
#define __H_TILEMAPSYSTEM__

using namespace ax;

namespace TileSystem {

    class Map : public Node {
    public:
        static Map* create(i32 _tileSize, i32 _contentScale, Vec2 _mapSize)
        {
            auto ref = new Map();
            if (ref) {
                ref->_tileSize = _tileSize;
                ref->_contentScale = _contentScale;
                ref->_mapSize = _mapSize;
                ref->_chunkSize = CHUNK_SIZE;
                ref->_chunkSizeInPixels = ref->_chunkSize * ref->_tileSize;
                ref->_gridSize = CHUNK_SIZE * 5;
                
                ref->autorelease();
                return ref;
            }
            AX_SAFE_DELETE(ref);
            return nullptr;
        }

        void addLayer(Layer* _layer) {
            _layers.push_back(_layer);
            addChild(_layer);
        }

        void bindLayer(i16 idx) {
            _layerBind = _layers[idx];
        }

        void setTilesetArray(TilesetArray* _tilesetArr) {
            this->_tilesetArr = _tilesetArr;
            for (auto& _ : _layers)
                _->_tilesetArr = _tilesetArr;
        }

        void reload() {
            if (_tilesetArr) {
                _tilesetArr->reloadTextures();
                _tilesetArr->calculateBounds();
            }
        }

        ax::Vec2 chunkTilePos(ax::Vec2 tilePos) {
            return Vec2((int(tilePos.x) - (tilePos.x < 0 ? (CHUNK_SIZE - 1) : 0)) / CHUNK_SIZE,
                (int(tilePos.y) - (tilePos.y < 0 ? (CHUNK_SIZE - 1) : 0)) / CHUNK_SIZE);
        }

        void setTileAt(ax::Vec2 pos, TileID gid) {
            if (_layerBind) {

            }
        }

        void draw(Renderer* renderer, const Mat4& parentTransform, uint32_t parentFlags) override {}
        void visit(Renderer* renderer, const Mat4& parentTransform, uint32_t parentFlags) override {
            for (auto& _ : _layers)
                _->visit(renderer, parentTransform, parentFlags);
        }

        TilesetArray* _tilesetArr;
        Layer* _layerBind;
        std::vector<Layer*> _layers;

        i32 _tileSize;
        i32 _contentScale = 1;
        Vec2 _mapSize;
        i32 _chunkSize;
        i32 _chunkSizeInPixels;
        i32 _gridSize;
    };
}

#endif
