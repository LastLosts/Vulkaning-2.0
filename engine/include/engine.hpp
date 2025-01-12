#pragma once

#include "gpu_buffer.hpp"
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

    [[nodiscard]] bool running() const { return m_running; }
    [[nodiscard]] const VulkanCore &core() const { return m_core; }
    [[nodiscard]] const ImGuiRenderer &imgui_renderer() const { return m_imgui_renderer; }

    FrameInfo begin_frame();
    void end_frame(FrameInfo frame);

    bool load_shader(const char *path, VkShaderModule &out_shader) const;
    void copy_buffer_to_buffer_immediate(const GPUBuffer &source, const GPUBuffer &destination) const;

  private:
    bool m_running;

    VulkanInstance m_instance;
    Window m_window;
    VulkanCore m_core;
    RenderFrames m_render_frames;

    ImGuiRenderer m_imgui_renderer;

    // Time in seconds from the start of a program
    float m_time;
    // Delta time in seconds
    float m_delta_time;

    float m_record_commands_time;
};

} // namespace ving
