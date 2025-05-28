#include "perspective_camera.hpp"
#include "math/matrix_transform.hpp"
#include "math/trigonometry.hpp"
#include <algorithm>

#include <cmath>

namespace ving
{
PerspectiveCamera::PerspectiveCamera(float fov, float aspect, float near, float far)
    : m_aspect{aspect}, m_fov{fov}, m_near{near}, m_far{far}, m_up{0.0f, 1.0f, 0.0f}, m_forward{0.0f, 0.0f, 1.0f},
      m_view{1.0f}, m_projection{1.0f}
{
}
void PerspectiveCamera::update()
{
    pitch = std::min(std::max(pitch, -89.0f), 89.0f);
    mat4 rot{1.0f};

    float c1 = cos(radians(-yaw)), c2 = cos(radians(pitch)), c3 = cos(radians(0));
    float s1 = sin(radians(-yaw)), s2 = sin(radians(pitch)), s3 = sin(radians(0));

    float ca = cos(radians(yaw)), cb = cos(radians(pitch)), cy = cos(radians(0));
    float sa = sin(radians(yaw)), sb = sin(radians(pitch)), sy = sin(radians(0));

    // YXZ Rotation from wikipedia
    rot[0][0] = c1 * c3 + s1 * s2 * s3;
    rot[0][1] = c2 * s3;
    rot[0][2] = c1 * s2 * s3 - c3 * s1;

    rot[1][0] = c3 * s1 * s2 - c1 * s3;
    rot[1][1] = c2 * c3;
    rot[1][2] = ca * cy * sb + sa * sy;

    rot[2][0] = c2 * s1;
    rot[2][1] = -s2;
    rot[2][2] = c1 * c2;

    m_forward = vec3{rot[2].x, rot[2].y, rot[2].z};
    m_up = {rot[1].x, rot[1].y, rot[1].z};

    rot = transpose(rot);

    mat4 trans{1.0f};

    trans[3] = (-position.x * trans[0]) + (-position.y * trans[1]) + (-position.z * trans[2]) + trans[3];
    // trans[3] = vec4{-position.x, -position.y, -position.z, 1.0f};

    m_view = rot * trans;

    m_projection = perspective(m_fov, m_aspect, m_near, m_far);
}
} // namespace ving
