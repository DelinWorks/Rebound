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

#define RLOG Rebound::log
#define RLOGE Rebound::loge

namespace Rebound
{
	template <typename... T>
	void log(fmt::format_string<T...> fmt, T&&... args) {
#ifdef WIN32
		HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
		SetConsoleTextAttribute(hConsole, 9);
#endif
		fmt::print("[{:%H:%M:%S}] {}\n", fmt::gmtime(std::time(NULL)), fmt::format(fmt, std::forward<T>(args)...));
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
		fmt::print("[{:%H:%M:%S}] {}\n", fmt::gmtime(std::time(NULL)), fmt::format(fmt, std::forward<T>(args)...));
#ifdef WIN32
		SetConsoleTextAttribute(hConsole, 15);
#endif
	}
}
