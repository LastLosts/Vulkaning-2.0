#pragma once

#include "constants.hpp"

namespace ving
{
inline float radians(float degrees)
{
    return (degrees / 90.0f) * PI;
}
} // namespace ving
