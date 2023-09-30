#include "cocos2d.h"
#include <string>
#include <codecvt>

#ifndef __STRINGS_H__
#define __STRINGS_H__

#define VECTOR_CHARS std::vector<char>
#define VECTOR_STRING std::vector<std::string>
#define VECTOR_WSTRING std::vector<std::wstring>

#define CCP(d) (const char*)d
#define CWCP(d) (const wchar_t*)d

#define NRW Strings::narrow
#define WDN Strings::widen

#define MAX_WC2MB_ALLOCATION 1024

namespace Strings
{
    template <class T>
    inline void ltrim(T& s) {
        s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](unsigned char ch) {
            return !std::isspace(ch);
            }));
    }

    template <class T>
    inline void rtrim(T& s) {
        s.erase(std::find_if(s.rbegin(), s.rend(), [](unsigned char ch) {
            return !std::isspace(ch);
            }).base(), s.end());
    }

    template <class T>
    inline void trim(T& s) {
        rtrim(s);
        ltrim(s);
    }

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

    inline void split_single_char(std::string& str, const char* delim, VECTOR_CHARS& out)
    {
        size_t start;
        size_t end = 0;

        while ((start = str.find_first_not_of(delim, end)) != std::string::npos)
        {
            end = str.find(delim, start);
            out.push_back(str.substr(start, end - start)[0]);
        }
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

    inline std::wstring widen(const std::string& utf8)
    {
        std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t> convert;
        std::u16string utf16 = convert.from_bytes(utf8);
        std::wstring wstr(utf16.begin(), utf16.end());
        return wstr;
    }

    inline std::string narrow(const std::wstring& utf16) {
        std::u16string u16str(utf16.begin(), utf16.end());
        std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t> convert;
        std::string utf8 = convert.to_bytes(u16str);
        return utf8;
    }

    template <class T>
    inline T alt_duplicate(std::vector<T> list, T dup)
    {
        trim<T>(dup);
        T name = dup;
        bool hasDuplicate = true;
        int duplicateCount = 1;
        while (hasDuplicate)
        {
            hasDuplicate = false;
            for (auto& _ : list)
            {
                if (_ == name)
                {
                    hasDuplicate = true;
                    if constexpr (std::is_same_v<T, std::string>) {
                        name = dup + "_" + std::to_string(++duplicateCount);
                    }
                    else if constexpr (std::is_same_v<T, std::wstring>) {
                        name = dup + L"_" + std::to_wstring(++duplicateCount);
                    }
                    break;
                }
            }
            if (!hasDuplicate) break;
        }
        return name;
    }
}

#endif