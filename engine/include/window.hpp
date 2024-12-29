#pragma once

#include "SDL3/SDL_video.h"
#include "vulkan_instance.hpp"
#include <vulkan/vulkan_core.h>

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

  private:
    VkInstance m_instance;

    uint32_t m_width;
    uint32_t m_height;

    SDL_Window *m_window;

    VkSurfaceKHR m_surface;
};
} // namespace ving
