#pragma once

#include "swapchain.hpp"
#include "texture_2d.hpp"
#include "vulkan_core.hpp"
#include <array>
#include <functional>
#include <vulkan/vulkan_core.h>

namespace ving
{
class RenderFrames final
{
  private:
    struct FrameResources
    {
        VkCommandPool command_pool;
        VkCommandBuffer commands;
        VkSemaphore image_acquired_semaphore, render_finished_semaphore;
        VkFence render_fence;
    };

  public:
    RenderFrames(const VulkanCore &core, const Window &window);
    ~RenderFrames();

    RenderFrames(const RenderFrames &) = delete;
    RenderFrames &operator=(const RenderFrames &) = delete;
    RenderFrames(RenderFrames &&) = delete;
    RenderFrames &operator=(RenderFrames &&) = delete;

    struct FrameInfo
    {
        Texture2D *draw_img;
        VkCommandBuffer cmd;
    };

    FrameInfo begin_frame();
    void end_frame();

    void immediate_submit(std::function<void(VkCommandBuffer cmd)> &&func);

  public:
    static constexpr int frames_in_flight = 2;
    std::array<FrameResources, frames_in_flight> m_frames;

    static constexpr VkFormat draw_image_format = VK_FORMAT_R16G16B16A16_SFLOAT;

    [[nodiscard]] VkQueue graphics_queue() const noexcept { return m_graphics_queue; }

  private:
    uint64_t m_frame_number;
    Texture2D m_draw_image;

    VkQueue m_graphics_queue;

    VkDevice m_device;
    uint32_t m_acquired_image_index;
    Swapchain m_swapchain;

    VkCommandPool m_immediate_pool;
    VkCommandBuffer m_immediate_commands;
    VkFence m_immediate_fence;
};
} // namespace ving
