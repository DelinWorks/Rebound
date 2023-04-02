#include "custom_ui.h"

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

void CustomUi::GUI::onEnter()
{
	notifyFocused(false);
}

void CustomUi::GUI::notifyFocused(bool focused)
{
	focused |= _isForceFocused;
	auto cast = DCAST(GUI, getParent());
	if (cast)
		cast->notifyFocused(focused);
	else if (_isFocused != focused) {
		_skipCallback = true;
		_isFocused = focused;
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
