#include "VirtualCamera.h"

VirtualCamera::VirtualCamera()
{
    auto shiftedTranform = Node::create();
    addChild(shiftedTranform);
}

void VirtualCamera::setZoom(float zoom)
{
	setScale(zoom);
}

ax::Mat4 VirtualCamera::getInverseMatrix(ax::Vec2 _frameSize)
{
	auto matrix = getChildren().at(0)->getNodeToWorldTransform();
	auto scale2 = getScale() * 2;
	//matrix.translate(_frameSize.x / scale2, _frameSize.y / scale2, 0);
	auto director = Director::getInstance();
	auto visibleSize = director->getVisibleSize();
	auto screenSize = director->getOpenGLView()->getFrameSize();
	auto ratio = Vec2(visibleSize.x / screenSize.x, visibleSize.y / screenSize.y);
	matrix.scale(ax::Vec3(ratio.x, ratio.y, 1));
	return matrix.getInversed();
}
