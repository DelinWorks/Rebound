#include "uiHoverEffect.h"

CustomUi::HoverEffectGUI::HoverEffectGUI()
{
    auto tex = ADD_IMAGE("shared/unready/ptrn_bg_hover2.png");
    tex->setAliasTexParameters();
    _sprite = ax::Sprite::createWithTexture(tex);
	//setOpacity(0);
    hover_shader = GameUtils::createGPUProgram("ui_hover_shader/hover.frag", "ui_hover_shader/hover.vert");
	SET_UNIFORM_TEXTURE(hover_shader, "u_tex1", 1, ADD_IMAGE("shared/unready/ptrn_bg_hover_shader.png")->getBackendTexture());
    _sprite->setProgramState(hover_shader);
    addChild(_sprite, -1);
}

void CustomUi::HoverEffectGUI::update(f32 dt)
{
    _hoverShaderTime += dt;
    SET_UNIFORM(hover_shader, "u_time", (float)_hoverShaderTime);
    SET_UNIFORM(hover_shader, "u_val", (float)1);
}

void CustomUi::HoverEffectGUI::hover(bool hovered, ax::Vec2 dSize)
{
    _sprite->setContentSize(dSize);

    //if (_hover_animation_time != -1)
    //{
    //    if (_hover_animation_time >= 0.25 && (!_isFocused && !hovered))
    //    {
    //        _hover_animation_time = -1;
    //        _sprite->stopAllActions();
    //        _sprite->runAction(
    //            Sequence::create(
    //                FadeTo::create(0.1, 0),
    //                CallFunc::create([this]() { _sprite->setVisible(false); }),
    //                _NOTHING
    //            )
    //        );
    //    }
    //}

    //if (_hover_animation_time != -1)
    //{
    //    _hover_animation_time += Director::getInstance()->getDeltaTime();
    //    _hover_animation_time = _hover_animation_time > 1.0 ? 1.0 : _hover_animation_time;

    //    if (_hover_animation_step > 0)
    //    {
    //        _hover_animation_step = 0;
    //        SET_UNIFORM(_sprite->getProgramState(), "u_val", (float)_hover_animation_time);
    //    }
    //    else _hover_animation_step += Director::getInstance()->getDeltaTime();
    //}
}

void CustomUi::HoverEffectGUI::reset()
{
    //SoundGlobals::playUiHoverSound();
    //_sprite->stopAllActions();
    //_sprite->runAction(
    //    Sequence::create(
    //        CallFunc::create([this]() { _sprite->setVisible(true); }),
    //        FadeTo::create(0.1f, 100),
    //        _NOTHING
    //    )
    //);
    //_hover_animation_time = 0.0;
    //_hover_animation_step = 0.0;
    //SET_UNIFORM(_sprite->getProgramState(), "u_val", (float)0.0);
}
