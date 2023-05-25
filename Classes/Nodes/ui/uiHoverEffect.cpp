#include "uiHoverEffect.h"

CustomUi::HoverEffectGUI::HoverEffectGUI()
{
    auto tex = ADD_IMAGE("shared/unready/ptrn_bg_hover2.png");
    tex->setAliasTexParameters();
    _hoverSprite = ax::Sprite::createWithTexture(tex);
    _hoverShader = GameUtils::createGPUProgram("ui_hover_shader/hover.frag", "ui_hover_shader/hover.vert");
    tex = ADD_IMAGE("shared/unready/ptrn_bg_hover_shader.png");
    tex->setAliasTexParameters();
	SET_UNIFORM_TEXTURE(_hoverShader, "u_tex1", 1, tex->getBackendTexture());
    tex = ADD_IMAGE("shared/unready/ptrn_bg_hover_shader_alpha_edge.png");
    tex->setAliasTexParameters();
	SET_UNIFORM_TEXTURE(_hoverShader, "u_tex2", 2, tex->getBackendTexture());
    _hoverSprite->setProgramState(_hoverShader);
    _hoverSprite->setVisible(false);
    _hoverSprite->setOpacity(30);
    addChild(_hoverSprite, -1);

    _prtcl = ax::ParticleSystemQuad::createWithTotalParticles(300);
    _prtcl->setTexture(ADD_IMAGE("shared/unready/ptrn_bg_hover2.png"));
    _prtcl->setDuration(ParticleSystem::DURATION_INFINITY);
    _prtcl->setGravity({ 0, 0 });
    _prtcl->setEmitterMode(ParticleSystem::Mode::GRAVITY);
    _prtcl->setStartColor(Color4F(1, 1, 1, 0.3));
    _prtcl->setEndColor(Color4F(1, 1, 1, 0));
    _prtcl->setSpeed(15);
    _prtcl->setSpeedVar(3);
    _prtcl->setAngleVar(180);
    _prtcl->setLife(3);
    _prtcl->setLifeVar(1);
    _prtcl->setSpawnFadeIn(1);
    //_prtcl->setSpawnAngleVar(180);
    _prtcl->setStartSize(15);
    _prtcl->setStartSizeVar(10);
    _prtcl->setEndSize(0);
    _prtcl->setEmissionRate(30);
    _prtcl->setEmissionShapes(true);
    _prtcl->setBlendAdditive(true);
    _prtcl->setFixedFPS(30);
    _prtcl->setPositionType(ax::ParticleSystem::PositionType::GROUPED);
    _prtcl->setVisible(false);
    _hoverSprite->addChild(_prtcl, 2);
}

void CustomUi::HoverEffectGUI::update(f32 dt) {
    if (_hoverSprite->isVisible()) {
        _hoverSprite->setContentSize(getContentSize() + _hoverOffset);
        _hoverShaderTime1 += dt;
        SET_UNIFORM(_hoverShader, "u_time", _hoverShaderTime1);
        _hoverShaderTime2 += dt;
        _hoverShaderTimeLerp2 = LERP(_hoverShaderTimeLerp2, _hoverShaderTime2, 10 * dt);
        SET_UNIFORM(_hoverShader, "u_val", _hoverShaderTimeLerp2);
        _prtcl->setPosition((_hoverSprite->getContentSize() + _hoverOffset) / 2);
        _prtcl->setOpacity(_hoverSprite->getOpacity() / 0.1176470588235294);
    }
}

void CustomUi::HoverEffectGUI::hover()
{
    if (isUiFocused()) return;

    if (isUiHovered()) {
        //_pCurrentHoveredItem = this;
        _hoverSprite->stopAllActions();
        _hoverSprite->setVisible(true);
        _prtcl->setVisible(true);
        update(0);
        _prtcl->setEmissionShape(0, ParticleSystem::createRectShape(Vec2::ZERO,
            { _hoverSprite->getContentSize().x + _hoverOffset.x, _hoverSprite->getContentSize().y + _hoverOffset.y }));
        if (!_isPrtclSimulated) {
            _prtcl->simulate(10, 20);
            _isPrtclSimulated = true;
        }
        _hoverSprite->runAction(FadeTo::create(0.1f, 30));
        if (_hoverShaderTime2 > 0.3)
            _hoverShaderTimeLerp2 = 0;
        _hoverShaderTime2 = 0.0f;
        SoundGlobals::playUiHoverSound();
    } else {
        //if (_pCurrentHoveredItem == this)
        //    _pCurrentHoveredItem = nullptr;
        _hoverSprite->stopAllActions();
        auto seq = Sequence::create(
            FadeTo::create(0, 30),
            DelayTime::create(.25 - clampf(_hoverShaderTime1, 0, .25)),
            FadeTo::create(0.5, 0),
            CallFunc::create([this]() { _hoverSprite->setVisible(false); _prtcl->setVisible(false); }),
            _NOTHING
        );
        _hoverSprite->runAction(seq);
    }
}
