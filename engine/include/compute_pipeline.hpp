#pragma once

#include "shader_resources.hpp"
#include "vulkan_core.hpp"

namespace ving
{
class ComputePipeline
{
  public:
    ComputePipeline();
    ComputePipeline(const VulkanCore &core, const ShaderResources &shader_resources, uint32_t push_constants_size,
                    VkShaderModule compute_shader);
    ~ComputePipeline();

    ComputePipeline(const ComputePipeline &) = default;
    ComputePipeline(ComputePipeline &&) = delete;
    ComputePipeline &operator=(const ComputePipeline &) = default;
    ComputePipeline &operator=(ComputePipeline &&other)
    {
        std::swap(m_device, other.m_device);
        std::swap(m_pipeline, other.m_pipeline);
        std::swap(m_layout, other.m_layout);
        return *this;
    }

    [[nodiscard]] VkPipeline pipeline() const noexcept { return m_pipeline; }
    [[nodiscard]] VkPipelineLayout layout() const noexcept { return m_layout; }

  private:
    VkDevice m_device;

    VkPipeline m_pipeline;
    VkPipelineLayout m_layout;
};
} // namespace ving
