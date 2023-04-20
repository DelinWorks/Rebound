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
		ax::ParticleSystemQuad* prtcl;
		ax::backend::ProgramState* _hoverShader = nullptr;
		ax::Sprite* _hoverSprite = nullptr;
		f32 _hoverShaderTime1 = 0;
		f32 _hoverShaderTime2 = 0;
		f32 _hoverShaderTimeLerp2 = 0;
	};
}

#endif
