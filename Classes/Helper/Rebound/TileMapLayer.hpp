#include <axmol.h>
#include "TileMapChunk.hpp"

#ifndef __H_TILEMAPLAYER__
#define __H_TILEMAPLAYER__

namespace TileSystem {

	class Layer : public Node {
	public:
		void draw(Renderer* renderer, const Mat4& parentTransform, uint32_t parentFlags) override {}
		void visit(Renderer* renderer, const Mat4& parentTransform, uint32_t parentFlags) override {
			for (auto& [__, _] : _chunks)
				_->visit(renderer, parentTransform, parentFlags);
		}

		TilesetArray* _tilesetArr;
		Color4F _layerColor;
		std::string _layerName;

		std::unordered_map<Vec2, ChunkRenderer*> _chunks;
	};
}

#endif
