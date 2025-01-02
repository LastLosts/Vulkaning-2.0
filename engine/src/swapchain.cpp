#include "swapchain.hpp"
#include "render_frames.hpp"

#include "utility/vulkan_utils.hpp"
#include <iostream>
#include <stdexcept>
#include <vulkan/vulkan_core.h>

namespace ving
{
Swapchain::Swapchain(const VulkanCore &core, const Window &window) : m_device{core.device()}
{
    vkGetDeviceQueue(m_device, core.present_queue_family(), 0, &m_present_queue);
    m_image_extent = VkExtent2D{window.width(), window.height()};
    m_swapchain = create_vulkan_swapchain(
        core.physical_device(), core.device(), window.vulkan_surface(), m_image_extent,
        (core.present_queue_family() == core.graphics_queue_family()) ? 1 : 2, RenderFrames::frames_in_flight);

    uint32_t count;
    vkGetSwapchainImagesKHR(m_device, m_swapchain, &count, nullptr);
    m_images.resize(count);
    vkGetSwapchainImagesKHR(m_device, m_swapchain, &count, m_images.data());

    for (size_t i = 0; i < m_images.size(); ++i)
    {
        m_views.push_back(create_vulkan_image_view(m_device, m_images[i], VK_FORMAT_B8G8R8A8_UNORM));
    }
}
Swapchain::~Swapchain()
{
    for (size_t i = 0; i < m_images.size(); ++i)
    {
        /*vkDestroyImage(m_device, m_images[i], nullptr);*/
        vkDestroyImageView(m_device, m_views[i], nullptr);
    }

    vkDestroySwapchainKHR(m_device, m_swapchain, nullptr);
}
uint32_t Swapchain::acquire_image(VkSemaphore image_acquire_semaphore)
{
    uint32_t index{};
    if (vkAcquireNextImageKHR(m_device, m_swapchain, 10000000000, image_acquire_semaphore, nullptr, &index) !=
        VK_SUCCESS)
        throw std::runtime_error("Failed to acquire swapchain image, probably it was timeout");
    return index;
}

void Swapchain::copy_image_to_swapchain(VkCommandBuffer cmd, VkImage source, VkExtent2D extent,
                                        uint32_t swapchain_image_index)
{
    transition_image(cmd, m_images[swapchain_image_index], VK_IMAGE_LAYOUT_UNDEFINED,
                     VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
    copy_image_to_image(cmd, source, m_images[swapchain_image_index], extent, m_image_extent);
}

void Swapchain::transition_swapchain_image_to_present(VkCommandBuffer cmd, uint32_t acquired_image_index)
{
    transition_image(cmd, m_images[acquired_image_index], VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                     VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);
}

void Swapchain::present_image(VkSemaphore wait_semaphore, uint32_t acquire_image_index)
{
    VkPresentInfoKHR info{};
    info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    info.swapchainCount = 1;
    info.pSwapchains = &m_swapchain;
    info.waitSemaphoreCount = 1;
    info.pWaitSemaphores = &wait_semaphore;
    info.pImageIndices = &acquire_image_index;

    if (vkQueuePresentKHR(m_present_queue, &info) != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to present image");
    }
}
} // namespace ving
