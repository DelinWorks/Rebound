#include "VirtualWorld.h"

VirtualWorld::VirtualWorld()
{
	_glview = ax::Director::getInstance()->getOpenGLView();
	_camera = new VirtualCamera();

	_worlds.push_back(new VirtualWorldSpace());
	_worlds.push_back(new VirtualWorldSpace());

	_rts.emplace_back(nullptr);
	_rts.emplace_back(nullptr);
}

VirtualWorld::~VirtualWorld()
{
	AX_SAFE_RELEASE(_camera);
	for (auto& _ : _worlds)
		AX_SAFE_RELEASE(_);
	for (auto& _ : _rts)
		AX_SAFE_RELEASE(_);
}

void VirtualWorld::refresh(ax::Scene* scene)
{
	if (_currentSize != _glview->getFrameSize()) {
		_currentSize = _glview->getFrameSize();
		auto visibleSize = _glview->getVisibleSize();

		int idx = -100;
		for (int i = 0; i < _rts.size(); i++) {
			auto& _ = _rts[i];

			if (!_) {
				_ = ax::RenderTexture::create();
				scene->addChild(_, idx++);
				_worlds[i]->rtPass = _;
			}

			_->initWithWidthAndHeight(_currentSize.x, _currentSize.y,
				ax::backend::PixelFormat::NONE, false);
			auto p = GameUtils::createGPUProgram("default.frag", "default.vert");
			_->getSprite()->setProgramState(p);

			SET_POSITION_HALF_SCREEN(_);
			SET_POSITION_MINUS_HALF_SCREEN(_camera->getChildren().at(0));

			GameUtils::setNodeIgnoreDesignScale(_, true, 1);
		}

		auto p = GameUtils::createGPUProgram("outline.frag", "default.vert");
		_rts[1]->getSprite()->setProgramState(p);
	}
}

void VirtualWorld::render(ax::Scene* scene, ax::Color4F bg)
{
	refresh(scene);

	// Transform the nodes by the inverse of the camera world matrix.
	auto invW = _camera->getWorldSpaceMatrix().getInversed();

	// First pass should be cleared with a color that is picked by the user, 
	// rest of the passes need to have an alpha channel for the previous
	// passes to be seen.
	_worlds[0]->rtClear = bg;

	// Render the Editor Pass, used for tools and draw nodes in the editor.
	_worlds[0]->renderPass(invW);

	// Render the Tilemap/Objects Pass, used for any decorations.
	_worlds[1]->renderPass(invW);

	// The order that these worlds are rendered in matters.
	// These render passes can have a unique shader attached 
	// to them for post processing & visual effects.
}

void VirtualWorldSpace::render(const Mat4& transformMatrix)
{
	visit(ax::Director::getInstance()->getRenderer(),
		transformMatrix, FLAGS_TRANSFORM_DIRTY);
}

void VirtualWorldSpace::renderPass(const Mat4& transformMatrix)
{
	rtPass->beginWithClear(rtClear.r, rtClear.g, rtClear.b, rtClear.a);
	render(transformMatrix);
	rtPass->end();
}

void VirtualWorldSpace::renderCompositePass(ax::RenderTexture* composite, const Mat4& transformMatrix)
{
	// TODO: implement composite render passes.
	// 
	// NOTE: Shaders used with compositing should
	// take in a sampler2D from the composite pass, and
	// blend it with the new pass, minding the alpha channel.
}

VirtualWorldSpace::~VirtualWorldSpace()
{
	LOG_RELEASE;
}
