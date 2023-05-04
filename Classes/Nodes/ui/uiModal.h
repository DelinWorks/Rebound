#pragma once

#include "uiContainer.h"

namespace CustomUi {
	class Modal : public Container {
	public:
		Modal();

		void pushSelf();
		void popSelf();
	};
}
