#include <string>

#ifndef _GameUtils_H__
#define _GameUtils_H__

#include "cocos2d.h"
#include "ui/CocosGUI.h"
#include "Helper/Color.hpp"
#include "AppDelegate.h"
#include "fmod/FMODAudioEngine.h"
#include "Helper/Random.h"
#include "shared_scenes/GameSingleton.h"
#include "Helper/short_types.h"
#include "Helper/win32_error.h"
#include "Helper/PlatDefines.h"

#include "Helper/Math.h"

#define MATH_PI                     3.14159265358979323846f

#define CALL0(__selector__, ...) std::bind(&__selector__, this, ##__VA_ARGS__)
#define CALL1(__selector__, ...) \
    std::bind(&__selector__, this, std::placeholders::_1, ##__VA_ARGS__)
#define CALL2(__selector__, ...) \
    std::bind(&__selector__, this, std::placeholders::_1, std::placeholders::_2, ##__VA_ARGS__)
#define CALL3(__selector__, ...)                                                          \
    std::bind(&__selector__, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, \
              ##__VA_ARGS__)

#define REGISTER_SCENE(T) SET_SCENE_NAME(T); ATTACH_SCENE_INPUT_MANAGER(T);

#define SET_SCENE_NAME(T) this->setName(Strings::replace_const(typeid(T).name(), "class ", "scene."));
#define ATTACH_SCENE_INPUT_MANAGER(T) addComponent((new SceneInputManagerComponent()) \
->initKeyboard(CALL2(T::onKeyPressed), CALL2(T::onKeyReleased), CALL2(T::onKeyHold)) \
->initMouse(CALL1(T::onMouseMove), CALL1(T::onMouseUp), CALL1(T::onMouseDown), CALL1(T::onMouseScroll)) \
->initTouchScreenOneByOne(CALL2(T::onTouchBegan), CALL2(T::onTouchMoved), CALL2(T::onTouchEnded), CALL2(T::onTouchCancelled)) \
->attachSceneGraph(this) \
);

#define REBUILD_UI if (Darkness::getInstance()->gameWindow.isScreenSizeDirty) \
{ \
visibleSize = Director::getInstance()->getVisibleSize(); \
std::cout << "resize game window: Size(" << Darkness::getInstance()->gameWindow.windowSize.width << ", " << Darkness::getInstance()->gameWindow.windowSize.height << ")\n"; \
rebuildEntireUi(); \
Darkness::getInstance()->gameWindow.isScreenSizeDirty = false; \
}

#define KILL_PTR_VECT_PTR(V) for (auto i : *V) \
delete(i); \
delete V; \

#define KILL_PTR_MAP_PTR(V) for (auto i : *V) \
delete(i.second); \
delete V; \

#define SET_POSITION_HALF_SCREEN(node) node->setPosition(Vec2((visibleSize.width / 2), (visibleSize.height / 2)));
#define SET_POSITION_MINUS_HALF_SCREEN(node) node->setPosition(Vec2((visibleSize.width / -2), (visibleSize.height / -2)));

#define SET_UNIFORM(ps, name, value)  do { \
decltype(value) __v = value; \
auto __loc = (ps)->getUniformLocation(name); \
(ps)->setUniform(__loc, &__v, sizeof(__v)); \
} while(false) 

#define FMT StringUtils::format

#define DESTROY(n, t) n->addComponent(new DestroyNodeComponent(t));

#define LOAD_TEXTURE(pTex, tex, alias) do { \
auto texture = Director::getInstance()->getTextureCache()->addImage(tex); \
if (alias) \
texture->setAliasTexParameters(); \
else \
texture->setAntiAliasTexParameters(); \
pTex = texture; \
} while (0);

#define SET_TEXTURE(node, tex, alias) do { \
auto texture = Director::getInstance()->getTextureCache()->addImage(tex); \
if (alias) \
texture->setAliasTexParameters(); \
else \
texture->setAntiAliasTexParameters(); \
node->initWithTexture(texture); \
} while (0);


#define SET_TEXTURE_ASYNC(node, texture, alias) do { \
auto loadTexture = [&](Texture2D* tex) { \
node->setTexture(tex); \
if (alias) \
tex->setAliasTexParameters(); \
else \
tex->setAntiAliasTexParameters(); \
}; \
Director::getInstance()->getTextureCache()->addImageAsync(texture, loadTexture); \
} while (0);

#define SET_TEXTURE_ASYNC_WAIT(node, placeholder, texture, alias) do { \
auto texture1 = Director::getInstance()->getTextureCache()->addImage(placeholder); \
if (alias) \
texture1->setAliasTexParameters(); \
else \
texture1->setAntiAliasTexParameters(); \
node->setTexture(texture1); \
auto loadTextureWait = [&]() \
{ \
auto loadTexture = [&](Texture2D* tex) { \
node->setTexture(tex); \
if (alias) \
tex->setAliasTexParameters(); \
else \
tex->setAntiAliasTexParameters(); \
}; \
Director::getInstance()->getTextureCache()->addImageAsync(texture, loadTexture); \
}; \
node->runAction(Sequence::create(DelayTime::create(1), CallFunc::create([&]() { loadTextureWait(); }), nullptr)); \
} while (0);

#define EXPOSE_TEXTURE(tex, alias) auto exposedTex = Director::getInstance()->getTextureCache()->addImage(tex); \
if (alias) \
exposedTex->setAliasTexParameters(); \
else \
exposedTex->setAntiAliasTexParameters();

