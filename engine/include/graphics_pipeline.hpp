#pragma once

#include "shader_resources.hpp"
#include "vulkan_core.hpp"
#include <vulkan/vulkan_core.h>

namespace ving
{
class GraphicsPipline
{
  public:
    GraphicsPipline(const VulkanCore &core, const ShaderResources &resources, uint32_t push_constant_size,
                    VkShaderModule vertex_shader, VkShaderModule fragment_shader);

  private:
    VkDevice m_device;

    VkPipeline m_pipeline;
    VkPipelineLayout m_layout;
};
} // namespace ving
