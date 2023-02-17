#ifdef WIN32

#ifndef _WIN32_ERROR_H__
#define _WIN32_ERROR_H__

#include <string>
#include <WinUser.h>
#include <wtypes.h>
class win32_error {
public:
	static void assert_win32(HWND hWnd, bool condition, std::string message, std::string caption = "assert failed!") {
		if (!condition) {
            std::wstring cMessage = std::wstring(message.begin(), message.end());
            LPCWSTR wMessage = cMessage.c_str();
			std::wstring cCaption = std::wstring(caption.begin(), caption.end());
			LPCWSTR wCaption = cCaption.c_str();
			MessageBox(hWnd, wMessage, wCaption, 0x00000010);
		}
	}
};
#endif

#endif
