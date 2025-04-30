#pragma once

#include "gpu_buffer.hpp"
#include "vertex.hpp"
#include <span>

namespace ving
{
class Mesh
{
  public:
    Mesh() = default;
    Mesh(const VulkanCore &core, std::span<Vertex> vertices, std::span<uint32_t> indices);

    [[nodiscard]] VkDeviceAddress vertex_address() const noexcept { return m_vertex_buffer_address; }
    [[nodiscard]] const GPUBuffer &vertex_buffer() const noexcept { return m_vertex_buffer; }
    [[nodiscard]] const GPUBuffer &index_buffer() const noexcept { return m_index_buffer; }

    [[nodiscard]] uint32_t index_count() const noexcept { return m_index_count; }

  private:
    GPUBuffer m_vertex_buffer;
    uint32_t m_index_count;
    GPUBuffer m_index_buffer;
    VkDeviceAddress m_vertex_buffer_address{0};
};
} // namespace ving
