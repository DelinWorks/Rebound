#include "VirtualWorld.h"

VirtualWorld::VirtualWorld()
{
	_glview = ax::Director::getInstance()->getOpenGLView();
	_camera = new VirtualCamera();
	_world = new VirtualWorldSpace();

	_rts.push_back(nullptr);
	_rts.push_back(nullptr);
}

VirtualWorld::~VirtualWorld()
{
	AX_SAFE_RELEASE(_camera);
	AX_SAFE_RELEASE(_world);
	for (auto& _ : _rts)
		AX_SAFE_RELEASE(_);
}

void VirtualWorld::refresh(ax::Scene* scene)
{
	if (_currentSize != _glview->getFrameSize()) {
		_currentSize = _glview->getFrameSize();
		auto visibleSize = _glview->getVisibleSize();

		int idx = -100;
		for (auto& _ : _rts) {
			if (!_) {
				_ = ax::RenderTexture::create();
				//_->retain();
				scene->addChild(_, idx++);
			}

			_->initWithWidthAndHeight(_currentSize.x, _currentSize.y,
				ax::backend::PixelFormat::RGBA8);
			auto p = GameUtils::createGPUProgram("default.frag", "default.vert");
			_->getSprite()->setProgramState(p);

			auto blend = ax::BlendFunc{};
			blend.src = backend::BlendFactor::ONE;
			blend.dst = backend::BlendFactor::ONE_MINUS_SRC_ALPHA;
			_->getSprite()->setBlendFunc(blend);

			SET_POSITION_HALF_SCREEN(_);
			SET_POSITION_MINUS_HALF_SCREEN(_camera->getChildren().at(0));

			GameUtils::setNodeIgnoreDesignScale(_, true, 1);
		}
	}
}

void VirtualWorld::render(ax::Scene* scene, ax::Color4F bg)
{
	refresh(scene);
	auto invMatrix = _camera->getInverseMatrix(_currentSize);

	_rts[0]->beginWithClear(bg.r, bg.g, bg.b, 1);
	_world->visit(ax::Director::getInstance()->getRenderer(),
		invMatrix, ax::Node::FLAGS_TRANSFORM_DIRTY);
	_rts[0]->end();
	_rts[0]->setScale(1);

	_rts[1]->beginWithClear(1, 1, 1, 1);
	_rts[1]->end();
	_rts[1]->setScale(0.5);
}

VirtualWorldSpace::~VirtualWorldSpace()
{
	LOG_RELEASE;
}
