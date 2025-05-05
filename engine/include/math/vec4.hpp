#pragma once

#include <immintrin.h>

namespace ving
{
struct vec4
{
    union {
        float x, y, z, w;
        __m128 data;
    };
};

inline vec4 operator*(float a, vec4 b)
{
    vec4 result;
    __m128 av = _mm_load1_ps(&a);
    result.data = _mm_mul_ps(b.data, av);
    return result;
}

inline vec4 operator+(vec4 a, vec4 b)
{
    vec4 result;
    result.data = _mm_add_ps(a.data, b.data);
    return result;
}
inline vec4 operator-(vec4 a, vec4 b)
{
    vec4 result;

    result.x = a.x - b.x;
    result.y = a.y - b.y;
    result.z = a.z - b.z;
    result.w = a.w - b.w;

    return result;
}
inline vec4 operator-(vec4 v)
{
    vec4 result;

    result.x = -v.x;
    result.y = -v.y;
    result.z = -v.z;
    result.w = -v.w;

    return result;
}

} // namespace ving
