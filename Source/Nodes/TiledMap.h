#include "axmol.h"
#include "Entities/CatPlayer.h"
#include "Helper/Math.h"
#include "Helper/short_types.h"
#include "Helper/Color.hpp"
#include "Helper/Random.h"
#include "string_manipulation_lib/stringFunctions.hpp"
#include "shared_scenes/GameSingleton.h"
#include "shared_scenes/GameUtils.h"
#include "Helper/EditorFunctionInterpreter.hpp"

#ifndef __H_TILEDMAP__
#define __H_TILEDMAP__

class TiledMap : public ax::Node {
public:
	TiledMap() : tiledMap(nullptr) {}
	~TiledMap();
	CatPlayer* player;
	ax::Camera* cam;
	ax::TMXTiledMap* tiledMap;
	void update(F32 dt) override;
	//bool initWithFilename(ax::Scene* scene, ReboundPhysicsWorld* world, std::string_view file, CatPlayer* _player);
};

#endif
