#pragma once

#include "glm/common.hpp"
#include "glm/ext/matrix_clip_space.hpp"
#include "glm/mat4x4.hpp"
#include "glm/vec3.hpp"

namespace ving
{
class PerspectiveCamera
{
  public:
    PerspectiveCamera(float fov, float aspect, float near, float far);

    float move_speed{1.0f};
    glm::vec3 position{};

    [[nodiscard]] glm::mat4 projection() const noexcept { return glm::perspective(m_fov, m_aspect, m_near, m_far); }
    [[nodiscard]] glm::mat4 view() noexcept;

    [[nodiscard]] glm::vec3 up() const noexcept { return m_up; }
    [[nodiscard]] glm::vec3 forward() const noexcept { return m_forward; }

    [[nodiscard]] float yaw() const noexcept { return m_yaw; }

    void set_yaw(float yaw) noexcept { m_yaw = yaw; }
    void set_pitch(float pitch) noexcept
    {
        pitch = glm::clamp(pitch, -89.0f, 89.0f);
        m_pitch = pitch;
    }

  private:
    glm::vec3 m_up{0.0f, 1.0f, 0.0f};
    glm::vec3 m_forward;

    float m_fov;
    float m_aspect;
    float m_near;
    float m_far;

    float m_pitch{};
    float m_yaw{};
};
} // namespace ving
