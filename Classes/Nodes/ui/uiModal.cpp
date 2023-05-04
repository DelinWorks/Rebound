#include "uiModal.h"

CustomUi::Modal::Modal()
{
	pushSelf();
}

void CustomUi::Modal::pushSelf()
{
	_modalStack.push(this);
}

void CustomUi::Modal::popSelf()
{
	if (_modalStack.top()  == this)
		_modalStack.pop();
}
