#include "VirtualWorld.h"

VirtualWorld::VirtualWorld()
{
	_glview = ax::Director::getInstance()->getOpenGLView();
	_camera = new VirtualCamera();
	_camera->retain();
	_world = new VirtualWorldSpace();
	_world->retain();
}

VirtualWorld::~VirtualWorld()
{
	AX_SAFE_RELEASE(_camera);
	AX_SAFE_RELEASE(_world);
}

void VirtualWorld::refresh(ax::Scene* scene)
{
	if (_currentSize != _glview->getFrameSize()) {
		_currentSize = _glview->getFrameSize();
		auto visibleSize = _glview->getVisibleSize();

		if (!_rt) {
			scene->addChild(_rt = ax::RenderTexture::create(_currentSize.x, _currentSize.y), -1);
			//_rt->addComponent((new UiRescaleComponent(visibleSize))->setVisibleSizeHints()->enableDesignScaleIgnoring());
		}

		_rt->initWithWidthAndHeight(_currentSize.x, _currentSize.y,
			ax::backend::PixelFormat::RGBA8);
		auto p = GameUtils::createGPUProgram("default.frag", "default.vert");
		_rt->getSprite()->setProgramState(p);

		SET_POSITION_HALF_SCREEN(_rt);

		_camera->getChildren().at(0)->setPosition(_glview->getVisibleSize() / -2);

		GameUtils::setNodeIgnoreDesignScale(_rt);
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
