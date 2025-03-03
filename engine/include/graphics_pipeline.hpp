#pragma once

#include "shader_resources.hpp"
#include "vulkan_core.hpp"
#include <vulkan/vulkan_core.h>

namespace ving
{
class GraphicsPipeline
{
  public:
    GraphicsPipeline();
    GraphicsPipeline(const VulkanCore &core, const ShaderResources &resources, VkShaderModule vertex_shader,
                     VkShaderModule fragment_shader, uint32_t push_constant_size = 0,
                     VkPolygonMode polygon_mode = VK_POLYGON_MODE_FILL);
    ~GraphicsPipeline();

    GraphicsPipeline(const GraphicsPipeline &) = delete;
    GraphicsPipeline(GraphicsPipeline &&) = delete;
    GraphicsPipeline &operator=(const GraphicsPipeline &) = delete;
    GraphicsPipeline &operator=(GraphicsPipeline &&other)
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
