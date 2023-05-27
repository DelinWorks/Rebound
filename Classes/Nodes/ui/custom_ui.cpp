#include "custom_ui.h"

void CustomUi::GUI::setContentSize(const Vec2& size, bool recursive)
{
	if (!size.equals(getContentSize())) {
		Node::setContentSize(size);
		if (recursive)
			notifyLayout();

#ifdef DRAW_NODE_DEBUG
		_contentSizeDebug->clear();
		_contentSizeDebug->drawRect(-getContentSize() / 2, getContentSize() / 2, Color4B(Color3B::ORANGE, 50));
#endif
	}
}

Vec2 CustomUi::GUI::getScaledContentSize()
{
	return getContentSize();
}

void CustomUi::GUI::onFontScaleUpdate(float scale)
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

void CustomUi::GUI::updateEnabled(bool state)
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

CustomUi::GUI::GUI()
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

CustomUi::GUI::~GUI()
{
	LOG_RELEASE;
}

void CustomUi::GUI::setUiOpacity(float opacity)
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

bool CustomUi::GUI::hover(Vec2 mouseLocationInView, Camera* cam)
{
	return false;
}

bool CustomUi::GUI::press(Vec2 mouseLocationInView, Camera* cam)
{
	return false;
}

bool CustomUi::GUI::release(Vec2 mouseLocationInView, Camera* cam)
{
	return false;
}

void CustomUi::GUI::keyPress(EventKeyboard::KeyCode keyCode)
{
}

void CustomUi::GUI::keyRelease(EventKeyboard::KeyCode keyCode)
{
}

void CustomUi::GUI::mouseScroll(EventMouse* event)
{
}

void CustomUi::GUI::pushModal(GUI* child)
{
	addChild(child);
}

void CustomUi::GUI::onEnter()
{
	Node::onEnter();
	notifyFocused(this, false);
}

void CustomUi::GUI::onExit()
{
	Node::onExit();
	notifyFocused(this, false);
}

void CustomUi::GUI::setAnchorPoint(const ax::Vec2& anchor)
{
	_anchorPoint = anchor;
	Node::setAnchorPoint(anchor);
}

void CustomUi::GUI::setAnchorOffset(const ax::Vec2& anchorOffset)
{
	_anchorOffset = anchorOffset;
	Node::setAnchorPoint(_anchorPoint * anchorOffset);
}

void CustomUi::GUI::notifyFocused(GUI* sender, bool focused, bool ignoreSelf)
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

void CustomUi::GUI::notifyEnabled()
{
	auto cast = DCAST(GUI, getParent());
	if (cast)
		cast->notifyEnabled();
	else
		updateEnabled(_isEnabled);
}

void CustomUi::GUI::notifyLayout()
{
	auto gui = DCAST(GUI, getParent());
	if (gui) gui->notifyLayout();
}

bool CustomUi::GUI::isEnabled()
{
	return _isEnabled && _isInternalEnabled;
}

bool CustomUi::GUI::isInternalEnabled()
{
	return _isEnabled;
}

void CustomUi::GUI::enable()
{
	_isEnabled = true;
	notifyEnabled();
}

void CustomUi::GUI::disable()
{
	_isEnabled = false;
	notifyEnabled();
}

void CustomUi::GUI::onEnable()
{
}

void CustomUi::GUI::onDisable()
{
}

Size CustomUi::GUI::getFitContentSize()
{
	return getContentSize();
}
