#pragma once

#include "fmod/FMODAudioEngine.h"
#include "Helper/Random.h"

namespace SoundGlobals
{
    inline FMOD::Channel* hover_sound;
    inline FMOD::Channel* hold_sound;
    inline FMOD::Channel* click_sound;

    inline void playUiHoverSound()
    {
        bool isPlaying; hover_sound->isPlaying(&isPlaying);
        if (isPlaying) hover_sound->stop();
        hover_sound = FMODAudioEngine::getInstance()->playSound("INC", FMODAudioEngine::platformResourcePath(L"shared/ui_hover2.wav"), false, false);
        hover_sound->setVolume(0.1f);
        hover_sound->setPitch(Random::rangeFloat(0.95f, 1.1f));
    }

    inline void playUiHoldSound()
    {
        bool isPlaying; hold_sound->isPlaying(&isPlaying);
        if (isPlaying) return;
        hold_sound = FMODAudioEngine::getInstance()->playSound("INC", FMODAudioEngine::platformResourcePath(L"shared/ui_hover.wav"), false, false);
        hold_sound->setVolume(0.1f);
        hold_sound->setPitch(Random::rangeFloat(0.97f, 1.05f));
    }

    inline void playUiClickSound()
    {
        bool isPlaying; click_sound->isPlaying(&isPlaying);
        if (isPlaying) return;
        click_sound = FMODAudioEngine::getInstance()->playSound("INC", FMODAudioEngine::platformResourcePath(L"shared/ui_click.wav"), false, false);
        click_sound->setVolume(0.05f);
    }
}
