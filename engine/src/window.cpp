#include "window.hpp"
#include "SDL3/SDL_init.h"
#include <SDL3/SDL_vulkan.h>
#include <stdexcept>

namespace ving
{
Window::Window(uint32_t width, uint32_t height)
{
    if (!SDL_Init(SDL_INIT_VIDEO))
        throw std::runtime_error("Failed to initialize SDL");
    m_window = SDL_CreateWindow("Window", width, height, SDL_WINDOW_VULKAN);
    if (!m_window)
        throw std::runtime_error("Failed to create SDL window");
}
[[nodiscard]] std::vector<const char *> Window::vulkan_extensions() const
{
    uint32_t count = 0;
    const char *const *window_extensions = SDL_Vulkan_GetInstanceExtensions(&count);

    std::vector<const char *> extensions{};
    extensions.reserve(count);

    for (size_t i = 0; i < count; ++i)
    {
        extensions.push_back(window_extensions[i]);
    }
    return extensions;
}
} // namespace ving
