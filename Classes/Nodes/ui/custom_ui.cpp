#include "custom_ui.h"

void CustomUi::GUI::setContentSize(const Vec2& size)
{
	if (!size.equals(getContentSize())) {
		Node::setContentSize(size);
		notifyLayout();
	}
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

void CustomUi::GUI::keyPress(EventKeyboard::KeyCode keyCode)
{
}

void CustomUi::GUI::keyRelease(EventKeyboard::KeyCode keyCode)
{
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

void CustomUi::GUI::notifyFocused(GUI* sender, bool focused)
{
	auto cast = DCAST(GUI, getParent());
	if (cast) {
		//if (_isFocused == focused) return;
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
