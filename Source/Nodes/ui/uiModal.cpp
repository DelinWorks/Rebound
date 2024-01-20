#include "uiModal.h"

CUI::Modal::Modal()
{
	pushSelf();
}

void CUI::Modal::pushSelf()
{
	_modalStack.push(this);
}

void CUI::Modal::popSelf()
{
	if (_modalStack.size() > 0 && _modalStack.top() == this)
		_modalStack.pop();
}

CUI::Modal::~Modal()
{
	LOG_RELEASE;
}
