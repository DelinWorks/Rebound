/****************************************************************************
 Copyright (c) 2017-2018 Xiamen Yaji Software Co., Ltd.
 
 http://www.cocos2d-x.org
 
 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:
 
 The above copyright notice and this permission notice shall be included in
 all copies or substantial portions of the Software.
 
 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.
 ****************************************************************************/

#include "EmptyScene.h"
#include "AppDelegate.h"
#include "WelcomeScene.h"
#include "fmod/include_fmod.h"
#include "fmod/FMODAudioEngine.h"
#include "string_manipulation_lib/stringFunctions.hpp"
#include "string_manipulation_lib/zlibString.hpp"
#include "string_manipulation_lib/base64.hpp"
#include <chrono>

#define MINI_CASE_SENSITIVE
#include "Helper/INI/ini.h"
#include <iostream>

#include "Helper/Logging.hpp"
#include "Helper/PlatDefines.h"
#include "shared_scenes/ProtectedTypes.hpp"

//using namespace tINI::unicode;

// #define USE_AUDIO_ENGINE 1

//#if USE_AUDIO_ENGINE
//#include "audio/include/AudioEngine.h"
//using namespace cocos2d::experimental;
//#endif

USING_NS_CC;

AppDelegate::AppDelegate()
{
}

AppDelegate::~AppDelegate() 
{
#if USE_AUDIO_ENGINE
    AudioEngine::end();
#endif
}

// if you want a different context, modify the value of glContextAttrs
// it will affect all platforms
void AppDelegate::initGLContextAttrs()
{
    // set OpenGL context attributes: red,green,blue,alpha,depth,stencil,multisamplesCount
    GLContextAttrs glContextAttrs = {8, 8, 8, 8, 24, 8, 0};

    GLView::setGLContextAttrs(glContextAttrs);
}

// if you want to use the package manager to install more packages,  
// don't modify or remove this function
static i32 register_all_packages()
{
    return 0; //flag for packages manager
}

#ifdef WIN32
static void window_size_callback(GLFWwindow* window, i32 width, i32 height)
{
    width = MAX(640, width);
    height = MAX(360, height);

    Darkness::getInstance()->gameWindow.windowSize = Size(width, height);
    auto director = Director::getInstance();
    auto glview = director->getOpenGLView();
    if (!glview) return;
    f32 aspect = 16.0f / 9.0f;
    glview->setFrameSize(width, height);
    glview->setDesignResolutionSize(designResolutionSize.width, designResolutionSize.height, Darkness::getInstance()->gameWindow.windowPolicy);
    //glfwSetWindowTitle(window, std::string("Darkness Among Us (" + std::to_string(width) + "x" + std::to_string(height) + ")").c_str());
    Darkness::getInstance()->gameWindow.isScreenSizeDirty = true;
    auto windowHandle = GetForegroundWindow();
    long Style = GetWindowLong(windowHandle, GWL_STYLE);
    //Style &= ~WS_MAXIMIZEBOX;
    SetWindowLong(windowHandle, GWL_STYLE, Style);
    glfwSwapInterval(0);
}

static void window_maximize_callback(GLFWwindow* window, i32 maximized)
{
    //if (maximized)
    //{
    //    auto director = Director::getInstance();
    //    auto glview = director->getOpenGLView();
    //    f32 aspect = 16.0f / 9.0f;
    //    i32 winSizeX, winSizeY;
    //    glfwGetWindowSize(window, &winSizeX, &winSizeY);
    //    //glfwSetWindowSize(window, winSizeX, winSizeX / aspect);
    //    glview->setFrameSize(winSizeX, winSizeX / aspect);
    //}
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

#if WIN32
bool IsCheatEngineProcessRunning()
{
    bool exists = false;
    PROCESSENTRY32 entry;
    entry.dwSize = sizeof(PROCESSENTRY32);

    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);

    if (Process32First(snapshot, &entry)) {
        while (Process32Next(snapshot, &entry)) {
            if (!strcasestr(entry.szExeFile, L"Cheat Engine") && 
                strcasestr(entry.szExeFile, L"CheatEngine")) {
                exists = true; break;
            }
        }
    }

    CloseHandle(snapshot); return exists;
}
#endif

