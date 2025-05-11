#pragma once

#include "math/mat4.hpp"

#include <iomanip>
#include <ostream>

inline std::ostream &operator<<(std::ostream &os, const ving::mat4 &m)
{
    // clang-format off
    os << std::setw(10) << m[0][0] << ' ' << std::setw(10) << m[1][0] << ' ' << std::setw(10) << m[2][0] << ' ' << std::setw(10) << m[3][0] << '\n'
       << std::setw(10) << m[0][1] << ' ' << std::setw(10) << m[1][1] << ' ' << std::setw(10) << m[2][1] << ' ' << std::setw(10) << m[3][1] << '\n'
       << std::setw(10) << m[0][2] << ' ' << std::setw(10) << m[1][2] << ' ' << std::setw(10) << m[2][2] << ' ' << std::setw(10) << m[3][2] << '\n'
       << std::setw(10) << m[0][3] << ' ' << std::setw(10) << m[1][3] << ' ' << std::setw(10) << m[2][3] << ' ' << std::setw(10) << m[3][3] << "\n\n";
    // clang-format on
    return os;
}

inline std::ostream &operator<<(std::ostream &os, const ving::vec4 &v)
{
    os << v.x << ' ' << v.y << ' ' << v.z << ' ' << v.w << '\n';
    return os;
}
