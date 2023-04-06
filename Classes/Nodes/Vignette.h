#ifndef __H_VIGNETTE__
#define __H_VIGNETTE__

#include <axmol.h>
#include "shared_scenes/GameUtils.h"

using namespace GameUtils;

class Vignette : public ax::Sprite {
public:
    ax::backend::ProgramState* Program;
    ax::Vec3 stored;

    static Vignette* create(ax::Size visibleSize);

    Vignette* init(ax::Size visibleSize);

    void UpdateVisibleSize(ax::Size visibleSize);

    void SetColor(Vec3 c);

    void SetColorR(f32 r);

    void SetColorG(f32 g);

    void SetColorB(f32 b);

    void SetDistance(f32 d);

    void SetFalloff(f32 f);

    void Hide();

    void Show();
};

#endif
