#pragma once

#include <immintrin.h>

namespace ving
{
struct vec4
{
    vec4() : x{0.0f}, y{0.0f}, z{0.0f}, w{0.0f} {}
    vec4(float _x, float _y, float _z, float _w) : x{_x}, y{_y}, z{_z}, w{_w} {}

    [[nodiscard]] float &operator[](int i);
    [[nodiscard]] const float &operator[](int i) const;

    void operator+=(vec4 b);
    void operator-=(vec4 b);

    union {
        struct
        {
            float x, y, z, w;
        };
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
