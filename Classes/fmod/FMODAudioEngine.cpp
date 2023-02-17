#include "fmod/FMODAudioEngine.h"
#include "cocos2d.h"
#include "Helper/PlatDefines.h"
#include "string_manipulation_lib/stringFunctions.hpp"
#include "shared_scenes/GameUtils.h"
#include <fstream>
#include <iostream>
#include <string>

#include "ntcvt/ntcvt.hpp"

void ERRCHECK_fn(FMOD_RESULT result, const char *file, int line) {
    if (result != FMOD_OK)
    {
#ifdef WIN32
        printf("%s(%d): FMOD error: %s\n", file, line, FMOD_ErrorString(result));
#endif
    }
}
#define ERRCHECK(_result) ERRCHECK_fn(_result, __FILENAME__, __LINE__)

using namespace cocos2d;

bool is_file_exist(const char* fileName)
{
#ifdef WIN32
    std::ifstream infile(fileName);
    return infile.good();
#else
    return true;
#endif
}

bool is_file_existw(const wchar_t* fileName)
{
#ifdef WIN32
    std::ifstream infile(fileName);
    return infile.good();
#else
    return true;
#endif
}

char* wchar_to_char(const wchar_t* pwchar)
{
    // get the number of characters in the string.
    int currentCharIndex = 0;
    char currentChar = pwchar[currentCharIndex];

    while (currentChar != '\0')
    {
        currentCharIndex++;
        currentChar = pwchar[currentCharIndex];
    }

    const int charCount = currentCharIndex + 1;

    // allocate a new block of memory size char (1 byte) instead of wide char (2 bytes)
    char* filePathC = (char*)malloc(sizeof(char) * charCount);

    for (int i = 0; i < charCount; i++)
    {
        // convert to char (1 byte)
        char character = pwchar[i];

        *filePathC = character;

        filePathC += sizeof(char);

    }
    filePathC += '\0';

    filePathC -= (sizeof(char) * charCount);

    return filePathC;
}

//struct AudioOBJ
//{
//public:
//    AudioOBJ(FMOD::Channel* c, FMOD::Sound* s)
//    {
//        channel = c;
//        sound = s;
//    }
//
//    FMOD::Channel* channel;
//    FMOD::Sound* sound;
//};

static FMODAudioEngine* instance = nullptr;

FMODAudioEngine* FMODAudioEngine::getInstance()
{
    if (instance == nullptr)
    {
        instance = new FMODAudioEngine();
        instance->lazyInit();
    }
    
    return instance;
}

std::wstring FMODAudioEngine::platformResourcePath(std::wstring filename)
{
    std::wstring fullPath = L"";
#if AX_TARGET_PLATFORM == AX_PLATFORM_WIN32
    fullPath = Darkness::getInstance()->res_path + filename;
#endif
#if AX_TARGET_PLATFORM == AX_PLATFORM_ANDROID
    fullPath = L"file:///android_asset/" + filename;
#endif
    return fullPath;
}

void FMODAudioEngine::destroyInstance()
{
    if (instance)
    {
        delete instance;
        instance = nullptr;
    }
}



FMODAudioEngine::FMODAudioEngine()
:_index(0)
,_eventListener(0)
,_pitch(1.0F)
,_pan(0.0F)
,_system(NULL)
,_channelGroup(NULL)
,_channel(NULL)
{
}

FMODAudioEngine::~FMODAudioEngine()
{
    Director::getInstance()->getScheduler()->unscheduleUpdate(this);
    releaseAllSounds();
    _system->close();
    _system->release();
}

bool FMODAudioEngine::lazyInit()
{
    FMOD_RESULT       result;
    unsigned int      version;
    
    result = FMOD::System_Create(&_system);
    ERRCHECK(result);
    
    result = _system->init(256, FMOD_INIT_NORMAL, NULL);
    ERRCHECK(result);
    
    result = _system->getVersion(&version);
    ERRCHECK(result);
    
    if (version < FMOD_VERSION)
    {
        printf("FMOD: lib version %08x doesn't match header version %08x\n", version, FMOD_VERSION);
    }
    
    result = _system->createChannelGroup("TOP_NODE", &_channelGroup);
    ERRCHECK(result);

    printf("FMOD: lib version %08x\n      Engine has been Initialized with Update\n      MAX_CHANNELS: 256\n      CHANNEL_GROUP: TOP_NODE\n", version);
    Director::getInstance()->getScheduler()->scheduleUpdate(this, 0, false);
    return true;
}

