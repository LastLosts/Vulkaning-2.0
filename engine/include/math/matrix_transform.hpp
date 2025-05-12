#pragma once

#include "math/mat4.hpp"
#include <cmath>

namespace ving
{
mat4 transpose(const mat4 &m);
mat4 ortho(float left, float right, float bottom, float top, float near, float far);
mat4 ortho(float left, float right, float bottom, float top);
mat4 perspectiveA(float fovy, float aspect, float near, float far);
mat4 perspective(float fovy, float aspect, float near, float far);
} // namespace ving
