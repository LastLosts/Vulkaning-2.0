#pragma once

#include "glm/ext/matrix_float4x4.hpp"
#include <ostream>

inline std::ostream &operator<<(std::ostream &os, const glm::mat4 &m)
{
    // clang-format off
    os << m[0][0] << ' ' << m[1][0] << ' ' << m[2][0] << ' ' << m[3][0] << '\n'
       << m[0][1] << ' ' << m[1][1] << ' ' << m[2][1] << ' ' << m[3][1] << '\n'
       << m[0][2] << ' ' << m[1][2] << ' ' << m[2][2] << ' ' << m[3][2] << '\n'
       << m[0][3] << ' ' << m[1][3] << ' ' << m[2][3] << ' ' << m[3][3] << std::endl;
    // clang-format on
    return os;
}