void FMODAudioEngine::update(float fDelta)
{
    FMOD_RESULT result;
    result = _system->update();

    f32 lerpSpeed = 1;

    if (_mainChannelVol != _mainChannelVolTarget && _mainChannelVol > _mainChannelVolTarget)
    {
        _mainChannelVol -= fDelta * lerpSpeed;

        if (_mainChannelVol <= _mainChannelVolTarget)
            _mainChannelVol = _mainChannelVolTarget;

        f32 vol = powf(_mainChannelVol, 1.0F / 0.9F);
        result = _channelGroup->setVolume(vol);
    }

    if (_mainChannelVol != _mainChannelVolTarget && _mainChannelVol < _mainChannelVolTarget)
    {
        _mainChannelVol += fDelta * lerpSpeed;

        if (_mainChannelVol >= _mainChannelVolTarget)
            _mainChannelVol = _mainChannelVolTarget;

        f32 vol = powf(_mainChannelVol, 1.0F / 2.0F);
        result = _channelGroup->setVolume(vol);
    }
    
    if (fDelta > .0f)
    {
        for (auto& i : _soundDeltaVec)
            i->setPosition(Director::getInstance()->getDeltaTime() * 1000, FMOD_TIMEUNIT_MS);
        _soundDeltaVec.clear();
    }

    auto it = _soundNameMap.begin();
    while (it != _soundNameMap.end())
    {
        FMOD::Channel* channel = it->second;
        bool isPlaying = false;
        result = channel->isPlaying(&isPlaying);
        if ((result != FMOD_OK) && (result != FMOD_ERR_INVALID_HANDLE) && (result != FMOD_ERR_CHANNEL_STOLEN))
        {
            ERRCHECK(result);
        }
        bool paused = false;
        result = channel->getPaused(&paused);
        if ((result != FMOD_OK) && (result != FMOD_ERR_INVALID_HANDLE) && (result != FMOD_ERR_CHANNEL_STOLEN))
        {
            ERRCHECK(result);
        }
        
        if (!paused && !isPlaying)
        {
            std::string nSoundName = it->first;
            this->onEvent(FMODAudioEventType::STOP, nSoundName);
            _soundNameMap.erase(it++);
        }
        else
        {
            ++it;
        }
    }
}

void FMODAudioEngine::releaseSound(const std::wstring &filename)
{
    this->update(0);
    auto it = _sounds.find(filename);
    FMOD::Sound* sound = NULL;
    if (it != _sounds.end())
    {
        sound = it->second;
        FMOD_RESULT result = sound->release();
        ERRCHECK(result);
    }
    else
    {
        wprintf(L"FMOD: The specified resource %s is not loaded, so it can't be unloaded\n", filename.c_str());
    }
}

void FMODAudioEngine::releaseAllSounds()
{
    this->update(0);
    FMOD_RESULT result;
    for (auto& s : _sounds)
    {
        FMOD::Sound* sound = s.second;
        result = sound->release();
        ERRCHECK(result);
    }
    _sounds.clear();
}

FMOD::Sound* FMODAudioEngine::load(const std::wstring &filename)
{
    FMOD_RESULT result;
    FMOD::Sound *sound;
    if (!is_file_existw(filename.c_str()))
    {
        wprintf(L"FMOD: preload file not found %s\n", filename.c_str());
        return NULL;
    }
    result = _system->createSound(Strings::narrow(filename.c_str()), FMOD_DEFAULT, 0, &sound);
    ERRCHECK(result);
    wprintf(L"FMOD: preload file %s was loaded to memory.\n", Strings::wreplace_const(filename, Darkness::getInstance()->res_path, L"resources/").c_str());
    return sound;
}

FMOD::Sound* FMODAudioEngine::loadStream(const std::wstring& filename)
{
    FMOD_RESULT result;
    FMOD::Sound* sound;
    if (!is_file_existw(filename.c_str()))
    {
        wprintf(L"FMOD: stream file not found %s\n", filename.c_str());
        return NULL;
    }
    result = _system->createStream(Strings::narrow(filename.c_str()), FMOD_DEFAULT, 0, &sound);
    ERRCHECK(result);
    wprintf(L"FMOD: preload file %s was streamed.\n", Strings::wreplace_const(filename, Darkness::getInstance()->res_path, L"resources/").c_str());
    return sound;
}

FMOD::Sound* FMODAudioEngine::getOrCreateIfNotExists(const std::wstring &filename, bool stream)
{
    this->update(0);
    auto it = _sounds.find(filename);
    FMOD::Sound* sound = NULL;
    if (it != _sounds.end())
    {
        sound = it->second;
    }
    if (sound == NULL)
    {
        sound = stream ? loadStream(filename) : load(filename);
        if (sound)
        {
            _sounds.emplace(filename, sound);
        }
    }
    
    return sound;
}

