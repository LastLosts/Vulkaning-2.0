#pragma once

#include "swapchain.hpp"
#include "texture_2d.hpp"
#include "vulkan_core.hpp"
#include <functional>
#include <vulkan/vulkan_core.h>

namespace ving
{
struct FrameInfo
{
    Texture2D *draw_img;
    Texture2D *depth_img;
    VkCommandBuffer cmd;
};

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
    RenderFrames(const VulkanCore &core, VkExtent2D render_resolution);
    ~RenderFrames();

    RenderFrames(const RenderFrames &) = delete;
    RenderFrames &operator=(const RenderFrames &) = delete;
    RenderFrames(RenderFrames &&) = delete;
    RenderFrames &operator=(RenderFrames &&) = delete;

    FrameInfo begin_frame(VkExtent2D render_resolution);
    void end_frame();

    void immediate_submit(std::function<void(VkCommandBuffer cmd)> &&func);

  public:
    static constexpr VkFormat draw_image_format = VK_FORMAT_R16G16B16A16_SFLOAT;
    static constexpr VkFormat depth_image_format = VK_FORMAT_D32_SFLOAT;

    [[nodiscard]] VkQueue graphics_queue() const noexcept { return m_graphics_queue; }

  private:
    uint32_t m_frames_in_flight;
    std::vector<FrameResources> m_frames;

    uint64_t m_frame_number;
    Texture2D m_draw_image;
    Texture2D m_depth_image;

    VkQueue m_graphics_queue;

    VkDevice m_device;
    uint32_t m_acquired_image_index;
    Swapchain m_swapchain;

    VkCommandPool m_immediate_pool;
    VkCommandBuffer m_immediate_commands;
    VkFence m_immediate_fence;

    bool m_resize_requested;
};
} // namespace ving