#define BENCHMARK_SECTION_BEGIN(name) const char* benchmark_bb7_name = name; auto benchmark_bb7_start = std::chrono::high_resolution_clock::now();
#define BENCHMARK_SECTION_END() auto benchmark_bb7_end = std::chrono::high_resolution_clock::now(); \
printf("benchmark %s took: %d millis, %d micros\n", benchmark_bb7_name, std::chrono::duration_cast<std::chrono::milliseconds>(benchmark_bb7_end - benchmark_bb7_start).count(), std::chrono::duration_cast<std::chrono::microseconds>(benchmark_bb7_end - benchmark_bb7_start).count());

USING_NS_CC;

// Some random ass enum
enum class Probability {
    IMPOSSIBLE    = 0,
    VERY_UNLIKELY = 1,
    UNLIKELY      = 2,
    EVEN_CHANCE   = 3,
    LIKELY        = 4,
    VERY_LIKELY   = 5,
    GUARANTEED    = 6,
};

namespace GameUtils
{
    namespace Debug
    {
        class StopWatch
        {
            std::chrono::high_resolution_clock::time_point begin;
            std::chrono::high_resolution_clock::time_point end;

        public:
            void start()
            {
                begin = std::chrono::high_resolution_clock::now();
            }

            void stop()
            {
                end = std::chrono::high_resolution_clock::now();
            }

            ll getElapsed() const
            {
                return std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count();
            }

            ll getElapsedMicro() const
            {
                return std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count();
            }
        };
    }

#ifdef WIN32
    namespace CursorsAndWindows
    {
        static void GLFW_ClipCursor(bool unset = false)
        {
            return;

            auto mWindow = glfwGetWin32Window(Darkness::getInstance()->gameWindow.window);

            RECT rect;
            GetClientRect(mWindow, &rect);

            POINT ul;
            ul.x = rect.left;
            ul.y = rect.top;

            POINT lr;
            lr.x = rect.right;
            lr.y = rect.bottom;

            MapWindowPoints(mWindow, nullptr, &ul, 1);
            MapWindowPoints(mWindow, nullptr, &lr, 1);

            rect.left = ul.x;
            rect.top = ul.y;

            rect.right = lr.x;
            rect.bottom = lr.y;

            ClipCursor(unset ? nullptr : (Darkness::getInstance()->gameWindow.isCursorLockedToWindow ? &rect : nullptr));
        }

        inline void GLFW_SetCursorNormal()
        {
            if (Darkness::getInstance()->gameWindow.currentWindowCursor == 0)
                return;

            Image* img = new Image();
            img->initWithImageFile("cursor.png"sv);
            GLFWimage* icon = new GLFWimage();
            icon->width = img->getWidth();
            icon->height = img->getHeight();
            icon->pixels = img->getData();
            GLFWcursor* cursor = glfwCreateCursor(icon, 0, 0);
            glfwSetCursor(Darkness::getInstance()->gameWindow.window, cursor);
            Darkness::getInstance()->gameWindow.currentWindowCursor = 0;
        }

        inline void GLFW_SetCursorSelected()
        {
            if (Darkness::getInstance()->gameWindow.currentWindowCursor == 1)
                return;

            Image* img = new Image();
            img->initWithImageFile("cursor_selected.png"sv);
            GLFWimage* icon = new GLFWimage();
            icon->width = img->getWidth();
            icon->height = img->getHeight();
            icon->pixels = img->getData();
            GLFWcursor* cursor = glfwCreateCursor(icon, 0, 0);
            glfwSetCursor(Darkness::getInstance()->gameWindow.window, cursor);
            Darkness::getInstance()->gameWindow.currentWindowCursor = 1;
        }

        inline void GLFW_SetBorder(HWND window, int on)
        {
            return;

            RECT rect;
            DWORD style;

            style = GetWindowLong(window, GWL_STYLE);

            if (on)
            {
                if (!GetWindowRect(window, &rect)) return;
                style &= ~WS_OVERLAPPEDWINDOW;
                style |= WS_POPUP;
                AdjustWindowRect(&rect, style, TRUE);
                SetWindowLong(window, GWL_STYLE, style);
            }
            else
            {
                if (!GetWindowRect(window, &rect)) return;

                style |= WS_CAPTION;
                style |= WS_OVERLAPPEDWINDOW;
                style &= ~WS_POPUP;

                AdjustWindowRect(&rect, style, TRUE);

                SetWindowLong(window, GWL_STYLE, style);
            }

            SetWindowPos(window, HWND_TOPMOST,
                rect.left, rect.top,
                rect.right - rect.left, rect.bottom - rect.top,
                SWP_NOMOVE | SWP_FRAMECHANGED);
        }
    }
#endif

    namespace Parser
    {
        inline Vec2 parseVector2D(std::string position)
        {
            try {
                std::string value = position;
                std::string::iterator end_pos = std::remove(value.begin(), value.end(), ' ');
                value.erase(end_pos, value.end());
                if (value.at(0) == '[')
                    value.erase(0, 1);
                if (value.at(value.length() - 1) == ']')
                    value.erase(value.length() - 1, value.length());
                std::string token, strX, strY;
                strX = value.substr(0, value.find(","));
                value.erase(0, value.substr(0, value.find(",")).length());
                if (value.length() != 0)
                    strY = value.substr(1, value.length());
                if (strX.length() == 0) strX = "0";
                if (strY.length() == 0) strY = strX;
                return Vec2(std::stof(strX), std::stof(strY));
            }
            catch (std::exception& e) {
                return { 0,0 };
            };
        }

