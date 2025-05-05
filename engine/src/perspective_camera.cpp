#include "perspective_camera.hpp"
#include <algorithm>

namespace ving
{
PerspectiveCamera::PerspectiveCamera(float fov, float aspect, float near, float far)
    : m_aspect{aspect}, m_fov{fov}, m_near{near}, m_far{far}, m_up{0.0f, 1.0f, 0.0f}, m_forward{0.0f, 0.0f, 1.0f},
      m_view{1.0f}
{
}
void PerspectiveCamera::update()
{
    pitch = std::min(std::max(pitch, -89.0f), 89.0f);
    mat4 rot{1.0f};

    rot = rotate(rot, glm::radians(yaw), {0.0f, 1.0f, 0.0f});
    rot = rotate(rot, glm::radians(pitch), {1.0f, 0.0f, 0.0f});

    m_forward = rot[2];
    m_up = rot[1];

    rot = transpose(rot);

    mat4 trans{1.0f};
    trans = translate(trans, -position);

    m_view = rot * trans;
    m_projection = perspective(m_fov, m_aspect, m_near, m_far);
}
} // namespace ving
