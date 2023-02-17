#ifndef __EXTRA_MATH_H__
#define __EXTRA_MATH_H__

#include "short_types.h"
#include <cmath>
#include <vector>

namespace Math
{
    inline f32 clamp(f32 current, f32 min, f32 max) {
        if (current > max)
            return max;
        if (current < min)
            return min;
        return current;
    }

    inline i32 snap(f32 i, i32 v) { return (i32)(round(i / v) * v); }

    inline f64 snap_interval(f64 original, i32 numerator, i32 denominator) { return round(original * denominator / numerator) * numerator / denominator; }

    inline f32 getVectorAVG(const std::vector<f32>& nums)
    {
        f32 sum = 0.f;
        for (auto i : nums)
            sum += i;
        return sum / nums.size();
    }
}

#endif
