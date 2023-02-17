//#ifndef __LUA_STATE_H__
//#define __LUA_STATE_H__
//
//#include <string>
//
//#define LUA_LIB
//
//#include "lua.hpp"
//#include "luajit.h"
//
//#include "extras/short_types.h"
//#include "core/base/CCRef.h"
//#include "platform/CCFileUtils.h"
//#include "core/base/CCDirector.h"
//#include "extras/PlatDefines.h"
//#include "string_manipulation_lib/stringFunctions.hpp"
//
//#define FMT StringUtils::format
//
//#define LUA_CREATE(L) L = luaL_newstate();
//#define LUA_CLOSE(L) if (L) lua_close(L); L = nullptr;
//
//#define LUA_CHECK(r) if (r != LUA_OK) LUA_COUT(LUA_STRING(L, -1));
//#define LUA_COUT(d) std::cout << "[LUA] " << d << std::endl;
//#define LUA_DOFILE(L, f) if (luaL_dostring(L, FileUtils::getInstance()->getStringFromFile(f).c_str()) != LUA_OK) { \
//		LUA_COUT("Failed to execute script: " << f); LUA_COUT(LUA_STRING(L, -1)); }
//
//#define LUA_GLOB_INT(L, n, v) lua_getglobal(L, n); v = (int)lua_tonumber(L, -1);
//#define LUA_GLOB_FLOAT(L, n, v) lua_getglobal(L, n); v = (float)lua_tonumber(L, -1);
//#define LUA_GLOB_STRING(L, n, v) lua_getglobal(L, n); v = (const char*)lua_tostring(L, -1);
//#define LUA_GLOB_BOOL(L, n, v) lua_getglobal(L, n); v = (bool)lua_toboolean(L, -1);
//
//#define LUA_INT(L, i) (int)lua_tonumber(L, -1)
//#define LUA_FLOAT(L, i) (float)lua_tonumber(L, -1)
//#define LUA_STRING(L, i) (const char*)lua_tostring(L, -1)
//#define LUA_BOOL(L, i) (bool)lua_toboolean(L, -1)
//
//static const luaL_Reg lualibs[] = {
//  {"", luaopen_base},
//  {LUA_LOADLIBNAME, luaopen_package},
//  {LUA_TABLIBNAME, luaopen_table},
//  {LUA_STRLIBNAME, luaopen_string},
//  {LUA_MATHLIBNAME, luaopen_math},
//  {LUA_DBLIBNAME, luaopen_debug},
//  {NULL, NULL}
//};
//
//namespace lua_host_functions
//{
//	static bool isServer = false;
//
//	namespace display
//	{
//		inline int get_window_width_in_pixels(lua_State* L)
//		{
//			lua_pushnumber(L, (int)Director::getInstance()->getOpenGLView()->getFrameSize().width);
//			return 1;
//		}
//
//		inline int get_window_height_in_pixels(lua_State* L)
//		{
//			lua_pushnumber(L, (int)Director::getInstance()->getOpenGLView()->getFrameSize().height);
//			return 1;
//		}
//
//		inline int get_interval(lua_State* L)
//		{
//			lua_pushnumber(L, (float)Director::getInstance()->getAnimationInterval());
//			return 1;
//		}
//
//		inline int get_design_width_in_pixels(lua_State* L)
//		{
//			lua_pushnumber(L, (int)Director::getInstance()->getWinSizeInPixels().x);
//			return 1;
//		}
//
//		inline int get_design_height_in_pixels(lua_State* L)
//		{
//			lua_pushnumber(L, (int)Director::getInstance()->getWinSizeInPixels().y);
//			return 1;
//		}
//	}
//
//	namespace darkness
//	{
//		inline int get_version(lua_State* L)
//		{
//			lua_pushnumber(L, 1);
//			lua_pushnumber(L, 2);
//			lua_pushnumber(L, 2);
//
//			return 3;
//		}
//
//		inline int dofile(lua_State* L)
//		{
//			std::wstring str = Strings::widen(lua_tostring(L, 1));
//
//			Strings::wreplace(str, L"${LUA}", L"scripts");
//			Strings::wreplace(str, L"${RES}", L"");
//			Strings::wreplace (str, L"${ROOT}", getCurrentDirectoryW());
//
//			LUA_DOFILE(L, Strings::narrow(str));
//
//			return 0;
//		}
//
//		inline int print(lua_State* L)
//		{
//			LUA_COUT(lua_tostring(L, 1));
//			return 0;
//		}
//
//		inline int setup(lua_State* L)
//		{
//			lua_pushnumber(L, 0);
//			LUA_COUT("Lua engine initialized successfully.");
//			return 1;
//		}
//
//		inline int is_windows(lua_State* L)
//		{
//#if AX_TARGET_PLATFORM == AX_PLATFORM_WIN32
//			lua_pushboolean(L, true);
//#else
//			lua_pushboolean(L, false);
//#endif
//			return 1;
//		}
//
//		inline int is_android(lua_State* L)
//		{
//#if AX_TARGET_PLATFORM == AX_PLATFORM_ANDROID
//			lua_pushboolean(L, true);
//#else
//			lua_pushboolean(L, false);
//#endif
//			return 1;
//		}
//
//		inline int is_ios(lua_State* L)
//		{
//#if AX_TARGET_PLATFORM == AX_PLATFORM_IOS
//			lua_pushboolean(L, true);
//#else
//			lua_pushboolean(L, false);
//#endif
//			return 1;
//		}
//
//		inline int is_client(lua_State* L)
//		{
//			lua_pushboolean(L, !isServer);
//			return 1;
//		}
//
//		inline int is_server(lua_State* L)
//		{
//			lua_pushboolean(L, isServer);
//			return 1;
//		}
//
//		inline int register_display(lua_State* L)
//		{
//			lua_register(L, "get_window_width_in_pixels", display::get_window_width_in_pixels);
//			lua_register(L, "get_window_height_in_pixels", display::get_window_height_in_pixels);
//			lua_register(L, "get_interval", display::get_interval);
//			lua_register(L, "get_design_width_in_pixels", display::get_design_width_in_pixels);
//			lua_register(L, "get_design_height_in_pixels", display::get_design_height_in_pixels);
//			return 0;
//		}
//	}
//}
//
//using namespace lua_host_functions;
//
//inline int lua_register_darkness(lua_State* L)
//{
//	lua_register(L, "get_game_version", darkness::get_version);
//	lua_register(L, "host_dofile", darkness::dofile);
//	lua_register(L, "host_print", darkness::print);
//	lua_register(L, "host_setup", darkness::setup);
//
//	lua_register(L, "host_is_client", darkness::is_windows);
//	lua_register(L, "host_is_server", darkness::is_windows);
//
//	lua_register(L, "host_is_windows", darkness::is_windows);
//	lua_register(L, "host_is_android", darkness::is_android);
//	lua_register(L, "host_is_ios", darkness::is_ios);
//
//	lua_register(L, "host_register_display", darkness::register_display);
//	return 0;
//}
//
//#endif