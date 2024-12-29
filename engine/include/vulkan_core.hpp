#pragma once

#include "window.hpp"
#include <vulkan/vulkan.h>

namespace ving
{
class VulkanCore
{
  public:
    VulkanCore(const VulkanInstance &instance, const Window &window);
    ~VulkanCore();

    VulkanCore(const VulkanCore &) = delete;
    VulkanCore &operator=(const VulkanCore &) = delete;
    VulkanCore(VulkanCore &&) = delete;
    VulkanCore &operator=(VulkanCore &&) = delete;

    [[nodiscard]] uint32_t graphics_queue_family() const noexcept { return m_graphics_queue_family; }
    [[nodiscard]] uint32_t present_queue_family() const noexcept { return m_present_queue_family; }
    [[nodiscard]] VkDevice device() const noexcept { return m_device; }
    [[nodiscard]] VkPhysicalDevice physical_device() const noexcept { return m_physical_device; }

  private:
    VkInstance m_instance;
    VkPhysicalDevice m_physical_device;
    VkDevice m_device;

    uint32_t m_graphics_queue_family;
    uint32_t m_present_queue_family;
};
} // namespace ving
