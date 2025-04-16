#include "perspective_camera.hpp"
#include "glm/ext/matrix_transform.hpp"

namespace ving
{
PerspectiveCamera::PerspectiveCamera(float fov, float aspect, float near, float far)
    : m_aspect{aspect}, m_fov{fov}, m_near{near}, m_far{far}, m_forward{0.0f}, m_view{1.0f}
{
}
void PerspectiveCamera::update()
{
    glm::mat4 rot{1.0f};
    rot = glm::rotate(rot, m_yaw, {0.0f, 1.0f, 0.0f});
    rot = glm::rotate(rot, m_pitch, {1.0f, 0.0f, 0.0f});

    m_forward = rot[2];
    m_up = rot[1];

    rot = glm::transpose(rot);

    glm::mat4 trans{1.0f};
    trans = glm::translate(trans, -position);

    m_view = rot * trans;
}
} // namespace ving
