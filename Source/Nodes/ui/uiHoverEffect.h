#include <axmol.h>
#include "uiContainer.h"
#include "shared_scenes/GameUtils.h"

#ifndef __H_HOVEREFFECTGUI_
#define __H_HOVEREFFECTGUI_

namespace CUI {
	class HoverEffectGUI : public GUI {
	public:
		HoverEffectGUI();
		~HoverEffectGUI();

		void update(F32 dt, V2D size);
		void hover();

		void setHoverOffset(const V2D& hoverOffset) { _hoverOffset = hoverOffset; }
		V2D getHoverOffset() { return _hoverOffset; }

	protected:
		V2D _hoverOffset = V2D::ZERO;
		ax::ParticleSystemQuad* _prtcl = nullptr;
		bool _isPrtclSimulated = false;
		ax::Sprite* _hoverSprite = nullptr;
		F32 _hoverShaderTime1 = 0;
		F32 _hoverShaderTime2 = 0;
		F32 _hoverShaderTimeLerp2 = 0;
	};
}

#endif
