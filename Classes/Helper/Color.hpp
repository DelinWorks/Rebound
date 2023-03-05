#include <string>
#include <iostream>
#include <sstream>
#include "cocos2d.h"

USING_NS_CC;

#ifndef __H_COLORCONVERSION__
#define __H_COLORCONVERSION__

class ColorConversion
{
public:
    static cocos2d::Color4B hex2rgba(std::string hex)
    {
        if (hex.at(0) == '#') {
            hex.erase(0, 1);
        }
        uint32_t raw_hex;
        std::stringstream ss;
        ss << std::hex << hex;
        ss >> raw_hex;
        Color4B col = Color4B(0, 0, 0, 0);
        col.r = ((raw_hex >> 24) & 0xFF);
        col.g = ((raw_hex >> 16) & 0xFF);
        col.b = ((raw_hex >> 8) & 0xFF);
        col.a = ((raw_hex) & 0xFF);
        return col;
    }

    static cocos2d::Color4B hex2argb(std::string hex)
    {
        if (hex.at(0) == '#') {
            hex.erase(0, 1);
        }
        uint32_t raw_hex;
        std::stringstream ss;
        ss << std::hex << hex;
        ss >> raw_hex;
        Color4B col = Color4B(0, 0, 0, 0);
        col.a = ((raw_hex >> 24) & 0xFF);
        col.r = ((raw_hex >> 16) & 0xFF);
        col.g = ((raw_hex >> 8) & 0xFF);
        col.b = ((raw_hex) & 0xFF);
        return col;
    }

    static std::string rgba2hex(cocos2d::Color4B color, bool with_head = false)
    {
        std::stringstream ss;
        if (with_head)
            ss << "#";
        ss << std::hex << (color.r << 24 | color.g << 16 | color.b << 8 | color.a);
        return ss.str();
    }

    static std::string rgba2hex(int r, int g, int b, int a, bool with_head = false)
    {
        std::stringstream ss;
        if (with_head)
            ss << "#";
        ss << std::hex << (r << 24 | g << 16 | b << 8 | a);
        return ss.str();
    }
};

#endif
