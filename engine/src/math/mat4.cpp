#include "math/mat4.hpp"

#include "math/vec_functions.hpp"

#include <cassert>

namespace ving
{
mat4::mat4(float s)
    : value{vec4{s, 0.0f, 0.0f, 0.0f}, vec4{0.0f, s, 0.0f, 0.0f}, vec4{0.0f, 0.0f, s, 0.0f}, vec4{0.0f, 0.0f, 0.0f, s}}
{
}
mat4::mat4(vec4 c1, vec4 c2, vec4 c3, vec4 c4) : value{c1, c2, c3, c4}
{
}
vec4 &mat4::operator[](int i)
{
    assert(i < 4 && i > -1);
    return value[i];
}
const vec4 &mat4::operator[](int i) const
{
    assert(i < 4 && i > -1);
    return value[i];
}

// TODO: if slow simd this bad boy
mat4 operator*(const mat4 &m1, const mat4 &m2)
{
    vec4 srcA0 = m1[0];
    vec4 srcA1 = m1[1];
    vec4 srcA2 = m1[2];
    vec4 srcA3 = m1[3];

    vec4 srcB0 = m2[0];
    vec4 srcB1 = m2[1];
    vec4 srcB2 = m2[2];
    vec4 srcB3 = m2[3];

    vec4 tmp0 = srcB0.x * srcA0;
    tmp0 = tmp0 + srcB0.y * srcA1;
    tmp0 = tmp0 + srcB0.z * srcA2;
    tmp0 = tmp0 + srcB0.w * srcA3;

    vec4 tmp1 = srcB1.x * srcA0;
    tmp1 = tmp1 + srcB1.y * srcA1;
    tmp1 = tmp1 + srcB1.z * srcA2;
    tmp1 = tmp1 + srcB1.w * srcA3;

    vec4 tmp2 = srcB2.x * srcA0;
    tmp2 = tmp2 + srcB2.y * srcA1;
    tmp2 = tmp2 + srcB2.z * srcA2;
    tmp2 = tmp2 + srcB2.w * srcA3;

    vec4 tmp3 = srcB3.x * srcA0;
    tmp3 = tmp3 + srcB3.y * srcA1;
    tmp3 = tmp3 + srcB3.z * srcA2;
    tmp3 = tmp3 + srcB3.w * srcA3;

    return mat4{tmp0, tmp1, tmp2, tmp3};
}

vec4 operator*(const vec4 &v, const mat4 &m)
{
    return vec4{dot(m[0], v), dot(m[1], v), dot(m[2], v), dot(m[3], v)};
}
} // namespace ving
