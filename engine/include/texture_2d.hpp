#pragma once

#include "vulkan_core.hpp"

namespace ving
{
class Texture2D
{
  public:
    Texture2D(const VulkanCore &core, VkExtent2D extent, VkImageUsageFlags image_usage, VmaMemoryUsage memory_usage,
              VkFormat format);
    ~Texture2D();

    [[nodiscard]] VkExtent2D extent() const noexcept { return m_extent; }
    [[nodiscard]] VkFormat format() const noexcept { return m_format; }
    [[nodiscard]] VkImage image() const noexcept { return m_image; }
    [[nodiscard]] VkImageView view() const noexcept { return m_view; }
    [[nodiscard]] VkImageLayout layout() const noexcept { return m_layout; }

    void transition_layout(VkCommandBuffer cmd, VkImageLayout new_layout);

  private:
    VkDevice m_device;
    VmaAllocator m_allocator;

    VkExtent2D m_extent;
    VkFormat m_format;
    VkImage m_image;
    VmaAllocation m_allocation;
    VkImageView m_view;
    VkImageLayout m_layout;
};
} // namespace ving
