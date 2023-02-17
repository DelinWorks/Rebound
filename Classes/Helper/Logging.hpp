#include <string>
#include "cocos2d.h"
#include <ctime>
#include "PlatDefines.h"
#include <fstream>

#include "string_manipulation_lib/stringFunctions.hpp"

USING_NS_CC;

enum LogType {
	LOG_TYPE_ERROR       = 0,
	LOG_TYPE_WARNING     = 1,
	LOG_TYPE_INFORMATION = 2
};

class Logging {
public:
	static void log_to_file(std::string content, std::string classnm = "NA", int line = 0, LogType typenm = LOG_TYPE_ERROR, std::string path = "error.log")
	{
#ifdef WIN32
		path = Strings::narrow(getCurrentDirectoryW() + L"\\" + Strings::widen(path.c_str()));
		content += "\n";
		time_t now = time(0);
		tm* ltm = localtime(&now);
		std::string pres = "[";
		pres += std::to_string(ltm->tm_hour) + ":";
		pres += std::to_string(ltm->tm_min) + ":";
		pres += std::to_string(ltm->tm_sec);
		std::string logType = "ERROR";
		if (typenm == LOG_TYPE_WARNING)
			logType = "WARNING";
		else if (typenm == LOG_TYPE_INFORMATION)
			logType = "INFORMATION";
		pres += "::" + logType + "::" + classnm + "(" + std::to_string(line) + ")]";
		std::ofstream outfile;
		outfile.open(path, std::ios_base::app); // append instead of overwrite
		outfile << pres + " " + content;
#endif
	}
};