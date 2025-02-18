#pragma once

#include "shader_resources.hpp"
#include "vulkan_core.hpp"
#include <vulkan/vulkan_core.h>

namespace ving
{
class GraphicsPipline
{
  public:
    GraphicsPipline();
    GraphicsPipline(const VulkanCore &core, const ShaderResources &resources, uint32_t push_constant_size,
                    VkShaderModule vertex_shader, VkShaderModule fragment_shader,
                    VkPolygonMode polygon_mode = VK_POLYGON_MODE_FILL);
    ~GraphicsPipline();

    GraphicsPipline(const GraphicsPipline &) = delete;
    GraphicsPipline(GraphicsPipline &&) = delete;
    GraphicsPipline &operator=(const GraphicsPipline &) = delete;
    GraphicsPipline &operator=(GraphicsPipline &&other)
    {
        m_device = other.m_device;
        std::swap(m_pipeline, other.m_pipeline);
        std::swap(m_layout, other.m_layout);
        std::swap(m_push_constants_size, other.m_push_constants_size);
        return *this;
    }

    [[nodiscard]] VkPipeline pipeline() const noexcept { return m_pipeline; }
    [[nodiscard]] VkPipelineLayout layout() const noexcept { return m_layout; }

  private:
    VkDevice m_device;

    VkPipeline m_pipeline;
    VkPipelineLayout m_layout;

    uint32_t m_push_constants_size;
};
} // namespace ving
