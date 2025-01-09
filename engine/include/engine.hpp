#pragma once

#include "imgui_renderer.hpp"
#include "render_frames.hpp"
#include "slime_renderer.hpp"
#include "vulkan_core.hpp"
#include "vulkan_instance.hpp"
#include "window.hpp"

namespace ving
{
class Engine
{
  public:
    Engine();

    [[nodiscard]] bool running() { return m_running; }
    void run();
    void update();

  private:
    bool m_running;

    VulkanInstance m_instance;
    Window m_window;
    VulkanCore m_core;
    RenderFrames m_render_frames;

    SlimeRenderer m_slime_renderer;
    ImGuiRenderer m_imgui_renderer;
    
    // Time in seconds from the start of a program
    float m_time;
    // Delta time in seconds
    float m_delta_time;
};

} // namespace ving