void FMODAudioEngine::preload(const std::wstring &filename, bool stream)
{
    getOrCreateIfNotExists(filename, stream);
}

FMOD::Channel* FMODAudioEngine::playSound(std::string soundname, const std::wstring &filename, bool loop, bool stream)
{
    this->update(0);
    FMOD_RESULT result;
    Strings::replace(soundname, "RND", std::to_string((rand() % 2147483647) + 0));
    Strings::replace(soundname, "INC", std::to_string(soundIncID++));

    FMOD::Sound* sound = getOrCreateIfNotExists(filename, stream);
    
    if (sound == nullptr)
    {
        return NULL;
    }
    
    FMOD::Channel* channel = nullptr;
    
    result = sound->setMode(loop ? FMOD_LOOP_NORMAL : FMOD_LOOP_OFF);
    ERRCHECK(result);
    
    if (loop)
    {
        result = sound->setLoopCount(-1);
        ERRCHECK(result);
    }
    else
    {
        result = sound->setLoopCount(0);
        ERRCHECK(result);
    }

    FMOD_REVERB_PROPERTIES default_prop = FMOD_PRESET_CONCERTHALL;
    ERRCHECK(_system->setReverbProperties(0, &default_prop));

    result = _system->playSound(sound, _channelGroup, false, &channel);

    channel->setPosition(Director::getInstance()->getDeltaTime() * 1000, 0);

    _soundDeltaVec.push_back(channel);
    //FMOD::DSP* dsp;
    //result = _system->createDSPByType(FMOD_DSP_TYPE_ITECHO, &dsp);
    //dsp->setParameterFloat(FMOD_DSP_ITECHO_WETDRYMIX, 6);
    //channel->addDSP(0, dsp);

    result = channel->setReverbProperties(0, 0);

    //float pitch = 1;
    //dsp->setParameterFloat(FMOD_DSP_PITCHSHIFT_PITCH, 1 / pitch);
    //channel->setPitch(pitch);

    _soundNameMap.emplace(soundname, channel);
    return channel;
}

FMOD::Channel* FMODAudioEngine::getSoundChannel(std::string soundName)
{
    this->update(0);
    FMOD::Channel* channel = NULL;
    auto it = _soundNameMap.find(soundName);
    FMOD_RESULT result;
    if (it != _soundNameMap.end()) {
        channel = it->second;
        return channel;
    }
    else
    {
        //CCLOG("FMOD: sound by name %s wasn't found.", soundName.c_str());
        return NULL;
    }
}

bool FMODAudioEngine::destroySoundChannel(std::string soundName)
{
    this->update(0);
    FMOD::Channel* channel = NULL;
    auto it = _soundNameMap.find(soundName);
    FMOD_RESULT result;
    if (it != _soundNameMap.end()) {
        channel = it->second;
        channel->stop();
        return true;
    }
    else
    {
        printf("FMOD: sound by name %s wasn't found.\n", soundName.c_str());
        return false;
    }
}

void FMODAudioEngine::pauseSound(std::string soundName)
{
    this->update(0);
    FMOD::Channel* channel = NULL;
    auto it = _soundNameMap.find(soundName);
    FMOD_RESULT result;
    if (it != _soundNameMap.end()) {
        channel = it->second;
        result = channel->setPaused(true);
        ERRCHECK(result);
    }
    else
    {
        printf("FMOD: sound by name %s wasn't found.\n", soundName.c_str());
    }
}

void FMODAudioEngine::resumeSound(std::string soundName)
{
    this->update(0);
    FMOD::Channel* channel = NULL;
    auto it = _soundNameMap.find(soundName);
    FMOD_RESULT result;
    if (it != _soundNameMap.end()) {
        channel = it->second;
        result = channel->setPaused(false);
        ERRCHECK(result);
    }
    else
    {
        printf("FMOD: sound by name %s wasn't found.\n", soundName.c_str());
    }
}

void FMODAudioEngine::pauseAllSounds()
{
    FMOD_RESULT result;
    _mainChannelVolTarget = 0.0F;
    //ERRCHECK(result);
}

void FMODAudioEngine::resumeAllSounds()
{
    FMOD_RESULT result;
    _mainChannelVolTarget = 1.0F;
    //ERRCHECK(result);
}

