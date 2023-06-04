#include "custom_ui.h"

void CUI::GUI::setContentSize(const Vec2& size, bool recursive)
{
	if (!size.equals(getContentSize())) {
		_prefferedSize = size;
		Node::setContentSize(size);
		if (recursive)
			notifyLayout();

#ifdef DRAW_NODE_DEBUG
		_contentSizeDebug->clear();
		_contentSizeDebug->drawRect(-getContentSize() / 2, getContentSize() / 2, Color4B(Color3B::ORANGE, 50));
#endif
	}
}

Vec2 CUI::GUI::getScaledContentSize()
{
	return getContentSize();
}

void CUI::GUI::onFontScaleUpdate(float scale)
{
	// any GUI node that doesn't override this function
	// is going to recursive update it's children.
	for (auto& _ : getChildren())
	{
		auto cast = DCAST(GUI, _);
		if (cast)
			cast->onFontScaleUpdate(scale);
	}
}

void CUI::GUI::updateEnabled(bool state)
{
	auto newState = _isEnabled;

	if (!newState && _isInternalEnabled)
	{
		_isInternalEnabled = false;
		disable();
		onDisable();
	}
	else
	{
		if (state && !_isInternalEnabled && _isEnabled)
		{
			_isInternalEnabled = true;
			enable();
			onEnable();
		}
		else if (!state && _isInternalEnabled)
		{
			_isInternalEnabled = false;
			disable();
			onDisable();
		}
	}

	_isEnabled = newState;

	if (_isContainer)
		for (auto& i : getChildren())
		{
			auto cast = DCAST(GUI, i);
			if (cast) cast->updateEnabled(_isInternalEnabled);
		}
}

CUI::GUI::GUI()
{
	_contentSizeDebug = DrawNode::create(1);
#ifdef DRAW_NODE_DEBUG
	addChild(_contentSizeDebug, 99);
	_contentSizeDebug->setTag(YOURE_NOT_WELCOME_HERE);
#endif

	if (!_backgroundShader) {
		_backgroundShader = GameUtils::createGPUProgram("ui_opacity.frag", "default.vert");
		SET_UNIFORM(_backgroundShader, "ui_alpha", 0.8f);
	}
}

CUI::GUI::~GUI()
{
	LOG_RELEASE;
}

void CUI::GUI::setUiOpacity(float opacity)
{
	SET_UNIFORM(_backgroundShader, "ui_alpha", opacity);
	if (opacity < 0.5 && !_ForceOutline)
	{
		_ForceOutline = true;
		onFontScaleUpdate(_UiScale / _UiScaleMul);
	}
	if (opacity > 0.5 && _ForceOutline)
	{
		_ForceOutline = false;
		onFontScaleUpdate(_UiScale / _UiScaleMul);
	}
}

bool CUI::GUI::hover(Vec2 mouseLocationInView, Camera* cam)
{
	return false;
}

bool CUI::GUI::press(Vec2 mouseLocationInView, Camera* cam)
{
	return false;
}

bool CUI::GUI::release(Vec2 mouseLocationInView, Camera* cam)
{
	return false;
}

void CUI::GUI::keyPress(EventKeyboard::KeyCode keyCode)
{
}

void CUI::GUI::keyRelease(EventKeyboard::KeyCode keyCode)
{
}

void CUI::GUI::mouseScroll(EventMouse* event)
{
}

void CUI::GUI::pushModal(GUI* child)
{
	addChild(child);
}

void CUI::GUI::onEnter()
{
	Node::onEnter();
	notifyFocused(this, false);
}

void CUI::GUI::onExit()
{
	Node::onExit();
	notifyFocused(this, false);
}

void CUI::GUI::setAnchorPoint(const ax::Vec2& anchor)
{
	_anchorPoint = anchor;
	Node::setAnchorPoint(anchor);
}

void CUI::GUI::setAnchorOffset(const ax::Vec2& anchorOffset)
{
	_anchorOffset = anchorOffset;
	Node::setAnchorPoint(_anchorPoint * anchorOffset);
}

void CUI::GUI::notifyFocused(GUI* sender, bool focused, bool ignoreSelf)
{
	auto cast = DCAST(GUI, getParent());
	if (cast) {
		//if (_isFocused == focused) return;
		if (!ignoreSelf)
			_isFocused = focused;
		cast->notifyFocused(sender, focused);
	}
	else {
		if (focused)
			_focusedElements.insert(sender);
		else
			_focusedElements.erase(sender);

		_isFocused = _focusedElements.size() > 0;
	}
}

void CUI::GUI::notifyEnabled()
{
	auto cast = DCAST(GUI, getParent());
	if (cast)
		cast->notifyEnabled();
	else
		updateEnabled(_isEnabled);
}

void CUI::GUI::notifyLayout()
{
	auto gui = DCAST(GUI, getParent());
	if (gui) gui->notifyLayout();
}

bool CUI::GUI::isEnabled()
{
	return _isEnabled && _isInternalEnabled;
}

bool CUI::GUI::isInternalEnabled()
{
	return _isEnabled;
}

void CUI::GUI::enable(bool show)
{
	_isEnabled = true;
	if (show) setVisible(true);
	notifyEnabled();
}

void CUI::GUI::disable(bool hide)
{
	_isEnabled = false;
	if (hide) setVisible(false);
	notifyEnabled();
}

void CUI::GUI::onEnable()
{
}

void CUI::GUI::onDisable()
{
}

Size CUI::GUI::getFitContentSize()
{
	return getContentSize();
}

const Size& CUI::GUI::getPrefferedContentSize() const
{
	return _prefferedSize;
}

void CUI::SignalHandeler::signalSceneRoot(std::string signal)
{
	auto handeler = DCAST(SignalHandeler, Director::getInstance()->getRunningScene());
	if (handeler) handeler->signal(signal);
}
