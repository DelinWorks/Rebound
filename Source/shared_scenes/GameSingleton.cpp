﻿#include "GameSingleton.h"
#include "WelcomeScene.h"

#ifdef WIN32
#include <Psapi.h>
#include <string.h>
#include <stdlib.h>
#include <windows.h>
#include <tlhelp32.h>
#endif
#include <string.h>

static Rebound* _rebound = nullptr;

Rebound::Rebound()
{
}

Rebound::~Rebound()
{
}

Rebound* Rebound::getInstance()
{
    if (_rebound == nullptr)
    {
        _rebound = new Rebound();
        _rebound->init();
    }
    return _rebound;
}

void Rebound::init()
{
    Director::getInstance()->getScheduler()->scheduleUpdate(this, 1, false);

    console.isHeadless = false;

#ifdef WIN32
    res_path = getCurrentResourcesDirectoryW();
#endif

#ifdef _DEBUG
    res_path = L"C:/Users/turky/Documents/GitHub/ReboundAmongUs/Resources/";
#endif

    server_path = "https://delingames.xyz/";

    {
        gameWindow.isFullscreen = false;
        gameWindow.isScreenSizeDirty = false;
        gameWindow.isCursorLockedToWindow = true;
        gameWindow.isAllowedToLeave = false;
        gameWindow.lastKnownWindowRect = cocos2d::Rect(0, 0, 1280, 720);
        gameWindow.windowPolicy = ResolutionPolicy::FIXED_WIDTH;
        gameWindow.currentWindowCursor = 0;
        gameWindow.focusState = false;
        gameWindow.guiScale = 1;
    }
}

void Rebound::setupController()
{
    ax::Controller::stopDiscoveryController();
    ax::Controller::startDiscoveryController();
}

ax::Controller::KeyStatus Rebound::getKeyState(ax::Controller::Key key)
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

void Rebound::setCursorNormal()
{
    if (!cursor) {
        Image* img = new Image();
        img->initWithImageFile("cursor.png");
        GLFWimage* icon = new GLFWimage();
        icon->width = img->getWidth();
        icon->height = img->getHeight();
        icon->pixels = img->getData();
        cursor = glfwCreateCursor(icon, 1, 1);
    }
    glfwSetCursor(gameWindow.window, cursor);
}

void Rebound::setCursorHand()
{
    if (!hand) {
        Image* img = new Image();
        img->initWithImageFile("cursor_selected.png");
        GLFWimage* icon = new GLFWimage();
        icon->width = img->getWidth();
        icon->height = img->getHeight();
        icon->pixels = img->getData();
        hand = glfwCreateCursor(icon, 5, 1);
    }
    glfwSetCursor(gameWindow.window, hand);
}

void Rebound::setCursorHold()
{
    if (!hold) {
        Image* img = new Image();
        img->initWithImageFile("cursor_held.png");
        GLFWimage* icon = new GLFWimage();
        icon->width = img->getWidth();
        icon->height = img->getHeight();
        icon->pixels = img->getData();
        hold = glfwCreateCursor(icon, 5, 1);
    }
    glfwSetCursor(gameWindow.window, hold);
}

void Rebound::destroyInstance()
{
    glfwHideWindow(Rebound::getInstance()->gameWindow.window);
    Director::getInstance()->getScheduler()->unscheduleUpdate(_rebound);
    _rebound->gameWindow.isAllowedToLeave = true;
    Director::getInstance()->end();
}

void Rebound::restartInstance()
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

void Rebound::update(float delta)
{
//#ifndef _DEBUG
    updateAntiCheat(delta);
//#endif
}

void Rebound::setupLuaEngine()
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

void Rebound::initAntiCheat()
{
    if (_isAntiCheatReady)
        return;

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

    _isAntiCheatReady = true;
}

void Rebound::updateAntiCheat(float delta)
{
#ifdef WIN32
    if (exit_thread_flag)
    {
        //MessageBoxA(glfwGetWin32Window(gameWindow.window),
        //    "third-party software detected, please close cheat engine or any of the like and start the game again.",
        //    "anti-cheat engine",
        //    0x00000010L | 0x00004000L | 0x00000000L);
        while (true) {}
        std::exit(0);
    }
#endif

    if (!_isAntiCheatReady)
        return;

    //  Game clock speed anti-cheat check  ////////////////////////////////////

    if (_timeSinceStart == 0ULL || _elapsedGameTime > 100.0F ||
        Director::getInstance()->getRunningScene()->_hashOfName == 14169343825431587970 /* scene.MapEditor */)
    {
        _timeSinceStart = _currentTime = time(0);
        _elapsedGameTime = 0.0F;
    }
    _elapsedGameTime += delta;

    F32 timeDiff = abs(_elapsedGameTime - float(_currentTime - _timeSinceStart));

//#ifndef WWDWD
//    if (timeDiff > 1)
//    {
//        _accumulatedKickTries++;
//        printf("_accumilatedKickTries: %d", _accumulatedKickTries);
//        if (_accumulatedKickTries > 2) {
//#ifdef WIN32
//            MessageBoxA(glfwGetWin32Window(gameWindow.window),
//                "game clock is not steady, a third-party program might be modifying the delta time of the game.",
//                "anti-cheat engine",
//                0x00000010L | 0x00004000L | 0x00000000L);
//            exit(0);
//#endif
//        }
//    } else
//        _accumulatedKickTries = 0;
//#endif

    _currentTime = time(0);

    ///////////////////////////////////////////////////////////////////////////
}

void Rebound::MessageBoxWin32(std::string caption, std::string text)
{
//#ifdef WIN32
//    MessageBoxA(glfwGetWin32Window(gameWindow.window), text.c_str(), caption.c_str(), 0x00000010L | 0x00004000L | 0x00000000L);
//#endif
}
