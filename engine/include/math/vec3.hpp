#pragma once

namespace ving
{
struct vec3
{
    vec3() : x{0.0f}, y{0.0f}, z{0.0f} {}
    explicit vec3(float _s) : x{_s}, y{_s}, z{_s} {}
    vec3(float _x, float _y, float _z) : x{_x}, y{_y}, z{_z} {}

    union {
        struct
        {
            float x, y, z;
        };
    };
    void operator+=(vec3 b);
    void operator-=(vec3 b);
};

inline vec3 operator*(float a, vec3 b)
{
    vec3 result;

    result.x = a * b.x;
    result.y = a * b.y;
    result.z = a * b.z;

    return result;
}
inline vec3 operator*(vec3 a, float b)
{
    vec3 result;

    result.x = b * a.x;
    result.y = b * a.y;
    result.z = b * a.z;

    return result;
}

inline vec3 operator+(vec3 a, vec3 b)
{
    vec3 result;

    result.x = a.x + b.x;
    result.y = a.y + b.y;
    result.z = a.z + b.z;

    return result;
}

inline vec3 operator-(vec3 a, vec3 b)
{
    vec3 result;

    result.x = a.x - b.x;
    result.y = a.y - b.y;
    result.z = a.z - b.z;

    return result;
}
inline vec3 operator-(vec3 v)
{
    vec3 result;

    result.x = -v.x;
    result.y = -v.y;
    result.z = -v.z;

    return result;
}

inline bool operator==(vec3 a, vec3 b)
{
    return a.x == b.x && a.y == b.y && a.z == b.z;
}
inline bool operator!=(vec3 a, vec3 b)
{
    return !(a == b);
}

} // namespace ving
