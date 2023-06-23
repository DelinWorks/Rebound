#ifndef _SHORTTYPES_H__
#define _SHORTTYPES_H__

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

// signed 8 bit int (signed char)
typedef std::int8_t i8;

// signed 16 bit int (short)
typedef std::int16_t i16;

// signed 32 bit int (int)
typedef std::int32_t i32;

// signed 32 bit int (int)
typedef std::int64_t i64;

// signed 32 bit long (long)
typedef long l32;

// unsigned 8 bit int (byte)
typedef std::uint8_t u8;

// unsigned 16 bit int (ushort)
typedef std::uint16_t u16;

// unsinged 32 bit int (unsigned int)
typedef std::uint32_t u32;

// unsinged 64 bit long long (unsigned long long)
typedef std::uint64_t u64;

// signed 64 bit int (long long)
typedef long long ll;

// unsigned 64 bit int (unsigned long long)
typedef unsigned long long ull;

// singed 32 bit floating point (float) (sacrificing precision for speed)
typedef float f32;

// singed 64 bit floating point (double) (sacrificing speed for precision)
typedef double f64;

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

#endif