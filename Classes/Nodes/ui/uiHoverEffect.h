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
		void hover(bool hovered, ax::Vec2 dSize);
		void reset();

	private:
		ax::backend::ProgramState* hover_shader;
		float _hover_animation_time;
		float _hover_animation_step;
		f32 _hoverShaderTime;
		ax::Sprite* _sprite;
	};
}

#endif
