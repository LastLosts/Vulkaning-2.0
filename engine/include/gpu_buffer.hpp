#pragma once

#include "vulkan_core.hpp"

namespace ving
{
class GPUBuffer
{
  public:
    GPUBuffer();
    GPUBuffer(const VulkanCore &core, size_t allocation_size, VkBufferUsageFlags usage, VmaMemoryUsage memory_usage);
    ~GPUBuffer();

    GPUBuffer(const GPUBuffer &) = delete;
    GPUBuffer(GPUBuffer &&other)
    {
        m_allocator = other.m_allocator;
        std::swap(m_buffer, other.m_buffer);
        std::swap(m_allocation, other.m_allocation);
        m_size = other.m_size;
        std::swap(m_memory_mapped, other.m_memory_mapped);
        std::swap(m_mapped_memory, other.m_mapped_memory);
    }
    GPUBuffer &operator=(const GPUBuffer &) = delete;
    GPUBuffer &operator=(GPUBuffer &&other)
    {
        m_allocator = other.m_allocator;
        std::swap(m_buffer, other.m_buffer);
        std::swap(m_allocation, other.m_allocation);
        m_size = other.m_size;
        std::swap(m_memory_mapped, other.m_memory_mapped);
        std::swap(m_mapped_memory, other.m_mapped_memory);
        return *this;
    }

    void *map_and_get_memory();
    void set_memory(const void *data, uint32_t size);

    [[nodiscard]] VkBuffer buffer() const noexcept { return m_buffer; }
    [[nodiscard]] VkDeviceSize size() const noexcept { return m_size; }

  private:
    VmaAllocator m_allocator;

    VkBuffer m_buffer;
    VmaAllocation m_allocation;

    VkDeviceSize m_size;

    bool m_memory_mapped;
    void *m_mapped_memory;
};
} // namespace ving
