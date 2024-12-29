#include "window.hpp"
#include "SDL3/SDL_init.h"
#include <SDL3/SDL_vulkan.h>
#include <stdexcept>
#include <vulkan/vulkan_core.h>

namespace ving
{
Window::Window(const VulkanInstance &instance, uint32_t width, uint32_t height)
    : m_instance{instance.instance}, m_width{width}, m_height{height}
{
    m_window = SDL_CreateWindow("Window", width, height, SDL_WINDOW_VULKAN);
    if (!m_window)
        throw std::runtime_error("Failed to create SDL window");
    if (!SDL_Vulkan_CreateSurface(m_window, m_instance, nullptr, &m_surface))
        throw std::runtime_error("Failed to create window surface");
}
Window::~Window()
{
    vkDestroySurfaceKHR(m_instance, m_surface, nullptr);
    SDL_DestroyWindow(m_window);
}
} // namespace ving
