#include "perspective_camera.hpp"
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

    float theta1 = 0.0f; // roll
    float theta2 = pitch;
    float theta3 = yaw;

    float c1 = 1.0f, c2 = cos(pitch), c3 = cos(yaw);
    float s1 = 0.0f, s2 = sin(pitch), s3 = sin(yaw);

    // YXZ Rotation from wikipedia
    rot[0][0] = c3 * c1 - s3 * s2 * s1;
    rot[0][1] = s3 * c1 + c3 * s2 * s1;
    rot[0][2] = -c2 * s1;
    rot[1][0] = -s3 * c2;
    rot[1][1] = c3 * c2;
    rot[1][2] = s2;
    rot[2][0] = c3 * s1 + s3 * s2 * c1;
    rot[2][1] = s3 * s1 - c3 * s2 * c1;
    rot[2][2] = c2 * c1;

    // rot = rotate(rot, radians(yaw), {0.0f, 1.0f, 0.0f});
    // rot = rotate(rot, radians(pitch), {1.0f, 0.0f, 0.0f});

    m_forward = {rot[2].x, rot[2].y, rot[2].z};
    m_up = {rot[1].x, rot[1].y, rot[1].z};

    // rot = transpose(rot);

    mat4 trans{1.0f};

    // trans = translate(trans, -position);

    trans[3].x = -position.x;
    trans[3].y = -position.y;
    trans[3].z = -position.z;

    m_view = rot * trans;

    // m_projection = perspective(m_fov, m_aspect, m_near, m_far);
}
} // namespace ving
