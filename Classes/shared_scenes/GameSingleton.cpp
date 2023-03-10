#include "GameSingleton.h"
#include "WelcomeScene.h"

#ifdef WIN32
#include <Psapi.h>
#include <string.h>
#include <stdlib.h>
#include <windows.h>
#include <tlhelp32.h>
#endif
#include <string.h>

static Darkness* _darkness = nullptr;

Darkness::Darkness()
{
}

Darkness::~Darkness()
{
}

Darkness* Darkness::getInstance()
{
    if (_darkness == nullptr)
    {
        _darkness = new Darkness();
        _darkness->init();
    }
    return _darkness;
}

void Darkness::init()
{
    Director::getInstance()->getScheduler()->scheduleUpdate(this, 1, false);

    console.isHeadless = false;

    res_path = getCurrentResourcesDirectoryW();

#ifdef _DEBUG
    res_path = L"C:/Users/turky/Documents/GitHub/DarknessAmongUs/Resources/";
#endif

    server_path = "https://delingames.xyz/";

    {
        gameWindow.isFullscreen = true;
        gameWindow.isScreenSizeDirty = false;
        gameWindow.isCursorLockedToWindow = true;
        gameWindow.isAllowedToLeave = false;
        gameWindow.lastKnownWindowRect = cocos2d::Rect(0, 0, 1280, 720);
        gameWindow.windowPolicy = ResolutionPolicy::SHOW_ALL;
        gameWindow.currentWindowCursor = 0;
    }
}

void Darkness::setupController()
{
    ax::Controller::stopDiscoveryController();
    ax::Controller::startDiscoveryController();
}

ax::Controller::KeyStatus Darkness::getKeyState(ax::Controller::Key key)
{
    auto controllers = ax::Controller::getAllController();
    if (controllers.size() > 0)
        for (auto& c : controllers)
            if (c->getDeviceName().length() > 0)
            {
                return c->getKeyStatus(key);
                break;
            }
    return ax::Controller::KeyStatus{0,0,0};
}

void Darkness::destroyInstance()
{
    Director::getInstance()->getScheduler()->unscheduleUpdate(_darkness);
    _darkness->gameWindow.isAllowedToLeave = true;
    Director::getInstance()->end();
}

void Darkness::restartInstance()
{
    //auto old_scene = Director::getInstance()->getRunningScene();
    //if (old_scene != nullptr)
    //    old_scene->autorelease();
    Director::getInstance()->popToRootScene();
    Director::getInstance()->getTextureCache()->removeAllTextures();
    FMODAudioEngine::getInstance()->releaseAllSounds();
    auto scene = WelcomeScene::createScene();
    Director::getInstance()->replaceScene(scene);
}

void Darkness::update(float delta)
{
//#ifndef _DEBUG
    updateAntiCheat(delta);
//#endif
}

void Darkness::setupLuaEngine()
{

}

#ifdef WIN32
inline wchar_t* strcasestr(wchar_t* haystack, const wchar_t* needle)
{
    do {
        wchar_t* h = haystack;
        const wchar_t* n = needle;
        while (towlower((wchar_t)*h) == towlower((wchar_t)*n) && *n) {
            h++;
            n++;
        }
        if (*n == 0) {
            return (wchar_t*)haystack;
        }
    } while (*haystack++);
    return 0;
}
#endif

std::atomic<bool> exit_thread_flag{ false };
std::atomic<bool> exit_thread_hook{ false };

