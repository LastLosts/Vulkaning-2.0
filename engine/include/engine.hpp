#pragma once

#include <chrono>

#include "window.hpp"

#include "gpu_buffer.hpp"
#include "imgui_renderer.hpp"

namespace ving
{
class Engine
{
  public:
    static constexpr uint32_t initial_window_width = 1280;
    static constexpr uint32_t initial_window_height = 720;

    Engine();

    [[nodiscard]] bool running() const noexcept { return m_running; }
    [[nodiscard]] float delta_time() const noexcept { return m_delta_time; }
    [[nodiscard]] float time() const noexcept { return m_time; }

    [[nodiscard]] const VulkanCore &core() const noexcept { return m_core; }
    [[nodiscard]] const ImGuiRenderer &imgui_renderer() const noexcept { return m_imgui_renderer; }

    FrameInfo begin_frame();
    void end_frame(FrameInfo frame);

    void begin_rendering(FrameInfo &frame, bool clear);
    void end_rendering(FrameInfo &frame);

    bool load_shader(const char *path, VkShaderModule &out_shader) const;
    void copy_buffer_to_buffer_immediate(const GPUBuffer &source, const GPUBuffer &destination) const;

  private:
    bool m_running;

    VulkanInstance m_instance;
    Window m_window;
    VulkanCore m_core;
    RenderFrames m_render_frames;

    ImGuiRenderer m_imgui_renderer;

    // Time in seconds from Engine creation
    float m_time;
    // Delta time in seconds
    float m_delta_time;

    std::chrono::high_resolution_clock::time_point m_engine_creation_time;
    std::chrono::high_resolution_clock::time_point m_frame_start_time;
    std::chrono::high_resolution_clock::time_point m_frame_end_time;

    float m_record_commands_time;
};

} // namespace ving
