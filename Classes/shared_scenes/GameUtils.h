#include <string>

#ifndef _GameUtils_H__
#define _GameUtils_H__

#define USING_NS_GAMEUTILS using namespace GameUtils

#include "axmol.h"
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
#define ATTACH_SCENE_INPUT_MANAGER(T) addComponent(_input = (new SceneInputManagerComponent()) \
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

#define SCENE_BUILD_UI auto list = GameUtils::findComponentsByName(this, "UiRescaleComponent"); \
for (auto& _ : list) { \
    auto i = DCAST(UiRescaleComponent, _); \
    if (i) { \
        auto c = DCAST(CustomUi::Container, i->getOwner()); \
        if (c && c->_closestStaticBorder) \
        { \
            auto parent = c; \
            while (parent) { \
                parent = DCAST(CustomUi::Container, parent->getParent()); \
                if (!parent) break; \
                if (!parent->_isDynamic) { \
                    i->windowSizeChange(parent->getContentSize()); \
                    break; \
                } \
            } \
        } \
        else i->windowSizeChange(visibleSize); \
    } \
} \
auto scale = Darkness::getInstance()->gameWindow.guiScale; \
if (getContainer()) { getContainer()->updateLayoutManagers(true); \
    getContainer()->onFontScaleUpdate(scale); } \
CustomUi::_UiScale = scale * CustomUi::_UiScaleMul; \

#define SET_POSITION_HALF_SCREEN(node) node->setPosition(Vec2((visibleSize.width / 2), (visibleSize.height / 2)));
#define SET_POSITION_MINUS_HALF_SCREEN(node) node->setPosition(Vec2((visibleSize.width / -2), (visibleSize.height / -2)));
#define DECOMPOSE_VEC2(V, X, Y) Vec2(V.x * X, V.y * Y)

#define SET_UNIFORM(ps, name, value)  do {   \
decltype(value) __v = value;                           \
auto __loc = (ps)->getUniformLocation(name);  \
(ps)->setUniform(__loc, &__v, sizeof(__v));  \
} while(false)

#define SET_UNIFORM_TEXTURE(ps, name, idx, value)  do {   \
auto * __v = value;                           \
auto __loc = (ps)->getUniformLocation(name);  \
(ps)->setTexture(__loc, idx, __v);  \
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

#define ADD_IMAGE Director::getInstance()->getTextureCache()->addImage

USING_NS_CC;

namespace GameUtils
{
#ifdef WIN32
    void GLFW_ClipCursor(bool unset = false);

    void GLFW_SetCursorNormal();

    void GLFW_SetCursorSelected();

    void GLFW_SetBorder(HWND window, int on);
#endif

    Vec2 parseVector2D(std::string position);

    backend::ProgramState* createGPUProgram(std::string resources_frag_shader_path = "", std::string resources_vertex_shader_path = "");

    void addSpriteFramesFromJson(const std::string_view texture_path, const std::string_view json_path);

    Vec2 convertFromScreenToSpace(const Vec2& locationInView, Node* cam, bool reverseY = false);

    void setNodeIgnoreDesignScale(cocos2d::Node* node, bool ignoreScaling = false);

    Size getWinDiff();

    void setNodeScaleFHD(cocos2d::Node* node);

    std::vector<Component*> findComponentsByName(Node* parent, std::string_view name, bool containParent = true, std::vector<Component*> list = std::vector<Component*>(), bool recursive = false);

    std::vector<Node*> findNodesByTag(Node* parent, int tag, bool containParent = true, std::vector<Node*> list = std::vector<Node*>(), bool recursive = false);
}

#endif
// END OF GAME UTILITIES
