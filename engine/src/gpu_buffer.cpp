#include "gpu_buffer.hpp"
#include "vulkan_core.hpp"
#include <cassert>
#include <cstring>

namespace ving
{
GPUBuffer::GPUBuffer(const VulkanCore &core, size_t allocation_size, VkBufferUsageFlags usage,
                     VmaMemoryUsage memory_usage)
    : m_allocator{core.allocator()}
{
    VkBufferCreateInfo buffer_info{};
    buffer_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    buffer_info.usage = usage;
    buffer_info.size = allocation_size;

    VmaAllocationCreateInfo alloc_info{};
    alloc_info.usage = memory_usage;
    alloc_info.flags = VMA_ALLOCATION_CREATE_MAPPED_BIT;

    if (vmaCreateBuffer(core.allocator(), &buffer_info, &alloc_info, &m_buffer, &m_allocation, &m_allocation_info) !=
        VK_SUCCESS)
        throw std::runtime_error("Failed to create buffer. Do proper error handling");
}
GPUBuffer::~GPUBuffer()
{
    vmaDestroyBuffer(m_allocator, m_buffer, m_allocation);
}
void GPUBuffer::set_memory(void *data, uint32_t size)
{
    assert(m_allocation_info.size >= size);
    void *buffer_data;
    vmaMapMemory(m_allocator, m_allocation, &buffer_data);

    memcpy(buffer_data, data, size);

    vmaUnmapMemory(m_allocator, m_allocation);
}

} // namespace ving
