#pragma once

#include "axmol.h"

namespace SelectionBox {
	struct Line {
		ax::Vec2 begin;
		ax::Vec2 end;
	};

	struct Triangle {
		ax::Vec2 p0;
		ax::Vec2 p1;
		ax::Vec2 p2;
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