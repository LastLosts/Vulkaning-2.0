#include "mesh.hpp"
#include <iostream>

namespace ving
{
Mesh::Mesh(const VulkanCore &core, std::span<Vertex> vertices, std::span<uint32_t> indices)
    : m_vertex_buffer{core, sizeof(Vertex) * vertices.size(),
                      VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT |
                          VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT,
                      VMA_MEMORY_USAGE_GPU_ONLY},
      m_index_buffer{core, sizeof(uint32_t) * indices.size(),
                     VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT, VMA_MEMORY_USAGE_GPU_ONLY},
      m_index_count{static_cast<uint32_t>(indices.size())}, m_vertex_buffer_address{0}
{
    GPUBuffer vertex_staging{core, m_vertex_buffer.size(), VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VMA_MEMORY_USAGE_CPU_ONLY};
    GPUBuffer index_staging{core, m_index_buffer.size(), VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VMA_MEMORY_USAGE_CPU_ONLY};

    vertex_staging.set_memory(vertices.data(), vertices.size() * sizeof(Vertex));
    index_staging.set_memory(indices.data(), indices.size() * sizeof(uint32_t));

    core.immediate_transfer([this, &vertex_staging, &index_staging](VkCommandBuffer cmd) {
        VkBufferCopy copy{};
        copy.size = m_vertex_buffer.size();
        vkCmdCopyBuffer(cmd, vertex_staging.buffer(), m_vertex_buffer.buffer(), 1, &copy);

        copy.size = m_index_buffer.size();
        vkCmdCopyBuffer(cmd, index_staging.buffer(), m_index_buffer.buffer(), 1, &copy);
    });

    VkBufferDeviceAddressInfo info{};
    info.sType = VK_STRUCTURE_TYPE_BUFFER_DEVICE_ADDRESS_INFO;
    info.buffer = m_vertex_buffer.buffer();

    m_vertex_buffer_address = vkGetBufferDeviceAddress(core.device(), &info);
    assert(m_vertex_buffer_address != 0);
}
} // namespace ving
