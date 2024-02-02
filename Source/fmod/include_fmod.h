// Platform specific include header file, include this file
// and get the fmod library working for each specific platform
//#pragma once

#ifdef WIN32
#include "fmod.hpp"
#include "fmod_errors.h"
#include "fmod/FMODAudioEngine.h"
#endif

#ifdef ANDROID
#include "fmod.hpp"
#include "fmod_errors.h"
#endif
