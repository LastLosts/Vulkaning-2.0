#pragma once

#include "vulkan_instance.hpp"

#include "GLFW/glfw3.h"

namespace ving
{
class Window
{
  public:
    Window(const VulkanInstance &instance, uint32_t width, uint32_t height);
    ~Window();

    Window(const Window &) = delete;
    Window &operator=(const Window &) = delete;
    Window(Window &&) = delete;
    Window &operator=(Window &&) = delete;

    void setup_surface(VkInstance instance);

    [[nodiscard]] uint32_t height() const noexcept { return m_height; }
    [[nodiscard]] uint32_t width() const noexcept { return m_width; }
    [[nodiscard]] VkSurfaceKHR vulkan_surface() const { return m_surface; }
    [[nodiscard]] GLFWwindow *window() const { return m_window; }

  private:
    VkInstance m_instance;

    uint32_t m_width;
    uint32_t m_height;

    GLFWwindow *m_window;

    VkSurfaceKHR m_surface;
};
} // namespace ving