        struct TIMESTAMP
        {
            i32 hour = 0;
            i32 min = 0;
            i32 sec = 0;

            explicit TIMESTAMP(i32 time)
            {
                hour = time / 3600;
                time = time % 3600;
                min = time / 60;
                time = time % 60;
                sec = time;
            }

            const char getSec() {
                if (this->sec < 10)
                    return *TEXT("0" + TO_TEXT(this->sec)).c_str();
                return *std::to_string(this->sec).c_str();
            }

            const char getMin() {
                if (this->min < 10)
                    return *TEXT("0" + TO_TEXT(this->min)).c_str();
                return *std::to_string(this->min).c_str();
            }

            const char getHour() {
                if (this->hour < 10)
                    return *TEXT("0" + TO_TEXT(this->hour)).c_str();
                return *std::to_string(this->hour).c_str();
            }
        };
    }

    namespace CocosExt
    {
        //template<typename T>
        //inline void remove(std::vector<T>& vec, size_t pos)
        //{
        //    std::vector<T>::iterator it = vec.begin();
        //    std::advance(it, pos);
        //    vec.erase(it);
        //}

        inline backend::ProgramState* createGPUProgram(std::string resources_frag_shader_path = "", std::string resources_vertex_shader_path = "")
        {
            auto fileUtiles = FileUtils::getInstance();
            auto fragmentFullPath = fileUtiles->fullPathForFilename("shaders/" + resources_frag_shader_path);
            auto fragSource = fileUtiles->getStringFromFile(fragmentFullPath);
            auto vertexFullPath = fileUtiles->fullPathForFilename("shaders/" + resources_vertex_shader_path);
            auto vertexSource = fileUtiles->getStringFromFile(vertexFullPath);
            auto program = backend::Device::getInstance()->newProgram(vertexSource.length() == 0 ? positionTextureColor_vert : vertexSource.c_str(), fragSource.length() == 0 ? positionTextureColor_frag : fragSource.c_str());
            auto theFuckingProgramState = new backend::ProgramState(program);
            return theFuckingProgramState;
        }

        inline void addSpriteFramesFromJson(const std::string_view texture_path, const std::string_view json_path)
        {
            Texture2D* tex = Director::getInstance()->getTextureCache()->addImage(texture_path);
            //SpriteFrameCache::getInstance()->addSpriteFramesWithFile(dat_path + ".dat", tex);

            auto cache = SpriteFrameCache::getInstance();

            if (FileUtils::getInstance()->fullPathForFilename(json_path).empty())
                return;

            Document d;
            d.Parse(FileUtils::getInstance()->getStringFromFile(json_path).c_str());

            const rapidjson::Value& meta = d["meta"];

            if (meta.IsObject() && meta.FindMember("alias")->value.GetBool())
                tex->setAliasTexParameters();

            const rapidjson::Value& frames = d["frames"];
            if (frames.IsArray())
            {
                for (auto const& it : frames.GetArray())
                {
                    if (it.IsObject())
                    {
                        auto const& _name = it.FindMember("filename")->name;
                        auto const& _value = it.FindMember("filename")->value;
                        auto const& _valuerot = it.FindMember("rotated")->value;
                        auto const& frame = it.FindMember("frame")->value;
                        auto const& sourceSize = it.FindMember("sourceSize")->value;
                        if (frame.IsObject() && sourceSize.IsObject()) {
                            const char* _filename = _value.GetString();
                            i32 _x = frame.FindMember("x")->value.GetInt();
                            i32 _y = frame.FindMember("y")->value.GetInt();
                            i32 _w = frame.FindMember("w")->value.GetInt();
                            i32 _h = frame.FindMember("h")->value.GetInt();
                            i32 _sw = sourceSize.FindMember("w")->value.GetInt();
                            i32 _sh = sourceSize.FindMember("h")->value.GetInt();
                            bool rotated = _valuerot.GetBool();

                            //std::cout << _name.GetString() << ": " << _filename << "\n"
                            //    "x: " << _x <<
                            //    ", y: " << _y <<
                            //    ", w: " << _w <<
                            //    ", h: " << _h << "\n"
                            //    "rotated: " << rotated << "\n\n";

                            auto spFrame = SpriteFrame::createWithTexture(tex, Rect(_x, _y, _w, _h), rotated, Vec2::ZERO, Size(_sw, _sh));
                            cache->addSpriteFrame(spFrame, _filename);
                        }
                    }
                }
            }
        }

        namespace Effects {
            class Vignette : public axmol::Sprite {
            public:
                cocos2d::backend::ProgramState* Program;
                cocos2d::Vec3 stored;

                static Vignette* create(cocos2d::Size visibleSize) {
                    Vignette* ret = new Vignette();
                    if (ret->init(visibleSize))
                    {
                        ret->autorelease();
                    }
                    else
                    {
                        AX_SAFE_DELETE(ret);
                    }
                    return ret;
                }

                Vignette* init(cocos2d::Size visibleSize) {
                    auto tex = Director::getInstance()->getTextureCache()->addImage("omf_2.png");
                    tex->setAliasTexParameters();
                    initWithTexture(tex);
                    setContentSize(visibleSize);
                    Program = createGPUProgram("shaders/vignette.fsh", "shaders/vignette.vsh");
                    setProgramState(Program);
                    return this;
                }

