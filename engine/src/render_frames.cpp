#include "render_frames.hpp"
#include "utility/vulkan_utils.hpp"
#include <iostream>
#include <vulkan/vulkan_core.h>

#include <chrono>

namespace ving
{
RenderFrames::RenderFrames(const VulkanCore &core, const Window &window)
    : m_device{core.device()}, m_swapchain{core, window},
      m_draw_image{core, VkExtent2D(window.width(), window.height()),
                   VkImageUsageFlags(VK_IMAGE_USAGE_STORAGE_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT |
                                     VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT),
                   VMA_MEMORY_USAGE_GPU_ONLY, draw_image_format}
{
    vkGetDeviceQueue(m_device, core.graphics_queue_family(), 0, &m_graphics_queue);

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

    m_immediate_pool = create_vulkan_command_pool(core.device(), core.graphics_queue_family(),
                                                  VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT);
    std::vector<VkCommandBuffer> buffers = allocate_vulkan_command_buffers(core.device(), m_immediate_pool, 1);
    m_immediate_commands = buffers[0];
    m_immediate_fence = create_vulkan_fence(core.device(), false);
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

    vkDestroyCommandPool(m_device, m_immediate_pool, nullptr);
    vkDestroyFence(m_device, m_immediate_fence, nullptr);
}
FrameInfo RenderFrames::begin_frame()
{
    FrameResources &current_frame = m_frames[m_frame_number % frames_in_flight];
    VkCommandBuffer cmd = current_frame.commands;

    vkWaitForFences(m_device, 1, &current_frame.render_fence, VK_TRUE, 1000000000);
    vkResetFences(m_device, 1, &current_frame.render_fence);

    m_acquired_image_index = m_swapchain.acquire_image(current_frame.image_acquired_semaphore);

    vkResetCommandBuffer(cmd, 0);

    VkCommandBufferBeginInfo begin_info{};
    begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    begin_info.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

    vkBeginCommandBuffer(cmd, &begin_info);

    return FrameInfo{&m_draw_image, current_frame.commands};
}
void RenderFrames::end_frame()
{
    FrameResources &current_frame = m_frames[m_frame_number % frames_in_flight];
    VkCommandBuffer cmd = current_frame.commands;

    m_swapchain.copy_image_to_swapchain(cmd, m_draw_image.image(), m_draw_image.extent(), m_acquired_image_index);
    m_swapchain.transition_swapchain_image_to_present(cmd, m_acquired_image_index);

    vkEndCommandBuffer(cmd);

    VkCommandBufferSubmitInfo cmd_info{};
    cmd_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_SUBMIT_INFO;
    cmd_info.commandBuffer = cmd;

    VkSemaphoreSubmitInfo signal_info{};
    signal_info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO;
    signal_info.semaphore = current_frame.render_finished_semaphore;
    signal_info.stageMask = VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT;
    signal_info.value = 1;

    VkSemaphoreSubmitInfo wait_info{};
    wait_info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO;
    wait_info.semaphore = current_frame.image_acquired_semaphore;
    wait_info.stageMask = VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT;
    wait_info.value = 1;

    VkSubmitInfo2 submit{};
    submit.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO_2;
    submit.signalSemaphoreInfoCount = 1;
    submit.pSignalSemaphoreInfos = &signal_info;
    submit.waitSemaphoreInfoCount = 1;
    submit.pWaitSemaphoreInfos = &wait_info;
    submit.commandBufferInfoCount = 1;
    submit.pCommandBufferInfos = &cmd_info;

    vkQueueSubmit2(m_graphics_queue, 1, &submit, current_frame.render_fence);
    m_swapchain.present_image(current_frame.render_finished_semaphore, m_acquired_image_index);

    ++m_frame_number;
}

void RenderFrames::immediate_submit(std::function<void(VkCommandBuffer cmd)> &&func)
{
    VkCommandBuffer cmd = m_immediate_commands;

    vkResetFences(m_device, 1, &m_immediate_fence);
    vkResetCommandBuffer(cmd, 0);

    VkCommandBufferBeginInfo begin_info{};
    begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    begin_info.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

    vkBeginCommandBuffer(cmd, &begin_info);
    func(cmd);
    vkEndCommandBuffer(cmd);

    VkCommandBufferSubmitInfo cmd_info{};
    cmd_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_SUBMIT_INFO;
    cmd_info.commandBuffer = cmd;

    VkSubmitInfo2 submit{};
    submit.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO_2;
    submit.commandBufferInfoCount = 1;
    submit.pCommandBufferInfos = &cmd_info;

    vkQueueSubmit2(m_graphics_queue, 1, &submit, m_immediate_fence);

    vkWaitForFences(m_device, 1, &m_immediate_fence, VK_TRUE, 1000000000);
}
} // namespace ving
