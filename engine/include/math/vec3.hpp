#pragma once

namespace ving
{
struct vec3
{
    union {
        struct
        {
            float x, y, z;
        };
        float e[3];
    };
    inline void operator+=(vec3 b);
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

} // namespace ving
