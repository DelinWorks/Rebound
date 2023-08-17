#include <string>
#include <iostream>
#include <sstream>
#include "cocos2d.h"
#include "short_types.h"

USING_NS_CC;

#ifndef __H_COLORCONVERSION__
#define __H_COLORCONVERSION__

class ColorConversion
{
public:
    static Color4F hex2rgba(std::string hexString) {
        if (hexString.at(0) == '#')
            hexString.erase(0, 1);
        bool overrideAlpha = false;
        if (hexString.length() < 7)
            overrideAlpha = true;
        if (hexString.length() < 8) {
            std::string zeros(8 - hexString.length(), '0');
            hexString += zeros;
        }
        U32 raw = 0;
        std::stringstream ss;
        ss << std::hex << hexString;
        ss >> raw;
        U8 r = ((raw >> 24) & 0xFF);
        U8 g = ((raw >> 16) & 0xFF);
        U8 b = ((raw >> 8) & 0xFF);
        U8 a = ((raw) & 0xFF);
        float red = static_cast<float>(r) / 255.0f;
        float green = static_cast<float>(g) / 255.0f;
        float blue = static_cast<float>(b) / 255.0f;
        float alpha = static_cast<float>(a) / 255.0f;
        return { red, green, blue, float(overrideAlpha ? 1.0 : alpha) };
    }

    static std::string rgba2hex(Color4F rgba) {
        unsigned int r = static_cast<unsigned int>(rgba.r * 255.0f);
        unsigned int g = static_cast<unsigned int>(rgba.g * 255.0f);
        unsigned int b = static_cast<unsigned int>(rgba.b * 255.0f);
        unsigned int a = static_cast<unsigned int>(rgba.a * 255.0f);

        std::stringstream ss;
        ss << std::hex << "#"
            << std::setw(2) << std::setfill('0') << r
            << std::setw(2) << std::setfill('0') << g
            << std::setw(2) << std::setfill('0') << b
            << std::setw(2) << std::setfill('0') << a;

        return ss.str();
    }
};

#endif
