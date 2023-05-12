#ifndef __H_VIRTUALCAMERA__
#define __H_VIRTUALCAMERA__

#include <axmol.h>
#include "shared_scenes/GameUtils.h"
#include "Components/UiRescaleComponent.h"

class VirtualCamera : public ax::Node {
public:
	VirtualCamera();

	void setZoom(float zoom);

	ax::Mat4 getWorldSpaceMatrix();
	ax::Mat4 getScreenSpaceMatrix();
};

#endif 
