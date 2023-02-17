#include "cocos2d.h"

#ifndef SELECTIONBOX_H
#define SELECTIONBOX_H

namespace SelectionBox {
	struct Line {
		cocos2d::Vec2 begin;
		cocos2d::Vec2 end;
	};

	struct Triangle {
		cocos2d::Vec2 p0;
		cocos2d::Vec2 p1;
		cocos2d::Vec2 p2;
	};

	struct Box {
		Line left;
		Line right;
		Line top;
		Line bottom;
		Triangle first;
		Triangle second;
	};
}

#endif