                void UpdateVisibleSize(cocos2d::Size visibleSize) {
                    setContentSize(visibleSize);
                }

                void SetColor(Vec3 c) {
                    stored = c;
                    SET_UNIFORM(Program, "_color", c);
                }

                void SetColorR(f32 r) {
                    Vec3 col = Vec3(r, stored.y, stored.z);
                    SET_UNIFORM(Program, "_color", col);
                    stored = col;
                }

                void SetColorG(f32 g) {
                    Vec3 col = Vec3(stored.x, g, stored.z);
                    SET_UNIFORM(Program, "_color", col);
                    stored = col;
                }

                void SetColorB(f32 b) {
                    Vec3 col = Vec3(stored.x, stored.y, b);
                    SET_UNIFORM(Program, "_color", col);
                    stored = col;
                }

                void SetDistance(f32 d) {
                    SET_UNIFORM(Program, "_distance", d);
                }

                void SetFalloff(f32 f) {
                    SET_UNIFORM(Program, "_falloff", f);
                }

                void Hide() {
                    setVisible(false);
                }

                void Show() {
                    setVisible(true);
                }
            };
        }

        inline Vec2 convertFromScreenToSpace(Vec2 locationInView, Size& visibleSize, Camera* cam, bool reverseY = false)
        {
            auto loc = Director::getInstance()->convertToGL(locationInView);
            return Vec2((((loc.x - visibleSize.width / 2) * cam->getZoom()) + cam->getPositionX()),
                (((loc.y - visibleSize.height / 2) * cam->getZoom()) * (reverseY ? 1 : -1)) + cam->getPositionY())
                .rotateByAngle(cam->getPosition(), -AX_DEGREES_TO_RADIANS(cam->getRotation()));
        }

        inline void setNodeIgnoreDesignScale(cocos2d::Node* node) {
            Size actualFrameSize = Director::getInstance()->getOpenGLView()->getFrameSize();
            Size actualWinSize = Director::getInstance()->getWinSizeInPixels();
            node->setScaleX(actualWinSize.width / actualFrameSize.width);
            node->setScaleY(actualWinSize.height / actualFrameSize.height);
        }

        inline Size getWinDiff() {
            Size actualFrameSize = Director::getInstance()->getOpenGLView()->getFrameSize();
            Size actualWinSize = Director::getInstance()->getWinSizeInPixels();
            return Size(actualFrameSize.width - actualWinSize.width, actualFrameSize.height - actualWinSize.height);
        }

        inline void setNodeScaleFHD(cocos2d::Node* node)
        {
            f32 res = Director::getInstance()->getOpenGLView()->getFrameSize().width +
                Director::getInstance()->getOpenGLView()->getFrameSize().height;

            //if (res > 3001) {
            //    node->setScaleX(node->getScaleX() + (1.0f / 3.0f));
            //    node->setScaleY(node->getScaleY() + (1.0f / 3.0f));
            //}

            AXLOG("KS AMK ASHTGHL");
        }

        // WARNING: Returns nullptr if nothing is found, please check if return pointer is nullptr to avoid problems
        // the 'list' argument should be left nullptr, it's only used for the recursive search
        inline std::vector<Component*> findComponentsByName(Node* parent, str name, bool containParent = true, std::vector<Component*>* list = nullptr)
        {
            std::vector<Component*>* nodes = list == nullptr ? new std::vector<Component*>() : list;

            if (parent->getChildrenCount() == 0 && containParent)
            {
                auto comp = parent->getComponent(name);
                if (comp)
                {
                    std::vector<Component*> vect;
                    vect.push_back(comp);
                    return vect;
                }
            }

            if (list == nullptr)
            {
                auto comp = parent->getComponent(name);
                if (comp)
                    nodes->push_back(comp);
            }

            bool hasFoundSomething = false;

            for (auto i : parent->getChildren())
            {
                if (i->getComponent(name) != nullptr)
                {
                    nodes->push_back(i->getComponent(name));
                    hasFoundSomething = true;
                }

                if (i->getChildrenCount() > 0)
                    findComponentsByName(i, name, containParent, nodes);
            }

            if (list == nullptr)
            {
                if (hasFoundSomething)
                    return *nodes;
            }

            return std::vector<Component*>();
        }

        inline std::vector<Node*> findNodesByTag(Node* parent, int tag, bool containParent = true, std::vector<Node*> list = std::vector<Node*>(), bool recursive = false)
        {
            std::vector<Node*> nodes;

            if (recursive)
                nodes = list;

            if (parent->getChildrenCount() == 0 && containParent)
            {
                if (parent->getTag() == tag)
                {
                    std::vector<Node*> vect;
                    vect.push_back(parent);
                    return vect;
                }
            }

            if (!recursive)
            {
                if (parent->getTag() == tag)
                    nodes.push_back(parent);
            }

            bool hasFoundSomething = false;

            for (auto i : parent->getChildren())
            {
                if (i->getTag() == tag)
                {
                    nodes.push_back(i);
                    hasFoundSomething = true;
                }

                if (i->getChildrenCount() > 0)
                    nodes = findNodesByTag(i, tag, containParent, nodes, true);
            }

            if (!recursive)
            {
                if (hasFoundSomething)
                    return nodes;
            }

            return nodes;
        }

