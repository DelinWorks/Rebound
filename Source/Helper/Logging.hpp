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

#define RLOG(...) do { _loggerCurrColor = 11; _loggerCurrFileName = __FILENAME__; _loggerCurrLine = __LINE__; ReboundLog::log(__VA_ARGS__); } while (false);
#define RLOGW(...) do { _loggerCurrColor = 12; _loggerCurrFileName = __FILENAME__; _loggerCurrLine = __LINE__; ReboundLog::log(__VA_ARGS__); } while (false);
#define RLOGE(...) do { _loggerCurrFileName = __FILENAME__; _loggerCurrLine = __LINE__; ReboundLog::loge(__VA_ARGS__); } while (false);

#define LOG_RELEASE RLOGW("object {} release", typeid(this).name())

#ifdef NDEBUG
#define RLOG 
#define RLOGW
#define RLOGE
#define LOG_RELEASE
#endif

#define RB_PROMISE_RELEASE(O) PoolManager::getInstance()->getCurrentPool()->addObject(O);

namespace ReboundLog
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

inline const char* benchmark_bb7_name;
inline std::chrono::steady_clock::time_point benchmark_bb7_start;
inline std::chrono::steady_clock::time_point benchmark_bb7_end;
#define BENCHMARK_SECTION_BEGIN(name) benchmark_bb7_name = name; benchmark_bb7_start = std::chrono::high_resolution_clock::now();
#define BENCHMARK_SECTION_END() benchmark_bb7_end = std::chrono::high_resolution_clock::now(); \
RLOG("benchmark {} took: {} millis, {} micros", benchmark_bb7_name, std::chrono::duration_cast<std::chrono::milliseconds>(benchmark_bb7_end - benchmark_bb7_start).count(), std::chrono::duration_cast<std::chrono::microseconds>(benchmark_bb7_end - benchmark_bb7_start).count());
