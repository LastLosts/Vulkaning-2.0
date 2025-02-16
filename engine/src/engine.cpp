#include "engine.hpp"
#include "backends/imgui_impl_glfw.h"
#include "utility/vulkan_utils.hpp"

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

    m_running = !glfwWindowShouldClose(m_window.window());

    glfwPollEvents();

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

void Engine::begin_rendering(FrameInfo &frame, bool clear)
{
    VkCommandBuffer cmd = frame.cmd;
    Texture2D *draw_img = frame.draw_img;

    draw_img->transition_layout(cmd, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);

    VkClearValue clear_color{};
    clear_color.color = VkClearColorValue{0.1f, 0.1f, 0.1f, 0.0f};

    VkRenderingAttachmentInfo color_attachment{};
    color_attachment.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO;
    color_attachment.imageView = draw_img->view();
    color_attachment.imageLayout = draw_img->layout();

    color_attachment.loadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;

    if (clear)
    {
        color_attachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        color_attachment.clearValue = clear_color;
    }

    color_attachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;

    VkRenderingInfo render_info{};
    render_info.sType = VK_STRUCTURE_TYPE_RENDERING_INFO;
    render_info.renderArea = VkRect2D{{0, 0}, draw_img->extent()};
    render_info.layerCount = 1;
    render_info.colorAttachmentCount = 1;
    render_info.pColorAttachments = &color_attachment;

    vkCmdBeginRendering(cmd, &render_info);

    VkViewport viewport{};
    viewport.width = m_window.width();
    viewport.height = m_window.height();
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;

    VkRect2D scissor{};
    scissor.offset = {0, 0};
    scissor.extent = render_info.renderArea.extent;

    vkCmdSetViewport(cmd, 0, 1, &viewport);
    vkCmdSetScissor(cmd, 0, 1, &scissor);
}

void Engine::end_rendering(FrameInfo &frame)
{
    vkCmdEndRendering(frame.cmd);
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