        namespace CustomComponents
        {
#define RESIZE_UI_ELEMENTS auto list = GameUtils::CocosExt::findComponentsByName(this, "UiRescaleComponent"); \
for (auto i : list) dynamic_cast<GameUtils::CocosExt::CustomComponents::UiRescaleComponent*>(i)->windowSizeChange(visibleSize);

            class SceneInputManagerComponent : public Component {
            public:
                cocos2d::EventListenerKeyboard* _keyboardListener;
                cocos2d::EventListenerMouse* _mouseListener;
                cocos2d::EventListenerTouchOneByOne* _touchListener;

                std::function<void(EventKeyboard::KeyCode, Event*)> onKeyPressed;
                std::function<void(EventKeyboard::KeyCode, Event*)> onKeyReleased;
                std::function<void(EventKeyboard::KeyCode, Event*)> onKeyHold;
                Event* onKeyEvent;
                std::vector<int>* _pressedKeys;

                SceneInputManagerComponent() : _keyboardListener(nullptr), _mouseListener(nullptr), _touchListener(nullptr), onKeyEvent(nullptr), _pressedKeys(new std::vector<int>()) {
                    setName(__func__);
                    setEnabled(true);
                }

                ~SceneInputManagerComponent() {

                    Director::getInstance()->getScheduler()->unscheduleUpdate(this);

                    if (_keyboardListener)
                        Director::getInstance()->getEventDispatcher()->removeEventListener(_keyboardListener);
                    if (_mouseListener)
                        Director::getInstance()->getEventDispatcher()->removeEventListener(_mouseListener);
                    if (_touchListener)
                        Director::getInstance()->getEventDispatcher()->removeEventListener(_touchListener);

                    AX_SAFE_DELETE(_keyboardListener);
                    AX_SAFE_DELETE(_mouseListener);
                    AX_SAFE_DELETE(_touchListener);
                    AX_SAFE_DELETE(_pressedKeys);
                }

                void onAdd() {
                    AXLOG("Input manager initialized for %s", str(_owner->getName()).c_str());
                    Director::getInstance()->getScheduler()->scheduleUpdate(this, 0, false);
                }

                void update(f32 dt) {
                    for (auto& keyCode : *_pressedKeys)
                    {
                        onKeyHold((EventKeyboard::KeyCode)keyCode, onKeyEvent);
                    }
                }

                SceneInputManagerComponent* initKeyboard(
                    std::function<void(EventKeyboard::KeyCode, Event*)> _onKeyPressed,
                    std::function<void(EventKeyboard::KeyCode, Event*)> _onKeyReleased,
                    std::function<void(EventKeyboard::KeyCode, Event*)> _onKeyHold
                ) {

#if AX_TARGET_PLATFORM == AX_PLATFORM_WIN32
                    _keyboardListener = EventListenerKeyboard::create();

                    onKeyPressed = _onKeyPressed;
                    onKeyReleased = _onKeyReleased;
                    onKeyHold = _onKeyHold;

                    auto _onKeyHoldCheck = [&](EventKeyboard::KeyCode keyCode, Event* event) {
                        do {
                            for (auto& i : *_pressedKeys)
                                if (i == (i32)keyCode)
                                    return;
                            _pressedKeys->push_back((i32)keyCode);
                        } while (false);

                        if ((std::find(_pressedKeys->begin(), _pressedKeys->end(), (int)EventKeyboard::KeyCode::KEY_RIGHT_ALT) != _pressedKeys->end()) && keyCode == EventKeyboard::KeyCode::KEY_ENTER)
                        {
                            if (!Darkness::getInstance()->gameWindow.isFullscreen)
                            {
                                Darkness::getInstance()->gameWindow.isFullscreen = true;
                                GameUtils::CursorsAndWindows::GLFW_SetBorder(glfwGetWin32Window(Darkness::getInstance()->gameWindow.window), 1);
                                GLFWmonitor* monitor = glfwGetPrimaryMonitor();
                                const GLFWvidmode* mode = glfwGetVideoMode(monitor);
                                i32 xpos, ypos;
                                Size frameSize = Director::getInstance()->getOpenGLView()->getFrameSize();
                                glfwGetWindowPos(Darkness::getInstance()->gameWindow.window, &xpos, &ypos);
                                Darkness::getInstance()->gameWindow.lastKnownWindowRect = Rect(xpos, ypos, frameSize.width, frameSize.width / (f32)(16.0 / 9.0));
                                Director::getInstance()->getOpenGLView()->setFrameSize(mode->width, mode->height);
                                glfwSetWindowSizeLimits(Darkness::getInstance()->gameWindow.window, 640, 360, mode->width, mode->height);
                                glfwSetWindowPos(Darkness::getInstance()->gameWindow.window, 0, 0);
                                glfwSetWindowSize(Darkness::getInstance()->gameWindow.window, mode->width, mode->height);
                                GameUtils::CursorsAndWindows::GLFW_ClipCursor(true);
                                return;
                            }
                            else if (Darkness::getInstance()->gameWindow.isFullscreen) {

                                Darkness::getInstance()->gameWindow.isFullscreen = false;
                                GameUtils::CursorsAndWindows::GLFW_SetBorder(glfwGetWin32Window(Darkness::getInstance()->gameWindow.window), 0);
                                GLFWmonitor* monitor = glfwGetPrimaryMonitor();
                                const GLFWvidmode* mode = glfwGetVideoMode(monitor);
                                auto rect = Darkness::getInstance()->gameWindow.lastKnownWindowRect;
                                glfwSetWindowMonitor(Darkness::getInstance()->gameWindow.window, nullptr, rect.origin.x, rect.origin.y, rect.size.width, rect.size.height, mode->refreshRate);
                            }
                        }

                        if (!Darkness::getInstance()->gameWindow.isFullscreen)
                            GameUtils::CursorsAndWindows::GLFW_ClipCursor(false);

                        if (keyCode == EventKeyboard::KeyCode::KEY_ESCAPE)
                            Darkness::destroyInstance();

                        if (keyCode == EventKeyboard::KeyCode::KEY_F2)
                            Darkness::restartInstance();

                        if (keyCode == EventKeyboard::KeyCode::KEY_F5)
                            FMODAudioEngine::destroyInstance();

                        onKeyEvent = event;
                        onKeyPressed(keyCode, event);
                    };

                    auto _onKeyHoldRelease = [&](EventKeyboard::KeyCode keyCode, Event* event) {
                        do {
                            for (auto& i : *_pressedKeys)
                                if (i == (i32)keyCode)
                                    _pressedKeys->erase(std::remove(_pressedKeys->begin(), _pressedKeys->end(), (i32)keyCode), _pressedKeys->end());
                        } while (false);

                        onKeyReleased(keyCode, event);
                    };

                    _keyboardListener->onKeyPressed = _onKeyHoldCheck;
                    _keyboardListener->onKeyReleased = _onKeyHoldRelease;
#endif

                    return this;
                }

