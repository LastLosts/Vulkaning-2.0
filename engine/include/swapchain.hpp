#pragma once

#include "vulkan_core.hpp"
#include <vector>
#include <vulkan/vulkan_core.h>

namespace ving
{
class Swapchain
{
  public:
    Swapchain(const VulkanCore &core, const Window &window);
    ~Swapchain();

    Swapchain(const Swapchain &) = delete;
    Swapchain &operator=(const Swapchain &) = delete;
    Swapchain(Swapchain &&) = delete;
    Swapchain &operator=(Swapchain &&) = delete;

    uint32_t acquire_image(VkSemaphore image_acquire_semaphore);
    void copy_image_to_swapchain(VkCommandBuffer cmd, VkImage source, VkExtent2D extent,
                                 uint32_t swapchain_image_index);
    void transition_swapchain_image_to_present(VkCommandBuffer cmd, uint32_t aqcuired_image_index);
    void present_image(VkSemaphore wait_semaphore, uint32_t acquire_image_index);

    static constexpr VkFormat image_format = VK_FORMAT_B8G8R8A8_UNORM;

  private:
    VkDevice m_device;

    VkSwapchainKHR m_swapchain;
    VkExtent2D m_image_extent;
    std::vector<VkImage> m_images;
    std::vector<VkImageView> m_views;

    VkQueue m_present_queue;
};
} // namespace ving