void FMODAudioEngine::stopSound(std::string soundName)
{
    this->update(0);
    FMOD::Channel* channel = NULL;
    auto it = _soundNameMap.find(soundName);
    FMOD_RESULT result;
    if (it != _soundNameMap.end()) {
        channel = it->second;
        result = channel->stop();
        ERRCHECK(result);
    }
    //else
    //{
    //    CCLOG("FMOD: sound by name %s wasn't found.", soundName.c_str());
    //}
}

void FMODAudioEngine::stopAllSounds()
{
    this->update(0);
    FMOD_RESULT result;
    result = _channelGroup->stop();
    ERRCHECK(result);
}

void FMODAudioEngine::setPitch(float pitch)
{
    if (pitch == _pitch)
    {
        return;
    }
    _pitch = pitch;
    
    FMOD_RESULT result;
    result = _channelGroup->setPitch(pitch);
    ERRCHECK(result);
}

void FMODAudioEngine::setPan(float pan)
{
    if (pan == _pan)
    {
        return;
    }
    _pitch = pan;

    FMOD_RESULT result;
    result = _channelGroup->setPan(pan);
    ERRCHECK(result);
}

int FMODAudioEngine::genSoundID()
{
    int nID = -1;
    if (_availableIDs.size() > 0)
    {
        nID = _availableIDs.at(0);
        _availableIDs.erase(_availableIDs.begin());
        return nID;
    }
    
    _index++;
    nID = _index;
    return nID;
}

void FMODAudioEngine::addEventListener(int listener)
{
    if (_eventListener != 0)
    {
        ScriptEngineManager::getInstance()->getScriptEngine()->removeScriptHandler(_eventListener);
    }
    _eventListener = listener;
}

void FMODAudioEngine::onEvent(FMODAudioEventType eventType, std::string soundName)
{
    //if (_eventListener != 0)
    //{
    //    LuaStack *luaStack = LuaEngine::getInstance()->getLuaStack();
    //    lua_State *L = luaStack->getLuaState();
    //    luaStack->pushInt(eventType);
    //    luaStack->pushInt(soundID);
    //    LuaEngine::getInstance()->getLuaStack()->executeFunctionByHandler(_eventListener, 2);
    //    CommonScriptData data(_eventListener, StringUtils::format("stop_%d", soundName).c_str());
    //    ScriptEvent event(kCommonEvent, (void*)&data);
    //    ScriptEngineProtocol* engine = ScriptEngineManager::getInstance()->getScriptEngine();
    //    engine->sendEvent(&event);
    //}
}

#if WIN32
FMOD_RESULT F_CALLBACK FMODAudioEngine::systemcallback(FMOD_SYSTEM* system, FMOD_SYSTEM_CALLBACK_TYPE type, void* commanddata1, void* commanddata2)
{
    FMOD::System* sys = (FMOD::System*)system;

    switch (type)
    {
    case FMOD_SYSTEM_CALLBACK_DEVICELISTCHANGED:
    {
        int numdrivers;


        printf("NOTE : FMOD_SYSTEM_CALLBACKTYPE_DEVICELISTCHANGED occured.\n");


        sys->getNumDrivers(&numdrivers);


        printf("Numdevices = %d\n", numdrivers);
        break;
    }
    case FMOD_SYSTEM_CALLBACK_MEMORYALLOCATIONFAILED:
    {
        printf("ERROR : FMOD_SYSTEM_CALLBACKTYPE_MEMORYALLOCATIONFAILED occured.\n");
        printf("%s.\n", commanddata1);
        printf("%d bytes.\n", commanddata2);
        break;
    }
    case FMOD_SYSTEM_CALLBACK_THREADCREATED:
    {
        printf("NOTE : FMOD_SYSTEM_CALLBACKTYPE_THREADCREATED occured.\n");
        printf("Thread ID = %d\n", (int)commanddata1);
        printf("Thread Name = %s\n", (char*)commanddata2);
        break;
    }
    case FMOD_SYSTEM_CALLBACK_BADDSPCONNECTION:
    {
        FMOD::DSP* source = (FMOD::DSP*)commanddata1;
        FMOD::DSP* dest = (FMOD::DSP*)commanddata2;


        printf("ERROR : FMOD_SYSTEM_CALLBACKTYPE_BADDSPCONNECTION occured.\n");
        if (source)
        {
            char name[256];
            source->getInfo(name, 0, 0, 0, 0);
            printf("SOURCE = %s\n", name);
        }
        if (dest)
        {
            char name[256];
            dest->getInfo(name, 0, 0, 0, 0);
            printf("DEST = %s\n", name);
        }
        break;
    }
    }

    return FMOD_OK;
}
#endif