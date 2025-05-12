#pragma once

namespace ving
{
struct vec2
{
    vec2() : x{0.0f}, y{0.0f} {}
    vec2(float _x, float _y) : x{_x}, y{_y} {}
    vec2(float s) : x{s}, y{s} {}

    void operator+=(vec2 b);

    union {
        struct
        {
            float x, y;
        };
    };
};

vec2 operator*(float s, vec2 v);

vec2 operator+(vec2 a, vec2 b);
vec2 operator-(vec2 a, vec2 b);
vec2 operator-(vec2 v);

} // namespace ving
