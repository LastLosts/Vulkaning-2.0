#pragma once

#include "shader_resources.hpp"
#include "vulkan_core.hpp"

namespace ving
{
class ComputePipeline
{
  public:
    ComputePipeline(const VulkanCore &core, const ShaderResources &shader_resources, uint32_t push_constants_size,
                    VkShaderModule compute_shader);
    ~ComputePipeline();

    ComputePipeline(const ComputePipeline &) = delete;
    ComputePipeline(ComputePipeline &&) = delete;
    ComputePipeline &operator=(const ComputePipeline &) = delete;
    ComputePipeline &operator=(ComputePipeline &&other) = delete;

    [[nodiscard]] VkPipeline pipeline() const noexcept { return m_pipeline; }
    [[nodiscard]] VkPipelineLayout layout() const noexcept { return m_layout; }

    void dispatch(VkCommandBuffer cmd, const ShaderResources &resources, void *push_constants, uint32_t group_count_x,
                  uint32_t group_count_y = 1, uint32_t group_count_z = 1);

  private:
    VkDevice m_device;

    VkPipeline m_pipeline;
    VkPipelineLayout m_layout;
    size_t m_push_constants_size;
};
} // namespace ving
