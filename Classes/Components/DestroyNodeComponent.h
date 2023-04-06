#include <axmol.h>
#include "shared_scenes/GameUtils.h"

#ifndef __H_DESTROYNODECOMPONENT__
#define __H_DESTROYNODECOMPONENT__

class DestroyNodeComponent : public Component {
public:
    f32 time_s;
    f32 time_e;

    DestroyNodeComponent(f32 _time);

    void update(f32 dt);

    void onAdd();
};

#endif
