#include "gpu_buffer.hpp"
#include "vulkan_core.hpp"
#include <cassert>
#include <cstring>
#include <iostream>

namespace ving
{
GPUBuffer::GPUBuffer(const VulkanCore &core, size_t allocation_size, VkBufferUsageFlags usage,
                     VmaMemoryUsage memory_usage)
    : m_allocator{core.allocator()}, m_memory_mapped{false}, m_size{allocation_size}
{
    VkBufferCreateInfo buffer_info{};
    buffer_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    buffer_info.usage = usage;
    buffer_info.size = allocation_size;

    VmaAllocationCreateInfo alloc_info{};
    alloc_info.usage = memory_usage;
    alloc_info.flags = VMA_ALLOCATION_CREATE_MAPPED_BIT;

    if (vmaCreateBuffer(core.allocator(), &buffer_info, &alloc_info, &m_buffer, &m_allocation, nullptr) != VK_SUCCESS)
        std::cout << "Failed to create buffer. Do proper error handling\n";
}
GPUBuffer::~GPUBuffer()
{
    if (m_memory_mapped)
        vmaUnmapMemory(m_allocator, m_allocation);

    vmaDestroyBuffer(m_allocator, m_buffer, m_allocation);
}
void *GPUBuffer::map_and_get_memory()
{
    if (vmaMapMemory(m_allocator, m_allocation, &m_mapped_memory) == VK_SUCCESS)
    {
        m_memory_mapped = true;
    }

    /*std::cerr << "Failed to map buffer memory\n";*/
    return m_mapped_memory;
}

void GPUBuffer::set_memory(void *data, uint32_t size)
{
    assert(m_size >= size);

    if (vmaMapMemory(m_allocator, m_allocation, &m_mapped_memory) == VK_SUCCESS)
    {
        memcpy(m_mapped_memory, data, size);

        vmaUnmapMemory(m_allocator, m_allocation);
    }
    else
    {
        std::cout << "Failed to map\n";
    }
}

} // namespace ving
