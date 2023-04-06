#include "uiHoverEffect.h"

CustomUi::HoverEffectGUI::HoverEffectGUI()
{
    auto tex = ADD_IMAGE("shared/unready/ptrn_bg_hover2.png");
    tex->setAliasTexParameters();
    _hoverSprite = ax::Sprite::createWithTexture(tex);
    _hoverShader = GameUtils::createGPUProgram("ui_hover_shader/hover.frag", "ui_hover_shader/hover.vert");
	SET_UNIFORM_TEXTURE(_hoverShader, "u_tex1", 1, ADD_IMAGE("shared/unready/ptrn_bg_hover_shader.png")->getBackendTexture());
	SET_UNIFORM_TEXTURE(_hoverShader, "u_tex2", 2, ADD_IMAGE("shared/unready/ptrn_bg_hover_shader_alpha_edge.png")->getBackendTexture());
    _hoverSprite->setProgramState(_hoverShader);
    _hoverSprite->setVisible(false);
    addChild(_hoverSprite, -1);
}

void CustomUi::HoverEffectGUI::update(f32 dt) {
    if (_hoverSprite->isVisible()) {
        _hoverSprite->setContentSize(getContentSize());
        _hoverShaderTime1 += dt;
        SET_UNIFORM(_hoverShader, "u_time", _hoverShaderTime1);
        _hoverShaderTime2 += dt;
        _hoverShaderTimeLerp2 = LERP(_hoverShaderTimeLerp2, _hoverShaderTime2, 10 * dt);
        SET_UNIFORM(_hoverShader, "u_val", _hoverShaderTimeLerp2);
    }
}

void CustomUi::HoverEffectGUI::hover()
{
    if (_isFocused) return;

    if (_isHovered) {
        _hoverSprite->stopAllActions();
        _hoverSprite->runAction(
            Sequence::create(
                CallFunc::create([this]() { _hoverSprite->setVisible(true); }),
                FadeTo::create(0.1f, 100),
                _NOTHING
            )
        );
        if (_hoverShaderTime2 > 0.3)
            _hoverShaderTimeLerp2 = 0;
        _hoverShaderTime1 = 0.0f;
        _hoverShaderTime2 = 0.0f;
    } else {
        _hoverSprite->stopAllActions();
        _hoverSprite->runAction(
            Sequence::create(
                FadeTo::create(0, 100),
                DelayTime::create(.25 - clampf(_hoverShaderTime1, 0, .25)),
                FadeTo::create(0.1f, 0),
                CallFunc::create([this]() { _hoverSprite->setVisible(false); }),
                _NOTHING
            )
        );
    }
}