static void window_focus_callback(GLFWwindow* window, i32 focused)
{
#if WIN32
    if (Darkness::getInstance()->isAntiCheatReady && IsCheatEngineProcessRunning()) {
        MessageBoxA(glfwGetWin32Window(Darkness::getInstance()->gameWindow.window),
            "third-party software detected, please close cheat engine or any of the like and start the game again.",
            "anti-cheat engine",
            0x00000010L | 0x00004000L | 0x00000000L);
        while (true) {}
        std::exit(0);
    }
#endif WIN32

    if (focused)
    {
        Darkness::getInstance()->setupController();
        if (Darkness::getInstance()->gameWindow.isFullscreen) {
                GLFWmonitor* monitor = glfwGetPrimaryMonitor();
                const GLFWvidmode* mode = glfwGetVideoMode(monitor);
                i32 xpos, ypos;
                Size frameSize = Director::getInstance()->getOpenGLView()->getFrameSize();
                glfwGetWindowPos(Darkness::getInstance()->gameWindow.window, &xpos, &ypos);
                Director::getInstance()->getOpenGLView()->setFrameSize(mode->width, mode->height);
                glfwSetWindowSizeLimits(Darkness::getInstance()->gameWindow.window, 640, 360, mode->width, mode->height);
                glfwSetWindowPos(Darkness::getInstance()->gameWindow.window, 0, 0);
                glfwSetWindowSize(Darkness::getInstance()->gameWindow.window, mode->width, mode->height);
                GameUtils::GLFW_ClipCursor(true);
        }
        FMODAudioEngine::getInstance()->resumeAllSounds();
    } else {
        FMODAudioEngine::getInstance()->pauseAllSounds();

        //SetWindowPos(glfwGetWin32Window(Darkness::getInstance()->window), HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
    }

    Darkness::getInstance()->gameWindow.focusState = focused;
}

static void window_close_callback(GLFWwindow* window)
{
    //if (!Darkness::getInstance()->gameWindow.isAllowedToLeave) {
    //    glfwSetWindowShouldClose(window, GLFW_FALSE);
    //    return;
    //}
}
#endif

