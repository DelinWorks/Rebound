#ifndef __H_VIGNETTE__
#define __H_VIGNETTE__

#include <axmol.h>
#include "shared_scenes/GameUtils.h"

class Vignette : public ax::Sprite {
public:
    ax::backend::ProgramState* Program;
    ax::Vec3 stored;

    static Vignette* create(ax::Size visibleSize);

    Vignette* init(ax::Size visibleSize);

    void UpdateVisibleSize(ax::Size visibleSize);

    void SetColor(Vec3 c);

    void SetColorR(F32 r);

    void SetColorG(F32 g);

    void SetColorB(F32 b);

    void SetDistance(F32 d);

    void SetFalloff(F32 f);

    void Hide();

    void Show();
};

#endif
