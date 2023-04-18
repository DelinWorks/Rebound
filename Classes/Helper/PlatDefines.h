#ifndef __PLAT_DEFINES_H__
#define __PLAT_DEFINES_H__

#include <string.h>
#ifndef WIN32
#define __FILENAME__ (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)
#endif
#ifdef WIN32
#define __FILENAME__ (strrchr(__FILE__, '\\') ? strrchr(__FILE__, '\\') + 1 : __FILE__)
#endif

#define RES_PATH "Resources"
#define LRES_PATH L"Resources"

#if WIN32
static std::wstring getCurrentDirectoryW()
{
    WCHAR buffer[MAX_PATH];
    GetModuleFileNameW(NULL, buffer, MAX_PATH);
    std::wstring::size_type pos = std::wstring(buffer).find_last_of(L"\\/");

    return std::wstring(buffer).substr(0, pos);
}

static std::wstring getCurrentResourcesDirectoryW()
{
    WCHAR buffer[MAX_PATH];
    GetModuleFileNameW(NULL, buffer, MAX_PATH);
    std::wstring::size_type pos = std::wstring(buffer).find_last_of(L"\\/");

    return std::wstring(buffer).substr(0, pos) + L"\\" + LRES_PATH + L"\\";
}

static void toClipboard(const std::string& s) {
	OpenClipboard(nullptr);
	EmptyClipboard();
	HGLOBAL hg = GlobalAlloc(GMEM_MOVEABLE, s.size() + 1);
	if (!hg) {
		CloseClipboard();
		return;
	}
	memcpy(GlobalLock(hg), s.c_str(), s.size() + 1);
	GlobalUnlock(hg);
	SetClipboardData(CF_TEXT, hg);
	CloseClipboard();
	GlobalFree(hg);
}

static std::string fromClipboard()
{
	OpenClipboard(nullptr);
	HANDLE hData = GetClipboardData(CF_TEXT);
	if (!hData) return "?";
	char* pszText = static_cast<char*>(GlobalLock(hData));
	std::string text(pszText);
	GlobalUnlock(hData);
	CloseClipboard();
	return text;
}

#endif
#endif