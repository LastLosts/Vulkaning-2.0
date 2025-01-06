#pragma once

#include "vulkan_core.hpp"

namespace ving
{
class GPUBuffer
{
  public:
    GPUBuffer(const VulkanCore &core, size_t allocation_size, VkBufferUsageFlags usage, VmaMemoryUsage memory_usage);
    ~GPUBuffer();

  private:
    VmaAllocator m_allocator;

    VkBuffer m_buffer;
    VmaAllocation m_allocation;
    VmaAllocationInfo m_allocation_info;
};
} // namespace ving
