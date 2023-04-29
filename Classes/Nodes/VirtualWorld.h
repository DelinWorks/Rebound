#ifndef __H_SCENEVIRTUALCAMERA__
#define __H_SCENEVIRTUALCAMERA__

#include <axmol.h>
#include "VirtualCamera.h"

class VirtualWorldSpace : public ax::Node {
	~VirtualWorldSpace();
};

class VirtualWorld {
public:
	VirtualWorld();
	~VirtualWorld();
	
	void refresh(ax::Scene* scene);
	void render(ax::Scene* scene, ax::Color4F bg = ax::Color4F(0, 0, 0, 0));

	ax::Vec2 _currentSize;
	ax::GLView* _glview;
	ax::RenderTexture* _rt;
	VirtualCamera* _camera;
	VirtualWorldSpace* _world;
};

#endif 
