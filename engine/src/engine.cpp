#include "engine.hpp"

#include "utility/vulkan_utils.hpp"

#include "math/common.h"

namespace ving
{
Engine::Engine(const Window &window)
    : m_core{window}, m_render_frames{m_core, {initial_window_width, initial_window_height}},
      m_imgui_renderer{m_core, window, m_render_frames}, m_delta_time{0.016f}, m_time{0.0f}, m_cursor_x{0.0f},
      m_cursor_y{0.0f}
{
    m_engine_creation_time = std::chrono::high_resolution_clock::now();
}

FrameInfo Engine::begin_frame(VkExtent2D window_resolution)
{
    m_frame_start_time = std::chrono::high_resolution_clock::now();

    glfwPollEvents();
    // glfwGetCursorPos(m_window.window(), &m_cursor_x, &m_cursor_y);

    m_cursor_x = clamp(m_cursor_x, 0.0, (double)window_resolution.width);
    m_cursor_y = clamp(m_cursor_y, 0.0, (double)window_resolution.height);

    FrameInfo frame = m_render_frames.begin_frame(window_resolution);

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

void Engine::begin_rendering(FrameInfo &frame, bool clear, VkExtent2D viewport_resoulution)
{
    VkCommandBuffer cmd = frame.cmd;
    Texture2D *draw_img = frame.draw_img;
    Texture2D *depth_img = frame.depth_img;

    draw_img->transition_layout(cmd, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);
    depth_img->transition_layout(cmd, VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL);

    VkClearValue clear_color{};
    clear_color.color = VkClearColorValue{0.1f, 0.1f, 0.1f, 1.0f};

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

    VkRenderingAttachmentInfo depth_attachment{};
    depth_attachment.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO;
    depth_attachment.imageView = depth_img->view();
    depth_attachment.imageLayout = depth_img->layout();
    depth_attachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    depth_attachment.clearValue.depthStencil.depth = 1.0f;
    depth_attachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;

    VkRenderingInfo render_info{};
    render_info.sType = VK_STRUCTURE_TYPE_RENDERING_INFO;
    render_info.renderArea = VkRect2D{{0, 0}, draw_img->extent()};
    render_info.layerCount = 1;
    render_info.colorAttachmentCount = 1;
    render_info.pColorAttachments = &color_attachment;
    render_info.pDepthAttachment = &depth_attachment;

    vkCmdBeginRendering(cmd, &render_info);

    VkViewport viewport{};
    viewport.width = viewport_resoulution.width;
    viewport.height = viewport_resoulution.height;
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
