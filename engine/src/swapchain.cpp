#include "swapchain.hpp"
#include "render_frames.hpp"

#include "utility/vulkan_utils.hpp"
#include <stdexcept>
#include <vulkan/vulkan_core.h>

namespace ving
{
Swapchain::Swapchain(const VulkanCore &core, const Window &window) : m_device{core.device()}
{
    m_swapchain = create_vulkan_swapchain(
        core.physical_device(), core.device(), window.vulkan_surface(), VkExtent2D{window.width(), window.height()},
        (core.present_queue_family() == core.graphics_queue_family()) ? 1 : 2, RenderFrames::frames_in_flight);

    uint32_t count;
    vkGetSwapchainImagesKHR(m_device, m_swapchain, &count, nullptr);
    m_swapchain_images.resize(count);
    vkGetSwapchainImagesKHR(m_device, m_swapchain, &count, m_swapchain_images.data());

    for (size_t i = 0; i < m_swapchain_images.size(); ++i)
    {
        m_swapchain_image_views.push_back(
            create_vulkan_image_view(m_device, m_swapchain_images[i], VK_FORMAT_B8G8R8A8_UNORM));
    }
}
Swapchain::~Swapchain()
{
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

} // namespace ving
