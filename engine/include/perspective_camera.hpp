#pragma once

#define GLM_FORCE_RADIANS

#include "math/mat4.hpp"
#include "math/vec3.hpp"

namespace ving
{
class PerspectiveCamera
{
  public:
    PerspectiveCamera(float fov, float aspect, float near, float far);

    float move_speed{1.0f};
    vec3 position{};
    float pitch{};
    float yaw{};

    void update();

    [[nodiscard]] mat4 projection() const noexcept { return m_projection; }
    [[nodiscard]] mat4 view() const noexcept { return m_view; }

    [[nodiscard]] vec3 up() const noexcept { return m_up; }
    [[nodiscard]] vec3 forward() const noexcept { return m_forward; }

  private:
    vec3 m_up;
    vec3 m_forward;

    mat4 m_view;
    mat4 m_projection;

    float m_fov;
    float m_aspect;
    float m_near;
    float m_far;
};
} // namespace ving
