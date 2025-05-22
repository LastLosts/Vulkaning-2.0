#pragma once

#include "vk_mem_alloc.h"
#include "window.hpp"
#include <functional>

namespace ving
{
class VulkanCore
{
  public:
    VulkanCore(const Window &window);
    ~VulkanCore();

    VulkanCore(const VulkanCore &) = delete;
    VulkanCore &operator=(const VulkanCore &) = delete;
    VulkanCore(VulkanCore &&) = delete;
    VulkanCore &operator=(VulkanCore &&) = delete;

    [[nodiscard]] uint32_t graphics_queue_family() const noexcept { return m_graphics_queue_family; }
    [[nodiscard]] uint32_t present_queue_family() const noexcept { return m_present_queue_family; }
    [[nodiscard]] VkInstance instance() const noexcept { return m_instance; }
    [[nodiscard]] VkSurfaceKHR window_surface() const noexcept { return m_window_surface; }
    [[nodiscard]] VkDevice device() const noexcept { return m_device; }
    [[nodiscard]] VkPhysicalDevice physical_device() const noexcept { return m_physical_device; }
    [[nodiscard]] VmaAllocator allocator() const noexcept { return m_allocator; }

    void immediate_transfer(std::function<void(VkCommandBuffer cmd)> &&func) const;

  private:
    VkInstance m_instance;
    VkSurfaceKHR m_window_surface;
    VkPhysicalDevice m_physical_device;
    VkDevice m_device;
    VmaAllocator m_allocator;

    uint32_t m_graphics_queue_family;
    uint32_t m_present_queue_family;

    VkQueue m_transfer_queue;
    VkCommandPool m_immediate_transfer_pool;
    VkCommandBuffer m_immediate_transfer_commands;
    VkFence m_immediate_transfer_fence;
};
} // namespace ving
