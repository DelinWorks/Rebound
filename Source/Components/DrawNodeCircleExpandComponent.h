#include <axmol.h>
#include "shared_scenes/GameUtils.h"

#ifndef __H_DRAWNODECIRCLEEXPANDCOMPONENT__
#define __H_DRAWNODECIRCLEEXPANDCOMPONENT__

class DrawNodeCircleExpandComponent : public Component {
public:
    F32 r_start = 0, r_end = 0, time = 0, amount = 0;
    I32 segs = 0;

    DrawNodeCircleExpandComponent(F32 _time, F32 _amount, I32 _segs);

    void update(F32 dt);

    void onAdd();
};

#endif
