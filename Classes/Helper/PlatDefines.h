#ifndef __PLAT_DEFINES_H__
#define __PLAT_DEFINES_H__

#include <string.h>
#ifndef WIN32
#define __FILENAME__ (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)
#endif
#ifdef WIN32
#define __FILENAMEPASS__ (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)
#define __FILENAME__ (strrchr(__FILENAMEPASS__, '\\') ? strrchr(__FILENAMEPASS__, '\\') + 1 : __FILENAMEPASS__)
#endif

#define RES_PATH "Resources"
#define LRES_PATH L"Resources"

#if WIN32
#include <windows.h>
#include <commdlg.h>

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

static std::wstring getSingleFileDialog(HWND window)
{
	OPENFILENAME ofn;       // common dialog box structure
	TCHAR szFile[260] = { 0 };       // if using TCHAR macros

	// Initialize OPENFILENAME
	ZeroMemory(&ofn, sizeof(ofn));
	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = window;
	ofn.lpstrFile = szFile;
	ofn.nMaxFile = sizeof(szFile);
	ofn.lpstrFilter = L"All\0*.*\0png\0*.png\0";
	ofn.nFilterIndex = 1;
	ofn.lpstrFileTitle = NULL;
	ofn.nMaxFileTitle = 0;
	ofn.lpstrInitialDir = NULL;
	ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

	if (GetOpenFileName(&ofn) == TRUE)
		return ofn.lpstrFile;
	return L"";
}

#endif
#endif