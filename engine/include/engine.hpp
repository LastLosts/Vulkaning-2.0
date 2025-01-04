#pragma once

#include "imgui_renderer.hpp"
#include "render_frames.hpp"
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

    ImGuiRenderer m_imgui_renderer;

    float m_time;
    float m_delta_time;
};

} // namespace ving