bool AppDelegate::applicationDidFinishLaunching() {
    //FMODAudioEngine::getInstance();
    //FileUtils::getInstance()->setPopupNotify(true);

//#ifdef WIN32
    FileUtils::getInstance()->addSearchPath("resources"sv, true);
    FileUtils::getInstance()->addSearchPath("content"sv, true);
//#endif

    PROTECTED(f32) t;

    // Rank degree
    t = 1.423F;

    Darkness::getInstance()->setupController();

    Device::setKeepScreenOn(true);

#if AX_TARGET_PLATFORM == AX_PLATFORM_WIN32
    std::wstring ws(Darkness::getInstance()->console.args);
    std::string str(ws.begin(), ws.end());
#else
    std::string str = "";
#endif

    std::wstring filename = L"settings.ini";
    std::wstring filepath = L"settings.ini";
#if AX_TARGET_PLATFORM == AX_PLATFORM_WIN32
    filepath = getCurrentDirectoryW() + L"\\" + filename;
#endif
#if AX_TARGET_PLATFORM == AX_PLATFORM_ANDROID
    filepath = Strings::widen(FileUtils::getInstance()->getWritablePath() + Strings::narrow(filename));
#endif
    //if (!FileUtils::getInstance()->isFileExist(Strings::WC2MB(filepath))) {
        std::string commentStart = "this file contains default setings that the user may want to edit to fit their needs\nremoving this file will make it regenerate with defaults on next launch\n\n"
                     + std::string("#----- Server Mode Settings -------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------\n")
                     + std::string("server_name:                  Server name that others can view before joining the server\n")
                     + std::string("dns_check_address             It is adviced to be left as is, Address used to check if server is able to communicate with machine's network interface, 8.8.8.8 stands for google's dns which the devs think it's most reliable... or is it 0_0\n")
                     + std::string("admin_cli_login               The password used to login as admin if you're the server owner to perform admin commands and control the server\n")
                     + std::string("map_full_path                 Can either be 'ASK' to ask the admin on server launch or A full path for the map along with the file name, or a folder containing maps to choose at random from, or a map link prefixed with MAP:123456789 to load from game servers and play, or a .txt file containing lines of MAP:123456789 map objects to choose and load at random from, keep in mind that you can spicify any map you desire from the console or the admin console\n")
                     + std::string("connection_port               Port the players should spicify to get in, they won't get asked about the port if it's set to 4624\n")
                     + std::string("server_tick_speed             How many times in a second should the server do it's calculations, 30 is recommended for not so good network connections while keeping accurate calculation results, the more this value is the more the calculations will be fair and accurate but more packets are required to be sent and more internet bandwidth would be used by the clients and the server\n")
                     + std::string("#----- Client Settings ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------\n")
                     + std::string("login_cookie                  Please don't tincker with that\n")
                     + std::string("resolution                    The resolution OpenGL context is created with, it's ignored if fullScreen is set to true\n")
                     + std::string("fullScreen                    Occupies the entire screen when launching the game\n")
                     + std::string("useVsync                      Stands for Vertical Sync, you may enable this if you experience stuttering or horizontal lines, if it's false the game will draw as many frames as possible, if true it will query the main screen's refresh rate and adapt on it (eg. 60hz is 60fps, 144hz is 144fps and so on)\n")
                     + std::string("chatFontName                  Chat text font (can be a .ttf or .otf font file)\n")
                     + std::string("mMFontName                    Overall ui text font (can be a .ttf or .otf font file)\n")
                     + std::string("lastOpenFilenames_CSV         Last opened editor map files in a comma seperated values (CSVs)\n")
                     + std::string("tileTextureAliasSetting:      specifies whether to use GL_NEAREST (Nearest-neighbor) or GL_LINEAR (Bilinear filtering)\n")
                     + std::string("perFrameChunkBuild:           if true, builds one chunk per frame, while this can increase performance it may cause memory access violations (crashes) or visual bugs where chunk fragments get stuck in memory and cant be modified\n")
                     + std::string("streamAudioFromDisk:          if true (recommended), audio will be streamed from disk otherwise it will entirely get loaded to memory and then played\n");
                     //+ std::string("; \n")
    //    std::ofstream out(filepath);
    //    out << commentStart;
    //    out.close();
    //    mINI::INIFile file(filepath);
    //    mINI::INIStructure ini;
    //    file.read(ini);
    //    ini[L"Headless"][L"server_name"]             = L"Our custom server :D";
    //    ini[L"Headless"][L"dns_check_address"]       = L"8.8.8.8";
    //    ini[L"Headless"][L"admin_cli_login"]         = L"password";
    //    ini[L"Headless"][L"map_full_path"]           = L"ASK";
    //    ini[L"Headless"][L"connection_port"]         = L"4624";
    //    ini[L"Headless"][L"server_tick_speed"]       = L"30";
    //    ini[L"Credentials"][L"login_cookie"]         = Strings::MB2WC(base64::to_base64(zlibString::compress_string("SOME_BITCH_ASS_SERVER_COOKIE_FUCK_YOU")));
    //    ini[L"OGL"][L"resolution"]                   = L"1280x720";
    //    ini[L"OGL"][L"fullScreen"]                   = L"false";
    //    ini[L"OGL"][L"useVsync"]                     = L"true";
    //    ini[L"TtfPaths"][L"chatFontName"]            = L"fonts/arial.ttf";
    //    ini[L"TtfPaths"][L"mMFontName"]              = L"fonts/andyb.ttf";
    //    ini[L"Editor"][L"lastOpenFilenames_CSV"]     = L"NULL";
    //    ini[L"Renderer"][L"tileTextureAliasSetting"] = L"GL_NEAREST";
    //    ini[L"FMODAudio"][L"streamAudioFromDisk"]    = L"true";
    //    file.write(ini, true);
    //    Logging::log_to_file(Strings::WC2MB(filename) + " wasn't present in the root directory, file regenerated with default settings!", __FILENAME__, __LINE__, LOG_TYPE_INFORMATION);
    //}

    //GameUtils::Debug::StopWatch watch;

    //auto f = tINI::File(Strings::narrow(Darkness::getCurrentDirectoryW() + L"\\settings.ini"), commentStart);

    ////f.Get("Headless", "server_name")             ->Set("Our custom server :D", "Server name that others can view before joining the server");
    ////f.Get("Headless", "dns_check_address")       ->Set("8.8.8.8", "It is adviced to be left as is, Address used to check if server is able to\ncommunicate with machine's network interface,\n8.8.8.8 stands for google's dns which the devs think it's most reliable.");
    ////f.Get("Headless", "admin_cli_login")         ->Set("password", "The password used to login as admin if you're the server owner to perform admin commands and control the server");
    ////f.Get("Headless", "map_full_path")           ->Set("ASK", "Can either be 'ASK' to ask the admin on server launch\nor A full path for the map along with the file name,\nor a folder containing maps to choose at random from,\nor a map link prefixed with MAP:123456789 to load from game servers and play,\nor a .txt file containing lines of MAP:123456789 map objects to choose and load at random from,\nkeep in mind that you can spicify any map you desire from the console or the admin console");
    ////f.Get("Headless", "connection_port")         ->Set("4624", "Port the players should spicify to get in,\nthey won't get asked about the port if it's set to 4624");
    ////f.Get("Headless", "server_tick_speed")       ->Set("30", "How many times in a second should the server do it's calculations,\n30 is recommended for not so good network connections while keeping accurate calculation results,\nthe more this value is the more the calculations will be fair and accurate\nbut more packets are required to be sent and more internet bandwidth would be used by the clients and the server");
    ////f.Get("Credentials", "login_cookie")         ->Set(base64::to_base64(zlibString::compress_string("SOME_BITCH_ASS_SERVER_COOKIE_FUCK_YOU")));
    ////f.Get("OGL", "resolution")                   ->Set("1280x720");
    ////f.Get("OGL", "fullScreen")                   ->Set("false");
    ////f.Get("OGL", "useVsync")                     ->Set("true");
    ////f.Get("TtfPaths", "chatFontName")            ->Set("fonts/arial.ttf");
    ////f.Get("TtfPaths", "mMFontName")              ->Set("fonts/andyb.ttf");
    ////f.Get("Editor", "lastOpenFilenames_CSV")     ->Set("NULL");
    ////f.Get("Renderer", "tileTextureAliasSetting") ->Set("GL_NEAREST");
    ////f.Get("FMODAudio", "streamAudioFromDisk")    ->Set("true");
    //// 
    ////f.Get("OGL")->SetComment("OpenGL Renderer Settings");
    ////f.Get("Editor", "lastOpenFilenames_CSV")->SetComment("Last saved editor files in a csv format");


    //f.Load();

    //f.Save(true, true);

#ifdef AX_PLATFORM_PC
    if (str.compare("headless") == 0)
    {
        if (!Darkness::getInstance()->console.isConsole)
        {
            AllocConsole();
            freopen("CONIN$", "r", stdin);
            freopen("CONOUT$", "w", stdout);
            freopen("CONOUT$", "w", stderr);
        }
        Darkness::getInstance()->console.isHeadless = true;
        //lua_host_functions::isServer = true;
        std::cout << "\nheadless mode is on (server mode)\nsome settings.ini entries will be ignored in this mode\n\n";
    }

    wprintf(L"platform-specific ini path: %s\n\n", filepath.c_str());
#endif

    //std::ifstream ifs(filepath);
    //std::string content( (std::istreambuf_iterator<char>(ifs) ),
    //                     (std::istreambuf_iterator<char>()    ) );

    //CCLOG("settings.ini content\n%s", content.c_str());

    // initialize director


    auto director = Director::getInstance();
    auto glview = director->getOpenGLView();

#ifdef AX_USE_COMPAT_GL
    bool AX_USE_COMPAT_GL_CHK = false;
#else
    bool AX_USE_COMPAT_GL_CHK = TRUE;
#endif

    if(!glview && !Darkness::getInstance()->console.isHeadless) {
#ifdef AX_PLATFORM_PC
        GLViewImpl* window;
#ifdef _DEBUG
        const char* buildType = "DEBUG";
#else
        const char* buildType = "RELEASE";
#endif
        if (Darkness::getInstance()->gameWindow.isFullscreen) resolutionSize = { 1,1 };
        glview = window = GLViewImpl::createWithRect(FMT("Dark Dimensions (%s) (%s) BUILD: %s TIMESTAMP: %s %s", buildType, "D3D11", "1.0.0-alpha", __DATE__, __TIME__), cocos2d::Rect(0, 0, resolutionSize.width, resolutionSize.height), 1.0F, true);
        Darkness::getInstance()->gameWindow.windowSize = Size(resolutionSize.width, resolutionSize.height);
        //glfwSetInputMode(window->getWindow(), GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
        glfwSetWindowSizeCallback(window->getWindow(), window_size_callback);
        glfwSetWindowMaximizeCallback(window->getWindow(), window_maximize_callback);
        glfwSetWindowFocusCallback(window->getWindow(), window_focus_callback);
        glfwSetWindowCloseCallback(window->getWindow(), window_close_callback);
        glfwSetWindowAspectRatio(window->getWindow(), 16, 9);
        Image* img = new Image();
        img->initWithImageFile("cursor.png");
        GLFWimage* icon = new GLFWimage();
        icon->width = img->getWidth();
        icon->height = img->getHeight();
        icon->pixels = img->getData();
        GLFWcursor* cursor = glfwCreateCursor(icon, 1, 1);
        glfwSetCursor(window->getWindow(), cursor);
        Darkness::getInstance()->gameWindow.window = window->getWindow();
#else
        glview = GLViewImpl::create("Dark Dimensions");
#endif
        director->setOpenGLView(glview);

        director->setStatsDisplay(false);

        //glfwWindowHint(GLFW_SAMPLES, 4);
        //glEnable(GL_MULTISAMPLE);
    }

#ifdef AX_PLATFORM_MOBILE
    Darkness::getInstance()->gameWindow.windowSize = Size(glview->getFrameSize().x, glview->getFrameSize().y);
    director->setAnimationInterval(0);
#endif

#if (AX_TARGET_PLATFORM == AX_PLATFORM_WIN32) || (AX_TARGET_PLATFORM == AX_PLATFORM_MAC) || (AX_TARGET_PLATFORM == AX_PLATFORM_LINUX)
    if (!Darkness::getInstance()->console.isHeadless)
        director->setAnimationInterval(1.0f / glfwGetVideoMode(glfwGetPrimaryMonitor())->refreshRate);
    //glfwSwapInterval(1);
#endif

    //director->setAnimationInterval(1.0f / 60);
#ifdef _DEBUG
    director->setAnimationInterval(0);
#endif
    director->setAnimationInterval(0);
    // Set the design resolution
    if (!Darkness::getInstance()->console.isHeadless)
        glview->setDesignResolutionSize(designResolutionSize.width, designResolutionSize.height, Darkness::getInstance()->gameWindow.windowPolicy);

    if (Darkness::getInstance()->console.isHeadless)
        director->setAnimationInterval(1.0f / 30);

#ifdef WIN32
    window_focus_callback(Darkness::getInstance()->gameWindow.window, true);
#endif

    Darkness::getInstance()->initAntiCheat();

    //auto frameSize = glview->getFrameSize();
    ////// If the frame's height is larger than the height of medium size.
    //if (frameSize.height > mediumResolutionSize.height) {
    //    director->setContentScaleFactor(
    //        MIN(largeResolutionSize.height / designResolutionSize.height,
    //            largeResolutionSize.width / designResolutionSize.width));
    //}
    //// If the frame's height is larger than the height of small size.
    //else if (frameSize.height > smallResolutionSize.height) {
    //    director->setContentScaleFactor(
    //        MIN(mediumResolutionSize.height / designResolutionSize.height,
    //            mediumResolutionSize.width / designResolutionSize.width));
    //}
    //// If the frame's height is smaller than the height of medium size.
    //else {
    //    director->setContentScaleFactor(
    //        MIN(smallResolutionSize.height / designResolutionSize.height,
    //            smallResolutionSize.width / designResolutionSize.width));
    //}

    Director::getInstance()->setProjection(ax::Director::Projection::_2D);

    register_all_packages();

    if (Darkness::getInstance()->console.isHeadless)
    {
        auto scene = EmptyScene::createScene();
        director->runWithScene(scene);
    }
    else
    {
        auto scene = WelcomeScene::createScene();
        director->runWithScene(scene);
    }

    return true;
}

// This function will be called when the app is inactive. Note, when receiving a phone call it is invoked.
void AppDelegate::applicationDidEnterBackground() {
    //Director::getInstance()->stopAnimation();
    //FMODAudioEngine::getInstance()->pauseAllSounds();

#if USE_AUDIO_ENGINE
    AudioEngine::pauseAll();
#endif
}

// this function will be called when the app is active again
void AppDelegate::applicationWillEnterForeground() {
    //Director::getInstance()->startAnimation();
    //FMODAudioEngine::getInstance()->resumeAllSounds();
    //FMOD::ChannelGroup* group;
    //FMODAudioEngine::getInstance()->_system->getMasterChannelGroup(&group);
    //group->setPaused(false);

#if USE_AUDIO_ENGINE
    AudioEngine::resumeAll();
#endif
}

void AppDelegate::applicationScreenSizeChanged(i32 newWidth, i32 newHeight)
{
    //AXASSERT(false, "Dynamic GUI resolution not supported");
}
