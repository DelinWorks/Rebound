#ifndef __EXTRA_MATH_H__
#define __EXTRA_MATH_H__

#include "short_types.h"
#include <iostream>
#include <cmath>
#include <vector>
#include <functional>

#ifndef MIN
#    define MIN(x, y) (((x) > (y)) ? (y) : (x))
#endif  // MIN

#ifndef MAX
#    define MAX(x, y) (((x) < (y)) ? (y) : (x))
#endif  // MAX

namespace Math
{
    // takes a value and limits it to be within a specified range defined by the minimumand maximum values. If the value is greater than the maximum, it returns the maximum value. If the value is less than the minimum, it returns the minimum value. Otherwise, it returns the original value.
    inline F32 clamp(F32 current, F32 min, F32 max) {
        if (min > max) return current;
        if (current > max)
            return max;
        if (current < min)
            return min;
        return current;
    }

    // takes a floating-point number and snaps it to the nearest multiple of an integer interval. It rounds the input number divided by the interval to get an integer result and then multiplies that result by the interval to get the snapped value.
    inline I32 snap(F32 i, I32 v) { return (I32)(round(i / v) * v); }

    // similar to "snap" but allows for snapping to a fractional interval. It takes an original floating-point number and snaps it to the nearest multiple of a fraction defined by numerator and denominator values. It rounds the original number multiplied by denominator divided by numerator to get an integer result and then multiplies that result by numerator divided by denominator to get the snapped value.
    inline F64 snap_interval(F64 original, F64 numerator, F64 denominator)
    { 
        return round(original * denominator / numerator) * numerator / denominator;
    }

    // maps a given input value from one range of values into another range of values. It takes an input value x and maps it from an input range defined by in_min and in_max into an output range defined by out_min and out_max using linear interpolation.
    inline F32 map(F32 x, F32 in_min, F32 in_max, F32 out_min, F32 out_max)
    {
        return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
    }

    inline F32 map_clamp_out(F32 x, F32 in_min, F32 in_max, F32 out_min, F32 out_max)
    {
        F32 r = map(x, in_min, in_max, out_min, out_max);
        return std::clamp<F32>(r, MIN(out_min, out_max), MAX(out_min, out_max));
    }

    inline void getVec2Average(ax::Vec2& d, ax::Vec2* s, int c) {
        d = { 0, 0 };
        for (int i = 0; i < c; i++) {
            d.x += s[i].x;
            d.y += s[i].y;
        }
        d.x /= static_cast<float>(c);
        d.y /= static_cast<float>(c);
    }

    inline F32 getEven(F32 x) {
        return round(x / 2) * 2;
    }

    inline ax::Vec2 getEven(const ax::Vec2& v) {
        return ax::Vec2(getEven(v.x), getEven(v.y));
    }

    inline F32 getOdd(F32 x) {
        return 2 * ((int)(x / 2.0f)) + 1;
    }

    inline ax::Vec2 getOdd(const ax::Vec2& v) {
        return ax::Vec2(getOdd(v.x), getOdd(v.y));
    }

    inline ax::Vec2 getRound(const ax::Vec2& v) {
        return ax::Vec2(round(v.x), round(v.y));
    }

    inline int cantorPair(int x, int y) {
        return (x + y) * (x + y + 1) / 2 + y;
    }

    inline ax::Vec2 cantorUnpair(int z) {
        ax::Vec2 v;
        int w = floor((sqrt(8 * z + 1) - 1) / 2);
        int t = (w * w + w) / 2;
        v.y = z - t;
        v.x = w - v.y;
        return v;
    }

    inline int cantorPairNeg(uint32_t x, uint32_t y) {
        x += INT32_MAX / 2;
        y += INT32_MAX / 2;
        return (x + y) * (x + y + 1) / 2 + y;
    }

    inline ax::Vec2 cantorUnpairNeg(uint32_t z) {
        ax::Vec2 v;
        int w = floor((sqrt(8 * z + 1) - 1) / 2);
        int t = (w * w + w) / 2;
        v.y = z - t;
        v.x = w - v.y;
        return v - INT32_MAX / 2;
    }

    inline int closestPowerOfTwo(int n) {
        int log2n = log2(n);
        if (n - pow(2, log2n) >= pow(2, log2n + 1) - n)
            return pow(2, log2n + 1);
        else
            return pow(2, log2n);
    }

    inline int closestMultipleRound(int num, int factor) {
        int quotient = num / factor;
        int closestMultiple = factor * quotient;
        if (num > closestMultiple + factor / 2) {
            closestMultiple += factor;
        }
        return closestMultiple;
    }

    inline int closestMultipleCeil(int num, int factor) {
        int quotient = num / factor;
        int closestMultiple = factor * quotient;
        if (num > closestMultiple) {
            closestMultiple += factor;
        }
        return closestMultiple;
    }

    inline int closestMultipleFloor(int num, int factor) {
        int quotient = num / factor;
        int closestMultiple = factor * quotient;
        return closestMultiple;
    }

    inline bool rectIntersectsRectOffOrigin(const ax::Rect& r1, const ax::Rect& r2) {
        return !(r1.size.x < r2.origin.x || r2.size.x < r1.origin.x || r1.size.y < r2.origin.y || r2.size.y < r1.origin.y);
    }

#define CPT Math::closestPowerOfTwo
#define CMR Math::closestMultipleRound
#define CMC Math::closestMultipleCeil
#define CMF Math::closestMultipleFloor

#define CANTOR_PAIR Math::cantorPair
#define CANTOR_UNPAIR Math::cantorUnpair
}

class Vec2Hashable {
public:
    float x; float y;

    Vec2Hashable() : x(0), y(0) {}
    Vec2Hashable(const V2D v) : x(v.x), y(v.y) {}
    Vec2Hashable(const Vec2Hashable& v) : x(v.x), y(v.y) {}
    Vec2Hashable(float _x, float _y) : x(_x), y(_y) {}

    bool operator==(const Vec2Hashable& other) const
    {
        return (x == other.x && y == other.y);
    }
};

namespace std {
    template<>
    struct hash<Vec2Hashable> {
        size_t operator()(const Vec2Hashable& v) const noexcept {
            const uint32_t kFnvOffsetBasis = 0x811c9dc5;
            const uint32_t kFnvPrime = 0x01000193;
            const uint32_t uX = static_cast<uint32_t>(v.x);
            const uint32_t uY = static_cast<uint32_t>(v.y);
            // Compute FNV-1a hash of (x, y)
            uint32_t hash = kFnvOffsetBasis;
            hash ^= uX;
            hash *= kFnvPrime;
            hash ^= uY;
            hash *= kFnvPrime;
            return hash;
        }
        hash() = default;
        bool operator==(const hash& other) const = default;
    };
}

typedef Vec2Hashable V2DH;

class I64Hashable {
public:
    I64 v;

    I64Hashable() : v(0) {}
    I64Hashable(const I64 _v) : v(_v) {}

    bool operator==(I64 other) const
    {
        return v == other;
    }
};

namespace std {
    template<>
    struct hash<I64Hashable> {
        size_t operator()(I64 v) const noexcept {
            const uint64_t m = 0x5bd1e995;
            const int r = 24;
            uint64_t hash = 0xDEADBEEF ^ static_cast<uint64_t>(v);
            hash ^= hash >> r;
            hash *= m;
            hash ^= hash >> r;
            return static_cast<size_t>(hash);
        }
        hash() = default;
        bool operator==(const hash& other) const = default;
    };
}

#endif
