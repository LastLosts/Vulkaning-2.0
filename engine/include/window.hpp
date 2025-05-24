#pragma once

#include <vulkan/vulkan_core.h>

#include "GLFW/glfw3.h"

namespace ving
{

class Window
{
  public:
    Window(uint32_t width, uint32_t height, bool resizable = false);
    ~Window();

    Window(const Window &) = delete;
    Window &operator=(const Window &) = delete;
    Window(Window &&) = delete;
    Window &operator=(Window &&) = delete;

    [[nodiscard]] int get_key(int key);
    [[nodiscard]] VkSurfaceKHR create_vulkan_surface(VkInstance instance) const;

    [[nodiscard]] uint32_t height() const noexcept { return m_height; }
    [[nodiscard]] uint32_t width() const noexcept { return m_width; }
    [[nodiscard]] GLFWwindow *window() const { return m_window; }

  private:
    friend struct ResizeCallback;

    uint32_t m_width;
    uint32_t m_height;

    GLFWwindow *m_window;
};
} // namespace ving