                SceneInputManagerComponent* initMouse(
                    std::function<void(EventMouse* event)> _onMouseMove,
                    std::function<void(EventMouse* event)> _onMouseUp,
                    std::function<void(EventMouse* event)> _onMouseDown,
                    std::function<void(EventMouse* event)> _onMouseScroll
                ) {
#if AX_TARGET_PLATFORM == AX_PLATFORM_WIN32
                    _mouseListener = EventListenerMouse::create();
                    _mouseListener->onMouseMove = _onMouseMove;
                    _mouseListener->onMouseUp = _onMouseUp;
                    _mouseListener->onMouseDown = _onMouseDown;
                    _mouseListener->onMouseScroll = _onMouseScroll;
#endif

                    return this;
                }

                SceneInputManagerComponent* initTouchScreenOneByOne(
                    std::function<bool(Touch*, Event*)> _onTouchesBegan,
                    std::function<void(Touch*, Event*)> _onTouchesMoved,
                    std::function<void(Touch*, Event*)> _onTouchesEnded,
                    std::function<void(Touch*, Event*)> _onTouchesCancelled
                ) {
#ifdef AX_PLATFORM_MOBILE
                    _touchListener = EventListenerTouchOneByOne::create();
                    _touchListener->onTouchBegan     = _onTouchesBegan;
                    _touchListener->onTouchMoved     = _onTouchesMoved;
                    _touchListener->onTouchEnded     = _onTouchesEnded;
                    _touchListener->onTouchCancelled = _onTouchesCancelled;
#endif
                    return this;
                }

                SceneInputManagerComponent* attachSceneGraph(Node* owner) {

#if AX_TARGET_PLATFORM == AX_PLATFORM_WIN32
                    Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(_keyboardListener, owner);
                    Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(_mouseListener, owner);
#endif

#ifdef AX_PLATFORM_MOBILE
                    Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(_touchListener, owner);
#endif

                    return this;
                }
            };

            //class SceneUiRebuilderComponent : public Component {
            //public:
            //    std::function<void()> rebuildEntireUi;

            //    SceneUiRebuilderComponent(std::function<void()> _rebuildEntireUi) {
            //        setName(__func__);
            //        setEnabled(true);
            //        rebuildEntireUi = _rebuildEntireUi;
            //    }

            //    ~SceneUiRebuilderComponent() {
            //        Director::getInstance()->getScheduler()->unscheduleUpdate(this);
            //    }

            //    void onAdd() {
            //        CCLOG("Ui builder initialized for scene.%s", str(_owner->getName()).c_str());
            //        Director::getInstance()->getScheduler()->scheduleUpdate(this, 0, false);
            //    }

            //    void update(f32 dt) {

            //    }
            //};

            enum BorderLayout {
                TOP = 0,
                TOP_RIGHT = 1,
                RIGHT = 2,
                BOTTOM_RIGHT = 3,
                BOTTOM = 4,
                BOTTOM_LEFT = 5,
                LEFT = 6,
                TOP_LEFT = 7,
                CENTER = 8,
            };

            class UiRescaleComponent : public Component {
            public:
                bool setLayerColor = false;
                bool ignore = false;
                bool resizeHints = false;
                bool fitting = false;
                Size fittingSize;
                Rect resizeHintsRect;
                Size visibleSize;
                bool recreateLayer = false;
                bool isUiElemDirty = false;

                UiRescaleComponent(Size _visibleSize) {
                    autorelease();
                    setName(__func__);
                    setEnabled(true);
                    resizeHintsRect = Rect(2, 0, 2, 0);
                    visibleSize = _visibleSize;
                }

                UiRescaleComponent() {
                    setName(__func__);
                    setEnabled(true);
                    resizeHintsRect = Rect(2, 0, 2, 0);
                }

                void onAdd() {
                    if (isUiElemDirty) {
                        windowSizeChange(visibleSize);
                        isUiElemDirty = false;
                    }
                }

                void onRemove() {

                }

