#pragma once

#include "swapchain.hpp"
#include "vulkan_core.hpp"
#include <array>
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
        VkCommandBuffer cmd;
    };

    FrameInfo begin_frame();
    void end_frame();

  public:
    static constexpr int frames_in_flight = 2;
    std::array<FrameResources, frames_in_flight> m_frames;

  private:
    uint64_t m_frame_number;

    VkQueue m_graphics_queue;

    VkDevice m_device;
    uint32_t m_acquired_image_index;
    Swapchain m_swapchain;
};
} // namespace ving
