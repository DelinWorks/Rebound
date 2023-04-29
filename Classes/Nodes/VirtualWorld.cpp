#include "VirtualWorld.h"

VirtualWorld::VirtualWorld()
{
	_glview = ax::Director::getInstance()->getOpenGLView();
	_camera = new VirtualCamera();
	_world = new VirtualWorldSpace();
}

VirtualWorld::~VirtualWorld()
{
	AX_SAFE_RELEASE(_camera);
	AX_SAFE_RELEASE(_world);
	AX_SAFE_RELEASE(_rt);
}

void VirtualWorld::refresh(ax::Scene* scene)
{
	if (_currentSize != _glview->getFrameSize()) {
		_currentSize = _glview->getFrameSize();
		auto visibleSize = _glview->getVisibleSize();

		if (!_rt) {
			scene->addChild(_rt = ax::RenderTexture::create(), -1);
			//_rt->addComponent((new UiRescaleComponent(visibleSize))->setVisibleSizeHints()->enableDesignScaleIgnoring());
			//_rt->setPositionZ(-100000);
		}

		_rt->initWithWidthAndHeight(_currentSize.x, _currentSize.y,
			ax::backend::PixelFormat::RGBA8);
		auto p = GameUtils::createGPUProgram("chrom.frag", "default.vert");
		_rt->getSprite()->setProgramState(p);

		SET_POSITION_HALF_SCREEN(_rt);
		SET_POSITION_MINUS_HALF_SCREEN(_camera->getChildren().at(0));

		GameUtils::setNodeIgnoreDesignScale(_rt, true, 1);
	}
}

void VirtualWorld::render(ax::Scene* scene, ax::Color4F bg)
{
	refresh(scene);
	auto invMatrix = _camera->getInverseMatrix(_currentSize);
	_rt->beginWithClear(bg.r, bg.g, bg.b, bg.a);
	_world->visit(ax::Director::getInstance()->getRenderer(),
		invMatrix, ax::Node::FLAGS_TRANSFORM_DIRTY);
	_rt->end();
}

VirtualWorldSpace::~VirtualWorldSpace()
{
	LOG_RELEASE;
}
