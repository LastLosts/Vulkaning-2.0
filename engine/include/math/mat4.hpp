#pragma once

#include "math/vec4.hpp"

namespace ving
{
struct mat4
{
    mat4() = default;
    explicit mat4(float s);
    mat4(vec4 c1, vec4 c2, vec4 c3, vec4 c4);

    [[nodiscard]] vec4 &operator[](int i);
    [[nodiscard]] const vec4 &operator[](int i) const;

    vec4 value[4];
};

[[nodiscard]] mat4 operator*(const mat4 &m1, const mat4 &m2);
[[nodiscard]] vec4 operator*(const vec4 &v, const mat4 &m);
} // namespace ving
