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

    prtcl = ax::ParticleSystemQuad::createWithTotalParticles(300);
    prtcl->setTexture(ADD_IMAGE("shared/unready/ptrn_bg_hover2.png"));
    prtcl->setDuration(ParticleSystem::DURATION_INFINITY);
    prtcl->setGravity({ 0, 0 });
    prtcl->setEmitterMode(ParticleSystem::Mode::GRAVITY);
    prtcl->setStartColor(Color4F(1, 1, 1, 0.3));
    prtcl->setEndColor(Color4F(1, 1, 1, 0));
    prtcl->setSpeed(10);
    prtcl->setSpeedVar(3);
    prtcl->setAngleVar(180);
    prtcl->setLife(3);
    prtcl->setLifeVar(1);
    prtcl->setSpawnFadeIn(1);
    prtcl->setSpawnAngleVar(180);
    prtcl->setStartSize(15);
    prtcl->setStartSizeVar(10);
    prtcl->setEndSize(0);
    prtcl->setEmissionRate(30);
    prtcl->setEmissionShapes(true);
    prtcl->setBlendAdditive(true);
    prtcl->simulate(-1, 10);
    prtcl->setFixedFPS(20);
    prtcl->setPositionType(ax::ParticleSystem::PositionType::GROUPED);
    _hoverSprite->addChild(prtcl, 2);
}

void CustomUi::HoverEffectGUI::update(f32 dt) {
    if (_hoverSprite->isVisible()) {
        _hoverSprite->setContentSize(getContentSize() + Vec2(30 * (getContentSize().x / 120), 30));
        _hoverShaderTime1 += dt;
        SET_UNIFORM(_hoverShader, "u_time", _hoverShaderTime1);
        _hoverShaderTime2 += dt;
        _hoverShaderTimeLerp2 = LERP(_hoverShaderTimeLerp2, _hoverShaderTime2, 10 * dt);
        SET_UNIFORM(_hoverShader, "u_val", _hoverShaderTimeLerp2);
        prtcl->setPosition(_hoverSprite->getContentSize() / 2);
        prtcl->setEmissionShape(0, ParticleSystem::createRectShape(Vec2::ZERO,
            { _hoverSprite->getContentSize().x, _hoverSprite->getContentSize().y }));
        prtcl->setTimeScale(1);
    } else prtcl->setTimeScale(0);
}

void CustomUi::HoverEffectGUI::hover()
{
    if (_isFocused) return;

    if (_isHovered) {
        _hoverSprite->stopAllActions();
        _hoverSprite->runAction(
            Sequence::create(
                CallFunc::create([this]() { _hoverSprite->setVisible(true); }),
                FadeTo::create(0.1f, 30),
                _NOTHING
            )
        );
        if (_hoverShaderTime2 > 0.3)
            _hoverShaderTimeLerp2 = 0;
        _hoverShaderTime2 = 0.0f;
        SoundGlobals::playUiHoverSound();
    } else {
        _hoverSprite->stopAllActions();
        _hoverSprite->runAction(
            Sequence::create(
                FadeTo::create(0, 30),
                DelayTime::create(.25 - clampf(_hoverShaderTime1, 0, .25)),
                FadeTo::create(0.1f, 0),
                CallFunc::create([this]() { _hoverSprite->setVisible(false); }),
                _NOTHING
            )
        );
    }
}
