#include "cocos2d.h"
#include <string>

#define VECTOR_STRING std::vector<std::string>
#define VECTOR_WSTRING std::vector<std::wstring>

#define CCP(d) (const char*)d
#define CWCP(d) (const wchar_t*)d

#include "ntcvt/ntcvt.hpp"

#ifndef __STRINGS_H__
#define __STRINGS_H__

#define MAX_WC2MB_ALLOCATION 1024

namespace Strings {
    inline bool replace(std::string& str, const std::string& from, const std::string& to) {
        size_t start_pos = str.find(from);
        if (start_pos == std::string::npos)
            return false;
        str.replace(start_pos, from.length(), to);
        return true;
    }

    inline bool wreplace(std::wstring& str, const std::wstring& from, const std::wstring& to) {
        size_t start_pos = str.find(from);
        if (start_pos == std::wstring::npos)
            return false;
        str.replace(start_pos, from.length(), to);
        return true;
    }

    inline std::string replace_const(const std::string str, const std::string& from, const std::string& to) {
        std::string final = str;
        size_t start_pos = final.find(from);
        if (start_pos == std::string::npos)
            return final;
        final.replace(start_pos, from.length(), to);
        return final;
    }

    inline std::wstring wreplace_const(const std::wstring str, const std::wstring& from, const std::wstring& to) {
        std::wstring final = str;
        size_t start_pos = final.find(from);
        if (start_pos == std::wstring::npos)
            return final;
        final.replace(start_pos, from.length(), to);
        return final;
    }

    inline void split(std::string& str, const char* delim, VECTOR_STRING& out)
    {
        size_t start;
        size_t end = 0;

        while ((start = str.find_first_not_of(delim, end)) != std::string::npos)
        {
            end = str.find(delim, start);
            out.push_back(str.substr(start, end - start));
        }
    }

    inline void wsplit(std::wstring& str, const wchar_t* delim, VECTOR_WSTRING& out)
    {
        size_t start;
        size_t end = 0;

        while ((start = str.find_first_not_of(delim, end)) != std::string::npos)
        {
            end = str.find(delim, start);
            out.push_back(str.substr(start, end - start));
        }
    }

    inline std::string gen_random(int len) {
        const char alphanum[] =
            "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
            "abcdefghijklmnopqrstuvwxyz"
            "0123456789";

        std::string tmp_s;
        tmp_s.reserve(len);

        for (int i = 0; i < len; ++i)
            tmp_s += alphanum[rand() % (sizeof(alphanum) - 1)];

        return tmp_s;
    }

    //// Wide Char to MultiByte: CHAR
    //inline const char* WC2MB(const wchar_t* data) {
    //    char c_data[MAX_PATH];
    //    WideCharToMultiByte(CP_UTF8, 0, data, -1, c_data, sizeof(c_data), NULL, NULL);
    //    return c_data;
    //    //std::wstring data { c_data };
    //    //return ntcvt::wcbs2a<std::string>(data.c_str(), sizeof(data.c_str()) + 1).c_str();
    //}

    //// Wide Char to MultiByte: STRING
    //inline std::string WC2MB(std::wstring data) {
    //    return std::string(WC2MB(data.c_str()));
    //}

    inline const wchar_t* widen(const char* str)
    {
        int str_len = (int)strlen(str);
        int num_chars = MultiByteToWideChar(CP_UTF8, 0, str, str_len, NULL, 0);
        wchar_t* wstrTo = (wchar_t*)malloc((num_chars + 1) * sizeof(WCHAR));
        if (wstrTo)
        {
            MultiByteToWideChar(CP_UTF8, 0, str, str_len, wstrTo, num_chars);
            wstrTo[num_chars] = L'\0';
        }
        return wstrTo;
    }

    inline const char* narrow(const wchar_t* wstr)
    {
        int wstr_len = (int)wcslen(wstr);
        int num_chars = WideCharToMultiByte(CP_UTF8, 0, wstr, wstr_len, NULL, 0, NULL, NULL);
        char* strTo = (char*)malloc((num_chars + 1) * sizeof(char));
        if (strTo)
        {
            WideCharToMultiByte(CP_UTF8, 0, wstr, wstr_len, strTo, num_chars, NULL, NULL);
            strTo[num_chars] = '\0';
        }
        return strTo;
    }

    inline std::wstring widen(std::string data)
    {
        return std::wstring(widen(data.c_str()));
    }

    inline std::string narrow(std::wstring data)
    {
        return std::string(narrow(data.c_str()));
    }

}

#endif