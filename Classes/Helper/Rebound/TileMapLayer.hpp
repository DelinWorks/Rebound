#include <axmol.h>
#include "TileMapChunk.hpp"

#ifndef __H_TILEMAPLAYER__
#define __H_TILEMAPLAYER__

namespace TileSystem {

	class Layer : public ChunkRenderMethod, public ax::Node {
	public:
		static Layer* create(std::string_view name) {
			auto ref = new Layer();
			if (ref) {
				ref->_layerName = name;
				ref->_layerColor = Color4F::WHITE;

				ref->autorelease();

				ref->setCascadeOpacityEnabled(true);

				return ref;
			}
			AX_SAFE_DELETE(ref);
			return nullptr;
		}

		~Layer() {
			LOG_RELEASE;
		}

		ChunkRenderer* getChunkAtPos(Vec2 pos, TileID hintGid = -1) {
			pos.x = round(pos.x); pos.y = round(pos.y);
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
				auto c = ChunkRenderer::create(d);
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

		void draw(Renderer* renderer, const Mat4& parentTransform, uint32_t parentFlags) override {}
		void visit(Renderer* renderer, const Mat4& parentTransform, uint32_t parentFlags) override {
			for (auto& [__, _] : _chunks) {
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

				_->visit(renderer, parentTransform, parentFlags);
			}
			for (auto& _ : _chunksToRemove) {
				_chunks.erase(_);
			}
			_chunksToRemove.clear();
		}

		void cacheVertices(bool _resize) {
			this->_resize = _resize;
			for (auto& _ : _chunks)
				_.second->cacheVertices(_resize);
		}

		std::string _layerName = "";
		Color4F _layerColor = Color4F::WHITE;

		TilesetArray* _tilesetArr = nullptr;
		std::vector<Vec2> _chunksToRemove;
		std::map<Vec2, ChunkRenderer*> _chunks;
	};
}

#endif