                // enableDesignScaleIgnoring and setVisibleSizeHints will be ignored if this is enabled
                // owner will be casted to LayerColor* object, so be careful using this function
                UiRescaleComponent* enableLayerResizing() {
                    setLayerColor = true;
                    isUiElemDirty = true;
                    return this;
                }

                UiRescaleComponent* enableDesignScaleIgnoring() {
                    ignore = true;
                    isUiElemDirty = true;
                    return this;
                }

                UiRescaleComponent* setVisibleSizeHints(f32 widthDiv = 2, f32 widthOffset = 0, f32 heightDiv = 2, f32 heightOffset = 0) {
                    resizeHints = true;
                    resizeHintsRect = Rect(widthDiv, heightDiv, widthOffset, heightOffset);
                    isUiElemDirty = true;
                    return this;
                }

                UiRescaleComponent* setBorderLayout(BorderLayout border = BorderLayout::CENTER) {
                    switch (border) {
                    case BorderLayout::TOP: {
                        setVisibleSizeHints(0);
                        break;
                    }
                    case BorderLayout::TOP_RIGHT: {
                        setVisibleSizeHints();
                        break;
                    }
                    case BorderLayout::RIGHT: {
                        setVisibleSizeHints(2, 0, 0, 0);
                        break;
                    }
                    case BorderLayout::BOTTOM_RIGHT: {
                        setVisibleSizeHints(2, 0, -2, 0);
                        break;
                    }
                    case BorderLayout::BOTTOM: {
                        setVisibleSizeHints(0, 0, -2, 0);
                        break;
                    }
                    case BorderLayout::BOTTOM_LEFT: {
                        setVisibleSizeHints(-2, 0, -2, 0);
                        break;
                    }
                    case BorderLayout::LEFT: {
                        setVisibleSizeHints(-2, 0, 0, 0);
                        break;
                    }
                    case BorderLayout::TOP_LEFT: {
                        setVisibleSizeHints(-2, 0, 2, 0);
                        break;
                    }
                    default: {
                        setVisibleSizeHints(0, 1, 0, 0);
                    }
                    }
                    return this;
                }

                UiRescaleComponent* enableSizeFitting(Size _sizeInPixels) {
                    fitting = true;
                    fittingSize = _sizeInPixels;
                    isUiElemDirty = true;
                    return this;
                }

                void windowSizeChange(Size newVisibleSize) {

                    auto repositionNode = [&](Node* target) {
                        auto newPos = Vec2(resizeHintsRect.origin.x == 0 ? 0 : newVisibleSize.width / resizeHintsRect.origin.x + resizeHintsRect.size.width,
                            resizeHintsRect.origin.y == 0 ? 0 : newVisibleSize.height / resizeHintsRect.origin.y + resizeHintsRect.size.height);
                        auto action = MoveTo::create(.5, Vec2(round(newPos.x), round(newPos.y)));
                        //auto ease = EaseBounceOut::create(action);
                        target->stopAllActions();
                        target->runAction(action);
                    };

                    if (setLayerColor)
                    {
                        //auto parent = layer->getParent();
                        //auto order = layer->getLocalZOrder();
                        //layer->removeFromParentAndCleanup(true);
                        //layer = LayerColor::create(layerColor);
                        //parent->addChild(layer, order);
                        ((LayerColor*)_owner)->changeWidthAndHeight(newVisibleSize.width, newVisibleSize.height);
                        if (resizeHints) repositionNode(_owner);

                        return;
                    }
                    if (ignore && !fitting)
                    {
                        setNodeIgnoreDesignScale(_owner);
                        setNodeScaleFHD(_owner);
                        //_owner->setScale(_owner->getScale() * 1);
                    }
                    //if (fitting && !ignore)
                    //{
                    //    auto winsize = Darkness::getInstance()->windowSize;

                    //    setNodeIgnoreDesignScale(_owner);

                    //    float finalScale = 0;

                    //    //if (winsize.width < fittingSize.width && winsize.width <= winsize.height)
                    //    //    finalScale = winsize.width / (fittingSize.width > fittingSize.height ? fittingSize.width : fittingSize.height);

                    //    if (winsize.height < fittingSize.height && winsize.width > winsize.height)
                    //        finalScale = winsize.height / (fittingSize.width > fittingSize.height ? fittingSize.width : fittingSize.height);
                    //    
                    //    _owner->setScale(_owner->getScale() - finalScale);

                    //    if (winsize.width > fittingSize.width && winsize.height > fittingSize.height)
                    //        setNodeIgnoreDesignScale(_owner);
                    //}
                    if (resizeHints) repositionNode(_owner);
                }
            };

            class LerpPropertyActionComponent : public Component {
            public:
                Node* owner;
                Node* actionNode;
                Sequence* action;
                bool isBy = false;
                float* ref_float;
                int* ref_int;
                uint8_t* ref_uint8;
                f32 start, end, finish;
                i32 type;

                void onAdd() {
                    //std::cout << "Action Started: " << getOwner()->_ID << std::endl;
                }

                void update(f32 dt) {
                    switch (type)
                    {
                    case 0:
                        *ref_float = actionNode->getPositionX();
                        break;
                    case 1:
                        *ref_int = actionNode->getPositionX();
                        break;
                    case 2:
                        *ref_uint8 = actionNode->getPositionX();
                        break;
                    default:
                        break;
                    }
                }

