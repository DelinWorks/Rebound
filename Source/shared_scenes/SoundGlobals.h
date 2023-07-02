#pragma once

#include "fmod/FMODAudioEngine.h"
#include "Helper/Random.h"

namespace SoundGlobals
{
    inline void playUiHoverSound()
    {
        FMODAudioEngine::getInstance()->stopSound("ui_hover");
        auto sound = FMODAudioEngine::getInstance()->playSound("ui_hover", FMODAudioEngine::platformResourcePath(L"shared/ui_hover.ogg"), false, false);
        //sound->setPosition(159000, FMOD_TIMEUNIT_MS);
        sound->setVolume(0.3f);
        sound->setPitch(Random::rangeFloat(0.97f, 1.05f));
    }
}
