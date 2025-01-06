#include "compute_pipeline.hpp"
#include <vulkan/vulkan_core.h>

namespace ving
{

ComputePipeline::ComputePipeline() : m_device{VK_NULL_HANDLE}, m_pipeline{VK_NULL_HANDLE}, m_layout{VK_NULL_HANDLE}
{
}
ComputePipeline::ComputePipeline(const VulkanCore &core, const ShaderResources &shader_resources,
                                 VkShaderModule compute_shader)
    : m_device{core.device()}
{
    VkPipelineLayoutCreateInfo layout_info{};
    layout_info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    layout_info.pSetLayouts = shader_resources.layouts();
    layout_info.setLayoutCount = shader_resources.layouts_size();

    if (vkCreatePipelineLayout(m_device, &layout_info, nullptr, &m_layout) != VK_SUCCESS)
        throw std::runtime_error("Failed to create compute pipeline layout. Do proper error handling.");

    VkPipelineShaderStageCreateInfo shader_stage{};
    shader_stage.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    shader_stage.stage = VK_SHADER_STAGE_COMPUTE_BIT;
    shader_stage.module = compute_shader;
    shader_stage.pName = "main";

    VkComputePipelineCreateInfo pipeline_info{};
    pipeline_info.sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;
    pipeline_info.layout = m_layout;
    pipeline_info.stage = shader_stage;

    if (vkCreateComputePipelines(m_device, VK_NULL_HANDLE, 1, &pipeline_info, nullptr, &m_pipeline))
        throw std::runtime_error("Failed to create compute pipeline. Do proper error handling.");
}
ComputePipeline::~ComputePipeline()
{
    if (m_layout != VK_NULL_HANDLE)
        vkDestroyPipelineLayout(m_device, m_layout, nullptr);
    if (m_pipeline != VK_NULL_HANDLE)
        vkDestroyPipeline(m_device, m_pipeline, nullptr);
}

} // namespace ving
