#include <axmol.h>
#include "uiContainer.h"
#include "shared_scenes/GameUtils.h"

#ifndef __H_HOVEREFFECTGUI_
#define __H_HOVEREFFECTGUI_

namespace CustomUi {
	class HoverEffectGUI : public GUI {
	protected:
		HoverEffectGUI();

		void update(f32 dt);
		void hover();

	private:
		ax::backend::ProgramState* _hoverShader;
		ax::Sprite* _hoverSprite;
		f32 _hoverShaderTime1;
		f32 _hoverShaderTime2;
		f32 _hoverShaderTimeLerp2 = 0;
	};
}

#endif
