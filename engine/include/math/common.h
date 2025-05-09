#pragma once

namespace ving
{
inline float min(float x, float y)
{
    return (y < x) ? y : x;
}
inline float max(float x, float y)
{
    return (x < y) ? y : x;
}

inline float clamp(float x, float min_value, float max_value)
{
    return min(max(x, min_value), max_value);
}
} // namespace ving
