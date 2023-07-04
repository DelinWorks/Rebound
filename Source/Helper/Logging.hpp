#pragma once

#include <string>
#include "cocos2d.h"
#include <ctime>
#include "PlatDefines.h"
#include <fstream>
#include <fmt/format.h>
#include <fmt/chrono.h>

#include "string_manipulation_lib/stringFunctions.hpp"

USING_NS_CC;

inline int _loggerCurrColor;
inline const char* _loggerCurrFileName;
inline int _loggerCurrLine;

#define RLOG _loggerCurrColor = 11; _loggerCurrFileName = __FILENAME__; _loggerCurrLine = __LINE__; Rebound::log
#define RLOGW _loggerCurrColor = 12; _loggerCurrFileName = __FILENAME__; _loggerCurrLine = __LINE__; Rebound::log
#define RLOGE _loggerCurrFileName = __FILENAME__; _loggerCurrLine = __LINE__; Rebound::loge

#define LOG_RELEASE /*RLOGW("object {} release", typeid(this).name())*/

//#ifdef NDEBUG
//#define RLOG 
//#define RLOGW
//#define RLOGE
//#define LOG_RELEASE
//#endif

#define RB_PROMISE_RELEASE(O) PoolManager::getInstance()->getCurrentPool()->addObject(O);

namespace Rebound
{
	template <typename... T>
	void log(fmt::format_string<T...> fmt, T&&... args) {
#ifdef WIN32
		HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
		SetConsoleTextAttribute(hConsole, _loggerCurrColor);
#endif
		fmt::print("[{}({})] {}\n", _loggerCurrFileName, _loggerCurrLine, fmt::format(fmt, std::forward<T>(args)...));
#ifdef WIN32
		SetConsoleTextAttribute(hConsole, 15);
#endif
	}

	template <typename... T>
	void loge(bool assert, fmt::format_string<T...> fmt, T&&... args) {
#ifdef WIN32
		HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
		SetConsoleTextAttribute(hConsole, assert ? 10 : 4);
#endif
		fmt::print("[{}({})] {}\n", _loggerCurrFileName, _loggerCurrLine, fmt::format(fmt, std::forward<T>(args)...));
#ifdef WIN32
		SetConsoleTextAttribute(hConsole, 15);
#endif
	}
}
