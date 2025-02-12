#include "engine.hpp"
#include "SDL3/SDL_events.h"
#include "backends/imgui_impl_sdl3.h"
#include "imgui.h"
#include "utility/vulkan_utils.hpp"
#include <algorithm>
#include <iostream>
#include <vulkan/vulkan_core.h>

namespace ving
{

Engine::Engine()
    : m_window{m_instance, 1280, 720}, m_core{m_instance, m_window}, m_render_frames{m_core, m_window},
      m_imgui_renderer{m_core, m_window, m_render_frames}, m_delta_time{0.016f}, m_time{0.0f}
{
    m_engine_creation_time = std::chrono::high_resolution_clock::now();
    m_running = true;
}

FrameInfo Engine::begin_frame()
{
    // TODO: Add begin rendering
    m_frame_start_time = std::chrono::high_resolution_clock::now();

    SDL_Event event;

    m_keys_pressed.clear();

    while (SDL_PollEvent(&event))
    {
        switch (event.type)
        {
        case SDL_EVENT_QUIT: {
            m_running = false;
            break;
        }
        case SDL_EVENT_KEY_DOWN: {
            m_keys_pressed.push_back(event.key.key);
            break;
        }
        case SDL_EVENT_KEY_UP: {
            break;
        }
        }

        ImGui_ImplSDL3_ProcessEvent(&event);
    }
    FrameInfo frame = m_render_frames.begin_frame();

    return frame;
}

void Engine::end_frame(FrameInfo frame)
{
    frame.draw_img->transition_layout(frame.cmd, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL);
    m_render_frames.end_frame();

    m_frame_end_time = std::chrono::high_resolution_clock::now();

    std::chrono::duration<float> duration = (m_frame_end_time - m_frame_start_time);
    std::chrono::duration<float> engine_duration = (m_frame_end_time - m_engine_creation_time);

    m_delta_time = duration.count();
    m_time = engine_duration.count();
}

bool Engine::load_shader(const char *path, VkShaderModule &out_shader) const
{
    return load_vulkan_shader_module(m_core.device(), path, out_shader);
}

void Engine::copy_buffer_to_buffer_immediate(const GPUBuffer &source, const GPUBuffer &destination) const
{
    m_core.immediate_transfer([&source, &destination](VkCommandBuffer cmd) {
        VkBufferCopy copy{};
        copy.size = source.size();
        copy.dstOffset = 0;
        copy.srcOffset = 0;

        vkCmdCopyBuffer(cmd, source.buffer(), destination.buffer(), 1, &copy);
    });
}
} // namespace ving
