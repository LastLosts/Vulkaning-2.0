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

  private:
    VkDevice m_device;

    VkSwapchainKHR m_swapchain;
    std::vector<VkImage> m_swapchain_images;
    std::vector<VkImageView> m_swapchain_image_views;

    VkQueue m_present_queue;
};
} // namespace ving
