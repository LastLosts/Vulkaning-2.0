#pragma once

#include "vulkan_core.hpp"
#include <vector>
#include <vulkan/vulkan_core.h>

namespace ving
{
class Swapchain
{
  public:
    Swapchain();
    Swapchain(const VulkanCore &core, VkExtent2D image_resolution, uint32_t preferred_image_count = 2);
    ~Swapchain();

    Swapchain(const Swapchain &) = delete;
    Swapchain &operator=(const Swapchain &) = delete;
    Swapchain(Swapchain &&other) : Swapchain()
    {
        m_device = other.m_device;
        m_present_queue = other.m_present_queue;

        std::swap(m_swapchain, other.m_swapchain);
        m_image_extent = other.m_image_extent;
        m_images = std::move(other.m_images);
        m_views = std::move(other.m_views);
    }
    Swapchain &operator=(Swapchain &&other)
    {
        m_device = other.m_device;
        m_present_queue = other.m_present_queue;

        m_swapchain = other.m_swapchain;
        other.m_swapchain = VK_NULL_HANDLE;
        m_image_extent = other.m_image_extent;
        m_images = std::move(other.m_images);
        m_views = std::move(other.m_views);

        return *this;
    }

    [[nodiscard]] uint32_t image_count() const { return m_images.size(); }

    VkResult acquire_image(VkSemaphore image_acquire_semaphore, uint32_t &out_index);
    void copy_image_to_swapchain(VkCommandBuffer cmd, VkImage source, VkExtent2D extent,
                                 uint32_t swapchain_image_index);
    void transition_swapchain_image_to_present(VkCommandBuffer cmd, uint32_t aqcuired_image_index);
    VkResult present_image(VkSemaphore wait_semaphore, uint32_t acquire_image_index);
    void resize(VkExtent2D render_resolution);

    static constexpr VkFormat image_format = VK_FORMAT_B8G8R8A8_UNORM;

  private:
    VkDevice m_device;
    VkQueue m_present_queue;

    VkSwapchainKHR m_swapchain;
    VkExtent2D m_image_extent;
    std::vector<VkImage> m_images;
    std::vector<VkImageView> m_views;
};
} // namespace ving
