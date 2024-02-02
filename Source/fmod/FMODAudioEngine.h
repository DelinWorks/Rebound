#ifndef FMODAUDIOENGINE_H
#define FMODAUDIOENGINE_H

#include "fmod/include_fmod.h"
#include <functional>
#include <list>
#include <string>
#include <unordered_map>
#include <vector> 
#include "base/Ref.h"
#include "string_manipulation_lib/stringFunctions.hpp"

enum FMODAudioEventType
{
    STOP
};

class AX_DLL FMODAudioEngine : public axmol::Ref
{
public:
    FMODAudioEngine();
    ~FMODAudioEngine();
    
    static FMODAudioEngine * getInstance();

    /**
    WIN32:  $(EXE_PATH)/resources/$filename
    ANROID: file:///android_asset/$filename
    IOS:    I don't fucking know
    Linux:  Uhm i think same as WIN32? Maybe?
    Mac:    I STILL don't fucking know
    **/
    static std::wstring platformResourcePath(std::wstring filename);
    static void destroyInstance();
    
    bool lazyInit();
    
    FMOD::Sound* getOrCreateIfNotExists(const std::wstring &filename, bool stream);
    
    FMOD::Sound* load(const std::wstring &filename);
    FMOD::Sound* loadStream(const std::wstring &filename);
    
    void preload(const std::wstring &filename, bool stream);

    /**
    contain 'soundname' to "INC" to get an ID from an increamented list
    or "RND" for a random number ID ranging from -INT32 to INT32
    */
    FMOD::Channel* playSound(std::string soundname, const std::wstring& filename, bool loop = false, bool stream = false);
    
    FMOD::Channel* getSoundChannel(std::string soundName);

    bool destroySoundChannel(std::string soundName);

    void pauseSound(std::string soundName);
    
    void pauseAllSounds();
    
    void resumeSound(std::string soundName);
    
    void resumeAllSounds();
    
    void stopSound(std::string soundName);
    
    void stopAllSounds();
    
    void setPitch(float pitch);

    void setPan(float pan);
    
    void releaseSound(const std::wstring &filename);
    
    void releaseAllSounds();
    
    void addEventListener(int listener);
    
    void onEvent(FMODAudioEventType eventType, std::string soundName);
    
    int genSoundID();
    
    virtual void update(float delta);
public:
    FMOD::System* _system;
    FMOD::ChannelGroup* _channelGroup;
    FMOD::Channel* _channel;
    static FMOD_RESULT F_CALLBACK systemcallback(FMOD_SYSTEM* system, FMOD_SYSTEM_CALLBACK_TYPE type, void* commanddata1, void* commanddata2);
    
private:
    int _index;
    int _eventListener;
    std::unordered_map<std::wstring, FMOD::Sound*> _sounds;
    std::unordered_map<std::string, FMOD::Channel*> _soundNameMap;
    std::vector<FMOD::Channel*> _soundDeltaVec;
    std::vector<int> _availableIDs;

    int soundIncID = 0;

    float _mainChannelVolTarget = 1.0F;
    float _mainChannelVol = 0.0F;

    float _pitch;
    float _pan;
};

#endif
