#include "math/matrix_transform.hpp"

#include <cmath>

namespace ving
{
mat4 transpose(const mat4 &m)
{
    mat4 result{1.0f};

    result[0][0] = m[0][0];
    result[0][1] = m[1][0];
    result[0][2] = m[2][0];
    result[0][3] = m[3][0];

    result[1][0] = m[0][1];
    result[1][1] = m[1][1];
    result[1][2] = m[2][1];
    result[1][3] = m[3][1];

    result[2][0] = m[0][2];
    result[2][1] = m[1][2];
    result[2][2] = m[2][2];
    result[2][3] = m[3][2];

    result[3][0] = m[0][3];
    result[3][1] = m[1][3];
    result[3][2] = m[2][3];
    result[3][3] = m[3][3];

    return result;
}
mat4 ortho(float left, float right, float bottom, float top, float near, float far)
{
    mat4 result{1.0f};
    result[0][0] = 2.0f / (right - left);
    result[1][1] = 2.0f / (top - bottom);
    result[2][2] = 1.0f / (far - near);
    result[3][0] = -(right + left) / (right - left);
    result[3][1] = -(top + bottom) / (top - bottom);
    result[3][2] = -near / (far - near);
    return result;
}

mat4 perspectiveA(float fovy, float aspect, float near, float far)
{
    mat4 result{1.0f};

    const float tan_half_fovy = tan(fovy / 2.0f);

    result[0][0] = 1.0f / (aspect * tan_half_fovy);
    result[1][1] = 1.0f / tan_half_fovy;
    result[2][2] = far / (far - near);
    result[2][3] = 1.0f;
    result[3][2] = -(far * near) / (far - near);

    return result;
}

mat4 perspective(float fovy, float aspect, float near, float far)
{
    mat4 result{};

    const float tan_half_fovy = tan(fovy / 2.0f);

    result[0][0] = 1.0f / (aspect * tan_half_fovy);
    result[1][1] = 1.0f / tan_half_fovy;
    result[2][2] = far / (far - near);
    result[2][3] = 1.0f;
    result[3][2] = -(far * near) / (far - near);

    return result;
}
} // namespace ving
