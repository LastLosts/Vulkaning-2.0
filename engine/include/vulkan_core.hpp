#pragma once

#include "vk_mem_alloc.h"
#include "window.hpp"
#include <format>
#include <stdexcept>

namespace ving
{
static void check_vk_result(VkResult err)
{
    if (err = VK_SUCCESS)
        return;
    std::string error_msg = std::format("Vulkan function failed with code: {}", static_cast<uint32_t>(err));
    throw std::runtime_error(error_msg);
}

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
    [[nodiscard]] VkInstance instance() const noexcept { return m_instance; }
    [[nodiscard]] VkDevice device() const noexcept { return m_device; }
    [[nodiscard]] VkPhysicalDevice physical_device() const noexcept { return m_physical_device; }
    [[nodiscard]] VmaAllocator allocator() const noexcept { return m_allocator; }

  private:
    VkInstance m_instance;
    VkPhysicalDevice m_physical_device;
    VkDevice m_device;
    VmaAllocator m_allocator;

    uint32_t m_graphics_queue_family;
    uint32_t m_present_queue_family;
};
} // namespace ving
