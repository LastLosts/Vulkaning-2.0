#include "engine.hpp"
#include "SDL3/SDL_events.h"
#include <vulkan/vulkan_core.h>

#include <chrono>

namespace ving
{

Engine::Engine()
    : m_window{m_instance, 400, 400}, m_core{m_instance, m_window}, m_render_frames{m_core, m_window},
      m_imgui_renderer{m_core, m_window, m_render_frames}
{
}
void Engine::run()
{
    m_running = true;
}
void Engine::update()
{
    auto start_update_time = std::chrono::high_resolution_clock::now();

    SDL_Event event;

    while (SDL_PollEvent(&event))
    {
        if (event.type == SDL_EVENT_QUIT)
            m_running = false;
    }
    RenderFrames::FrameInfo frame = m_render_frames.begin_frame();
    VkCommandBuffer cmd = frame.cmd;
    Texture2D &draw_img = *frame.draw_img;

    draw_img.transition_layout(cmd, VK_IMAGE_LAYOUT_GENERAL);

    VkClearColorValue clear_val = {{0.0f, 0.0f, (std::sin(m_time * 0.001f) + 1.0f) / 2.0f, 1.0f}};

    VkImageSubresourceRange subresource_range{};
    subresource_range.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    subresource_range.baseMipLevel = 0;
    subresource_range.levelCount = VK_REMAINING_MIP_LEVELS;
    subresource_range.baseArrayLayer = 0;
    subresource_range.layerCount = VK_REMAINING_ARRAY_LAYERS;

    vkCmdClearColorImage(cmd, draw_img.image(), draw_img.layout(), &clear_val, 1, &subresource_range);
    draw_img.transition_layout(cmd, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL);

    m_render_frames.end_frame();

    auto end_update_time = std::chrono::high_resolution_clock::now();

    std::chrono::duration<float> delta = end_update_time - start_update_time;
    m_delta_time = delta.count() * 1000.0f;
    m_time += m_delta_time;
}

} // namespace ving
