#include "math/mat4.hpp"

#include <cassert>

namespace ving
{
mat4::mat4(float s)
    : value{vec4{s, 0.0f, 0.0f, 0.0f}, vec4{0.0f, s, 0.0f, 0.0f}, vec4{0.0f, 0.0f, s, 0.0f}, vec4{0.0f, 0.0f, 0.0f, s}}
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
    mat4 result;
    for (int i = 0; i < 4; ++i)
    {
        for (int j = 0; j < 4; ++j)
        {
            float num = 0;
            for (int k = 0; k < 4; ++k)
            {
                num += m1[i][k] * m2[k][j];
            }
            result[i][j] = num;
        }
    }
    return result;
}
} // namespace ving
