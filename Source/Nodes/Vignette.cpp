#include "Vignette.h"

USING_NS_GAMEUTILS;

Vignette* Vignette::create(ax::Size visibleSize) {
    Vignette* ret = new Vignette();
    if (ret->init(visibleSize))
    {
        ret->autorelease();
    }
    else
    {
        AX_SAFE_DELETE(ret);
    }
    return ret;
}

Vignette* Vignette::init(ax::Size visibleSize) {
    auto tex = ADD_IMAGE_ALIAS(tex, "omf_2.png");
    initWithTexture(tex);
    setContentSize(visibleSize);
    Program = createGPUProgram("shaders/vignette.fsh", "shaders/vignette.vsh");
    setProgramState(Program);
    return this;
}

void Vignette::UpdateVisibleSize(ax::Size visibleSize) {
    setContentSize(visibleSize);
}

void Vignette::SetColor(Vec3 c) {
    stored = c;
    SET_UNIFORM(Program, "_color", c);
}

void Vignette::SetColorR(f32 r) {
    Vec3 col = Vec3(r, stored.y, stored.z);
    SET_UNIFORM(Program, "_color", col);
    stored = col;
}

void Vignette::SetColorG(f32 g) {
    Vec3 col = Vec3(stored.x, g, stored.z);
    SET_UNIFORM(Program, "_color", col);
    stored = col;
}

void Vignette::SetColorB(f32 b) {
    Vec3 col = Vec3(stored.x, stored.y, b);
    SET_UNIFORM(Program, "_color", col);
    stored = col;
}

void Vignette::SetDistance(f32 d) {
    SET_UNIFORM(Program, "_distance", d);
}

void Vignette::SetFalloff(f32 f) {
    SET_UNIFORM(Program, "_falloff", f);
}

void Vignette::Hide() {
    setVisible(false);
}

void Vignette::Show() {
    setVisible(true);
}
