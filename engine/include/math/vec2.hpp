#pragma once

namespace ving
{
struct vec2
{
    vec2() : x{0.0f}, y{0.0f} {}
    vec2(float _x, float _y) : x{_x}, y{_y} {}

    union {
        struct
        {
            float x, y;
        };
        float d[2];
    };
};
} // namespace ving