                LerpPropertyActionComponent(Node* _owner) {
                    actionNode = Node::create();
                    _owner->addChild(actionNode);
                    setName(__func__);
                    setEnabled(true);
                    setOwner(_owner);
                    owner = _owner;
                }

                LerpPropertyActionComponent* initFloat(f32* _ref, f32 _duration, f32 _start, f32 _end, f32 _finish) {
                    type = 0;
                    ref_float = _ref;
                    actionNode->setPositionX(start = _start);
                    auto actionTo = MoveTo::create(_duration, Vec2(end = _end, _finish));
                    auto finishFunc = CallFunc::create([&]() {
                        *ref_float = actionNode->getPositionX();
                        *ref_float = actionNode->getPositionY();
                        owner->removeComponent(this);
                        owner->removeChild(actionNode);
                        this->release();
                    });
                    action = Sequence::create(actionTo, finishFunc, nullptr);
                    actionNode->runAction(action);
                    return this;
                }
            };

            class LerpFuncCallFloatShaderComponent : public Component {
            public:
                std::function<void(cocos2d::backend::ProgramState*, f32)> func;
                f32 current, time, start, end;
                cocos2d::backend::ProgramState* shader;

                LerpFuncCallFloatShaderComponent(cocos2d::backend::ProgramState* _shader, std::function<void(cocos2d::backend::ProgramState*, f32)> _func, f32 _time, f32 _start, f32 _end)
                : func(_func), shader(_shader), current(0), time(_time), start(_start), end(_end)
                {
                    setName(__func__);
                    setEnabled(true);
                    current = start;
                }

                void update(f32 dt) {
                    float exp = dt * time;
                    current = current + (start > end ? -exp : exp);
                    if (end > start && current > end)
                    {
                        func(shader, end);
                        _owner->removeComponent(this);
                        return;
                    }
                    if (start > end && current < end)
                    {
                        func(shader, end);
                        _owner->removeComponent(this);
                        return;
                    }
                    func(shader, current);
                }
            };

            class FMODAudioLerpActionComponent : public Component {
            public:
                Node* owner;
                Node* actionNode;
                Sequence* action;
                FMOD::Channel* ref;
                f32 start, end;

                void onAdd() {
                    //std::cout << "Action Started: " << getOwner()->_ID << std::endl;
                }

                void update(f32 dt) {
                    ref->setVolume(actionNode->getPositionX());
                }

                FMODAudioLerpActionComponent(Node* _owner) {
                    actionNode = Node::create();
                    _owner->addChild(actionNode);
                    setName(__func__);
                    setEnabled(true);
                    setOwner(actionNode);
                    owner = _owner;
                }

                FMODAudioLerpActionComponent* initComponent(FMOD::Channel* sound, f32 _duration, f32 _start, f32 _end) {
                    ref = sound;
                    getOwner()->setPositionX(start = _start);
                    auto actionTo = MoveTo::create(_duration, Vec2(end = _end, 0));
                    auto ease = EaseQuadraticActionIn::create(actionTo);
                    auto finish = CallFunc::create([&]() {
                        ref->setVolume(getOwner()->getPositionX());
                        actionNode->removeComponent(this);
                        owner->removeChild(actionNode);
                        delete this;
                        });
                    action = Sequence::create(actionTo, finish, nullptr);
                    getOwner()->runAction(action);
                    actionNode->addComponent(this);
                    return this;
                }

                //LerpFloatAction(float* _ref, f32 duration, f32 by) {
                //    ref = _ref;
                //    node = Node::create();
                //    node->retain();
                //    node->setPositionX(*ref);
                //    actionBy = MoveTo::create(duration, Vec2(by, 0));
                //    isBy = true;
                //}
            };

            class DestroyNodeComponent : public Component {
            public:
                f32 time_s;
                f32 time_e;

                DestroyNodeComponent(f32 _time) {
                    setName(__func__);
                    setEnabled(true);
                    time_s = 0;
                    time_e = _time;
                }

                void update(f32 dt) {
                    if (time_s < time_e)
                        time_s += dt;
                    else
                    {
                        _owner->removeFromParentAndCleanup(true);
                        setEnabled(false);
                    }
                }

                void onAdd() {

                }
            };

            class DrawNodeCircleExpandComponent : public Component {
            public:
                f32 r_start, r_end, time, amount;
                i32 segs;

                DrawNodeCircleExpandComponent(f32 _time, f32 _amount, i32 _segs) {
                    setName(__func__);
                    setEnabled(true);
                    r_start = 0;
                    r_end = 1;
                    time = 1.0 / _time;
                    amount = _amount;
                    segs = _segs;
                }

                void update(f32 dt) {
                    ((DrawNode*)_owner)->clear();
                    if (r_start < r_end)
                        r_start += dt * time;
                    if (r_start > r_end)
                        r_start = 0;

                    ((DrawNode*)_owner)->drawSolidCircle(Vec2::ZERO, r_start * amount,
                        MATH_DEG_TO_RAD(0), segs, true, true, Color4B(255, 255, 255, (255 * (r_end - r_start)) / 10));
                    ((DrawNode*)_owner)->drawCircle(Vec2::ZERO, r_start * amount,
                        MATH_DEG_TO_RAD(0), segs, false, Color4B(255, 255, 255, (255 * (r_end - r_start))));
                }

                void onAdd() {

                }
            };
        }

        namespace AdvancedUi
        {


        }
    }
}

#endif
// END OF GAME UTILITIES