void Darkness::initAntiCheat()
{
    if (isAntiCheatReady)
        return;

    //GetWriteWatch(
    //    WRITE_WATCH_FLAG_RESET,
    //    ptr,
    //    4,
    //    pageAddresses
    //);

    //  Check for sus externally loaded modules ///////////////////////////////
#ifdef WIN32
    modCheck = std::thread([&]()
        {
            bool isRunning = true;

            DWORD aProcesses[1024];
            DWORD cProcesses;
            HMODULE hMods[1024];
            HANDLE hProcess = NULL;
            DWORD cbNeeded;
            unsigned int mpfi;
            std::vector<std::wstring> disallowedMods;

            while (isRunning)
            {
                if (hProcess == NULL)
                {
                    hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, GetCurrentProcessId());

                    //str enc = base64::to_base64(zlibString::compress_string("cheatengine\ncheat_engine\ncheat engine\nspeed hack\nspeedhack\nhook"));

                    std::wstring s = Strings::widen(zlibString::decompress_string(Strings::from_base64("eNpLzkhNLEnNS8/MS+VKBrHjkTkKUE5xQWpqikJGYnI2hAlmZeTnZwMA8kMXRA")));

                    Strings::wreplace(s, L"\r", L"");
                    Strings::wsplit(s, L"\n", disallowedMods);
                }

                {
                    if (!EnumProcesses(aProcesses, sizeof(aProcesses), &cbNeeded))
                    {
                        return 1;
                    }
                    cProcesses = cbNeeded / sizeof(DWORD);
                    for (mpfi = 0; mpfi < cProcesses; mpfi++)
                    {
                        if (aProcesses[mpfi] != 0)
                        {
                            HANDLE Handle = OpenProcess(
                                PROCESS_QUERY_INFORMATION | PROCESS_VM_READ,
                                FALSE,
                                aProcesses[mpfi]
                            );
                            if (Handle)
                            {
                                wchar_t szProcName[MAX_PATH];
                                if (GetModuleBaseNameW(Handle, 0, szProcName, MAX_PATH))
                                {
                                    for (auto& i : disallowedMods)
                                    {
                                        if (strcasestr(szProcName, i.c_str()) != 0)
                                        {
                                            if (i == L"hook")
                                                continue;

                                            exit_thread_flag = true;
                                            isRunning = false;
                                        }
                                    }
                                }
                                CloseHandle(Handle);
                            }
                        }
                    }
                }

                if (EnumProcessModules(hProcess, hMods, sizeof(hMods), &cbNeeded))
                {
                    for (mpfi = 0; mpfi < (cbNeeded / sizeof(HMODULE)); mpfi++)
                    {
                        wchar_t szModName[MAX_PATH];

                        if (GetModuleBaseNameW(hProcess, hMods[mpfi], szModName,
                            sizeof(szModName) / sizeof(wchar_t)))
                        {
                            bool isOkay = false;
                            for (auto& i : disallowedMods)
                            {
                                if (strcasestr(szModName, i.c_str()) != 0)
                                {
                                    if (i == L"hook")
                                    {
                                        if (!exit_thread_hook)
                                        {
                                            //MessageBoxA(glfwGetWin32Window(gameWindow.window),
                                            //    "external overlay programs are allowed but it's adviced that they are not used with this game.",
                                            //    "anti-cheat agent",
                                            //    MB_ICONASTERISK | MB_OK);
                                            exit_thread_hook = true;
                                        }
                                        continue;
                                    }
                                    else
                                    {
                                        exit_thread_flag = true;
                                        isRunning = false;
                                    }
                                }
                            }
                        }
                    }
                }

                std::this_thread::sleep_for(std::chrono::milliseconds(2000));
            }
        });
#endif

    isAntiCheatReady = true;
}

void Darkness::updateAntiCheat(float delta)
{
    if (exit_thread_flag)
    {
        MessageBoxA(glfwGetWin32Window(gameWindow.window),
            "third-party software detected, please close cheat engine or any of the like and start the game again.",
            "anti-cheat engine",
            0x00000010L | 0x00004000L | 0x00000000L);
        while (true) {}
        std::exit(0);
    }

    if (!isAntiCheatReady)
        return;

    //  Game clock speed anti-cheat check  ////////////////////////////////////

    if (timeSinceStart == 0ULL || elapsedGameTime > 10.0F)
    {
        timeSinceStart = currentTime = time(0);
        elapsedGameTime = 0.0F;
    }
    elapsedGameTime += delta;

    f32 timeDiff = abs(elapsedGameTime - float(currentTime - timeSinceStart));

#ifndef _DEBUG
    if (timeDiff > 3)
    {
#ifdef WIN32
        MessageBoxA(glfwGetWin32Window(gameWindow.window),
            "game clock is not steady, possibly a third-party program is modifying the clock speed.",
            "anti-cheat engine",
            0x00000010L | 0x00004000L | 0x00000000L);
#endif
        while (true) {}
    }
#endif

    currentTime = time(0);

    ///////////////////////////////////////////////////////////////////////////
}

void Darkness::MessageBoxWin32(std::string caption, std::string text)
{
#ifdef WIN32
    MessageBoxA(glfwGetWin32Window(gameWindow.window), text.c_str(), caption.c_str(), 0x00000010L | 0x00004000L | 0x00000000L);
#endif
}
