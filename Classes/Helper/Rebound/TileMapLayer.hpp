#include <axmol.h>
#include "TileMapChunk.hpp"

#ifndef __H_TILEMAPLAYER__
#define __H_TILEMAPLAYER__

namespace TileSystem {

	class Layer : public Node {
	public:
		static Layer* create(std::string_view name) {
			auto ref = new Layer();
			if (ref) {
				ref->_layerName = name;
				ref->_layerColor = Color4F::WHITE;

				ref->autorelease();
				return ref;
			}
			AX_SAFE_DELETE(ref);
			return nullptr;
		}

		ChunkRenderer* getChunkAtPos(Vec2 pos, TileID hintGid = -1) {
			pos.x = round(pos.x); pos.y = round(pos.y);
			auto iter = _chunks.find(pos);
			if (iter == _chunks.end()) {
				if (hintGid == 0) return nullptr;
				TileID* tiles = (TileID*)malloc(CHUNK_BUFFER_SIZE * sizeof(TileID));
				memset(tiles, 0, CHUNK_BUFFER_SIZE * sizeof(TileID));
				auto tilesArr = TileArray::create(tiles);
				ChunkFactory::buildVertexCache(tilesArr, _tilesetArr);
				ChunkDescriptor d{};
				d._tiles = tilesArr;
				d._tilesetArr = _tilesetArr;
				auto c = ChunkRenderer::create(d);
				c->setPositionInChunkSpace(pos);
				_chunks.emplace(pos, c);
				c->_isModified = true;
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

		std::string _layerName;
		Color4F _layerColor;

		TilesetArray* _tilesetArr;
		std::vector<Vec2> _chunksToRemove;
		std::map<Vec2, ChunkRenderer*> _chunks;
	};
}

#endif
