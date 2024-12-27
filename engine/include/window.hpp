#pragma once

#include "SDL3/SDL_video.h"
#include <vector>

namespace ving
{
class Window
{
  public:
    Window(uint32_t width, uint32_t height);

    [[nodiscard]] std::vector<const char *> vulkan_extensions() const;

    [[nodiscard]] uint32_t height() const noexcept { return m_height; }
    [[nodiscard]] uint32_t width() const noexcept { return m_width; }

  private:
    uint32_t m_height;
    uint32_t m_width;

    SDL_Window *m_window;
};
} // namespace ving
