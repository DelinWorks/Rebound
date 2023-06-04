#pragma once

#include "uiContainer.h"

namespace CUI {
	class Modal : public Container {
	public:
		Modal();

		void pushSelf();
		void popSelf();
	};
}
