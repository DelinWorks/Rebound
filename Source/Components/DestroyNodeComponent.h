#include <axmol.h>
#include "shared_scenes/GameUtils.h"

#ifndef __H_DESTROYNODECOMPONENT__
#define __H_DESTROYNODECOMPONENT__

class DestroyNodeComponent : public Component {
public:
    F32 time_s = 0;
    F32 time_e = 0;

    DestroyNodeComponent(F32 _time);

    void update(F32 dt);

    void onAdd();
};

#endif
