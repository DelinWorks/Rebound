#pragma once

//#define I8 byte
//#define I16 short
//#define UI16 ushort
//#define I32 int
//#define UI32 uint
//#define I64 long
//#define I128 long long
//#define ULL unsigned long long
//#define F32 float
//#define F64 double
//#define STR std::string

#include <iostream>

// signed 8 bit int (signed char)
typedef std::int8_t I8;

// signed 16 bit int (short)
typedef std::int16_t I16;

// signed 32 bit int (int)
typedef std::int32_t I32;

// signed 32 bit int (int)
typedef std::int64_t I64;

// signed 32 bit long (long)
typedef long L32;

// unsigned 8 bit int (byte)
typedef std::uint8_t U8;

// unsigned 16 bit int (ushort)
typedef std::uint16_t U16;

// unsinged 32 bit int (unsigned int)
typedef std::uint32_t U32;

// unsinged 64 bit long long (unsigned long long)
typedef std::uint64_t U64;

// signed 64 bit int (long long)
typedef long long LL;

// unsigned 64 bit int (unsigned long long)
typedef unsigned long long ULL;

// singed 32 bit floating point (float) (sacrificing precision for speed)
typedef float F32;

// singed 64 bit floating point (double) (sacrificing speed for precision)
typedef double F64;

#define TEXT(t) std::string(t)
#define TO_TEXT(t) std::to_string(t)

#define SCAST static_cast
#define DCAST(T, O) dynamic_cast<T*>(O)
#define RCAST reinterpret_cast
#define CCCP const char*
#define C_UCHAR_P const unsigned char*

#define SELF this->

#define _NOTHING nullptr

#define LERP MathUtil::lerp

#include "math/Vec2.h"
#include "math/Vec3.h"
#include "math/Vec4.h"
#include "math/Rect.h"

typedef ax::Vec2 V2D;
typedef ax::Size S2D;
typedef ax::Rect R2D;

typedef ax::Vec3 V3D;
typedef ax::Vec4 V4D;

#include <list>

template<typename T>
class MaxPushList {
public:
    MaxPushList() {}
    MaxPushList(size_t max_push_count) : _max_push_count(max_push_count) {}

    T& at(int _i) {
        auto it = _list.begin();
        for (int i = 0; i < _i; i++) {
            ++it;
        }
        return *it;
    }

    void push_back(const T& value) {
        if (_list.size() == _max_push_count) {
            _list.pop_front();
        }
        _list.push_back(value);
    }

    void fill(const T& value) {
        for (int i = 0; i < _max_push_count; i++) {
            push_back(value);
        }
    }

    size_t _max_push_count;
    std::list<T> _list;
};
