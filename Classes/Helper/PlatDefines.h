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
#endif
#endif