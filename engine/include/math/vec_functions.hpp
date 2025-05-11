#pragma once

#include "math/vec3.hpp"
#include "math/vec4.hpp"

namespace ving
{
float dot(vec4 a, vec4 b);
vec3 cross(vec3 a, vec3 b);
vec3 normalize(vec3 v);
} // namespace ving
