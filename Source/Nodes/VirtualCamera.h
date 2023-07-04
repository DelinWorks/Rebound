#pragma once

#include <axmol.h>
#include "shared_scenes/GameUtils.h"


class VirtualCamera : public ax::Node {
public:
	VirtualCamera();

	void setZoom(float zoom);

	ax::Mat4 getUnscaledWorldSpaceMatrix();
	ax::Mat4 getWorldSpaceMatrix();
	ax::Mat4 getScreenSpaceMatrix();
};
