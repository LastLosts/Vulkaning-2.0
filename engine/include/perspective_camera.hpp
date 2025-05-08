#pragma once

#define GLM_FORCE_RADIANS

#include "glm/ext/matrix_float4x4.hpp"
#include "glm/ext/vector_float4.hpp"

namespace ving
{
class PerspectiveCamera
{
  public:
    PerspectiveCamera(float fov, float aspect, float near, float far);

    float move_speed{1.0f};
    glm::vec3 position{};
    float pitch{};
    float yaw{};

    void update();

    [[nodiscard]] glm::mat4 projection() const noexcept { return m_projection; }
    [[nodiscard]] glm::mat4 view() const noexcept { return m_view; }

    [[nodiscard]] glm::vec3 up() const noexcept { return m_up; }
    [[nodiscard]] glm::vec3 forward() const noexcept { return m_forward; }

  private:
    glm::vec3 m_up;
    glm::vec3 m_forward;

    glm::mat4 m_view;
    glm::mat4 m_projection;

    float m_fov;
    float m_aspect;
    float m_near;
    float m_far;
};
} // namespace ving
