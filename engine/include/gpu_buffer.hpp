#pragma once

#include "vulkan_core.hpp"

namespace ving
{
class GPUBuffer
{
  public:
    GPUBuffer(const VulkanCore &core, size_t allocation_size, VkBufferUsageFlags usage, VmaMemoryUsage memory_usage);
    ~GPUBuffer();

    void set_memory(void *data, uint32_t size);

    [[nodiscard]] VkBuffer buffer() const noexcept { return m_buffer; }
    [[nodiscard]] size_t size() const noexcept { return m_allocation_info.size; }

  private:
    VmaAllocator m_allocator;

    VkBuffer m_buffer;
    VmaAllocation m_allocation;
    VmaAllocationInfo m_allocation_info;
};
} // namespace ving
