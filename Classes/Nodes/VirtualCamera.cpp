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

ax::Mat4 VirtualCamera::getWorldSpaceMatrix()
{
	auto matrix = getChildren().at(0)->getNodeToWorldTransform();
	auto director = Director::getInstance();
	auto visibleSize = director->getVisibleSize();
	auto screenSize = director->getOpenGLView()->getFrameSize();
	auto ratio = Vec2(visibleSize.x / screenSize.x, visibleSize.y / screenSize.y);
	matrix.scale(ax::Vec3(ratio.x, ratio.y, 1));
	return matrix;
}

ax::Mat4 VirtualCamera::getScreenSpaceMatrix()
{
	auto matrix = Mat4::IDENTITY;
	auto director = Director::getInstance();
	auto visibleSize = director->getVisibleSize();
	auto screenSize = director->getOpenGLView()->getFrameSize();
	auto ratio = Vec2(visibleSize.x / screenSize.x, visibleSize.y / screenSize.y);
	auto n = Node::create();
	GameUtils::setNodeIgnoreDesignScale(n);
	matrix.scale(ax::Vec3(ratio.x, ratio.y, 1));
	matrix.translate(ax::Vec3(visibleSize.x / 2 / -n->getScaleX(), visibleSize.y / 2 / -n->getScaleY(), 0));
	return matrix;
}
