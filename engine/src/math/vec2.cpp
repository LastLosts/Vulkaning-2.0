#include "math/vec2.hpp"

namespace ving
{
inline vec2 operator*(float a, vec2 b)
{
    vec2 result;

    result.x = a * b.x;
    result.y = a * b.y;

    return result;
}

inline vec2 operator+(vec2 a, vec2 b)
{
    vec2 result;

    result.x = a.x + b.x;
    result.y = a.y + b.y;

    return result;
}
inline vec2 operator-(vec2 a, vec2 b)
{
    vec2 result;

    result.x = a.x - b.x;
    result.y = a.y - b.y;

    return result;
}
inline vec2 operator-(vec2 v)
{
    vec2 result;

    result.x = -v.x;
    result.y = -v.y;

    return result;
}
} // namespace ving
