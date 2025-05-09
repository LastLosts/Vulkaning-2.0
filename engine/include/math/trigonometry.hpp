#pragma once

#include "constants.hpp"

namespace ving
{
inline float radians(float degrees)
{
    constexpr float PI_180 = PI / 180.0f;
    return degrees * PI_180;
}
} // namespace ving
