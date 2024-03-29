#include "SelectableObject.h"

Selectable* Selectable::create(ax::Node* child)
{
	Selectable* ret = new Selectable();
	if (ret->init()) {
		ret->drawNode = ax::DrawNode::create(1);
		ret->child = child;
		ret->addChild(child);
		ret->addChild(ret->drawNode);
		ret->drawNode->setVisible(false);
		ret->autorelease();
		return ret;
	}
	AX_SAFE_DELETE(ret);
	return nullptr;
}

bool Selectable::editorDraw(V2D worldSpacePosition)
{
	bool cond = isMouseOver(worldSpacePosition);
	drawNode->setVisible(cond);
	if (_isDrawDirty) {
		drawNode->clear();
		drawNode->drawRect(child->getContentSize() / -2, child->getContentSize() / 2, ax::Color4B::GREEN);
		//drawNode->setGlobalZOrder(UINT32_MAX);
		_isDrawDirty = false;
	}
	return cond;
}

bool Selectable::isMouseOver(V2D worldSpacePosition)
{
	auto rot = worldSpacePosition.rotateByAngle(getPosition(), AX_DEGREES_TO_RADIANS(getRotation()));
	V2D d = rot + getPosition() * V2D(1, -1);
	auto c = child->getContentSize() * getScale();
	return Rect(c.x / -2, c.y / -2, c.x, c.y).containsPoint(d);
}
