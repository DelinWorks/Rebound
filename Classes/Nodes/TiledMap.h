#include "axmol.h"
#include "Entities/CatPlayer.h"
#include "Helper/Math.h"
#include "Helper/short_types.h"
#include "Helper/Color.hpp"
#include "string_manipulation_lib/stringFunctions.hpp"

#ifndef __H_TILEDMAP__
#define __H_TILEDMAP__

class TiledMap : public ax::Node {
public:
	TiledMap() {}
	bool createWithFilename(ax::Scene* scene, std::string_view file, CatPlayer* player);
};

#endif
