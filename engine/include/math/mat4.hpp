#pragma once

#include "math/vec4.hpp"

namespace ving
{
struct mat4
{
    mat4() = default;
    explicit mat4(float s);

    [[nodiscard]] vec4 &operator[](int i);
    [[nodiscard]] const vec4 &operator[](int i) const;

    vec4 value[4];
};

[[nodiscard]] mat4 operator*(const mat4 &m1, const mat4 &m2);
} // namespace ving
