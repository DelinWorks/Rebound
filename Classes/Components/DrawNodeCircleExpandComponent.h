#include <axmol.h>
#include "shared_scenes/GameUtils.h"

#ifndef __H_DRAWNODECIRCLEEXPANDCOMPONENT__
#define __H_DRAWNODECIRCLEEXPANDCOMPONENT__

class DrawNodeCircleExpandComponent : public Component {
public:
    f32 r_start, r_end, time, amount;
    i32 segs;

    DrawNodeCircleExpandComponent(f32 _time, f32 _amount, i32 _segs);

    void update(f32 dt);

    void onAdd();
};

#endif
