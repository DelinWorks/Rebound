#pragma once

#include "shared_scenes/GameUtils.h"
#include <2d/Node.h>
#include <2d/DrawNode.h>

class Selectable : public ax::Node {
public:
	static Selectable* create(ax::Node* child);

	bool editorDraw(ax::Vec2 worldSpacePosition);
	bool isMouseOver(ax::Vec2 worldSpacePosition);

private:
	bool _isDrawDirty = true;
	ax::DrawNode* drawNode;
	ax::Node* child;

};
