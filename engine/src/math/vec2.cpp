#include "math/vec2.hpp"

namespace ving
{
void vec2::operator+=(vec2 b)
{
    *this = *this + b;
}
vec2 operator*(float s, vec2 v)
{
    vec2 result;

    result.x = s * v.x;
    result.y = s * v.y;

    return result;
}

vec2 operator+(vec2 a, vec2 b)
{
    vec2 result;

    result.x = a.x + b.x;
    result.y = a.y + b.y;

    return result;
}
vec2 operator-(vec2 a, vec2 b)
{
    vec2 result;

    result.x = a.x - b.x;
    result.y = a.y - b.y;

    return result;
}
vec2 operator-(vec2 v)
{
    vec2 result;

    result.x = -v.x;
    result.y = -v.y;

    return result;
}
} // namespace ving
