#include "render_frames.hpp"
#include "utility/vulkan_utils.hpp"
#include <vulkan/vulkan_core.h>

namespace ving
{
RenderFrames::RenderFrames(const VulkanCore &core, const Window &window)
    : m_device{core.device()}, m_swapchain{core, window}
{
    for (auto &&frame : m_frames)
    {
        frame.command_pool = create_vulkan_command_pool(core.device(), core.graphics_queue_family(),
                                                        VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT);
        std::vector<VkCommandBuffer> buffers = allocate_vulkan_command_buffers(core.device(), frame.command_pool, 1);
        frame.commands = buffers[0];
        frame.image_acquired_semaphore = create_vulkan_semaphore(m_device);
        frame.render_finished_semaphore = create_vulkan_semaphore(m_device);
        frame.render_fence = create_vulkan_fence(m_device, true);
    }
}
RenderFrames::~RenderFrames()
{
    vkDeviceWaitIdle(m_device);
    for (auto &&frame : m_frames)
    {
        vkDestroyCommandPool(m_device, frame.command_pool, nullptr);
        vkDestroyFence(m_device, frame.render_fence, nullptr);
        vkDestroySemaphore(m_device, frame.image_acquired_semaphore, nullptr);
        vkDestroySemaphore(m_device, frame.render_finished_semaphore, nullptr);
    }
}
RenderFrames::FrameInfo RenderFrames::begin_frame()
{
    FrameResources &current_frame = m_frames[m_frame_number % frames_in_flight];

    vkWaitForFences(m_device, 1, &current_frame.render_fence, VK_TRUE, 1000000000);
    vkResetFences(m_device, 1, &current_frame.render_fence);

    uint32_t swapchain_image_index = m_swapchain.acquire_image(current_frame.image_acquired_semaphore);

    VkCommandBuffer cmd = current_frame.commands;

    vkResetCommandBuffer(cmd, 0);

    VkCommandBufferBeginInfo begin_info{};
    begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    begin_info.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

    vkBeginCommandBuffer(cmd, &begin_info);

    return {current_frame.commands};
}
void RenderFrames::end_frame()
{
    FrameResources &current_frame = m_frames[m_frame_number % frames_in_flight];

    ++m_frame_number;
}

} // namespace ving
