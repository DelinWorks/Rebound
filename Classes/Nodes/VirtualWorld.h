#ifndef __H_SCENEVIRTUALCAMERA__
#define __H_SCENEVIRTUALCAMERA__

#include <axmol.h>
#include "VirtualCamera.h"

class VirtualWorldSpace : public ax::Node {
	~VirtualWorldSpace();

public:
	ax::RenderTexture* rtPass = nullptr;
	Color4F rtClear = Color4F(0, 0, 0, 0);
	Mat4 transform;
	bool skipSameTransform = false;

	void render(const Mat4& transformMatrix = Mat4::IDENTITY);
	void renderPass(const Mat4& transformMatrix = Mat4::IDENTITY);
	void renderCompositePass(ax::RenderTexture* composite, const Mat4& transformMatrix = Mat4::IDENTITY);
};

class VirtualWorldManager {
public:
	VirtualWorldManager();
	~VirtualWorldManager();
	
	void resizeRenderTextures(ax::Scene* scene);
	void renderAllPasses(ax::Scene* scene, ax::Color4F bg = ax::Color4F(0, 0, 0, 0));

	ax::Vec2 _currentSize;
	ax::GLView* _glview;
	std::vector<ax::RenderTexture*> _rts;
	VirtualCamera* _camera;
	std::vector<VirtualWorldSpace*> _worlds;
};

#endif 
