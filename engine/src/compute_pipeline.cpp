#include "compute_pipeline.hpp"
#include <iostream>

namespace ving
{
ComputePipeline::ComputePipeline(const VulkanCore &core, const ShaderResources &shader_resources,
                                 VkShaderModule compute_shader, uint32_t push_constants_size)
    : m_device{core.device()}, m_push_constants_size{push_constants_size}
{

    VkPipelineLayoutCreateInfo layout_info{};
    layout_info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    layout_info.pSetLayouts = shader_resources.layouts();
    layout_info.setLayoutCount = shader_resources.layouts_size();
    layout_info.pushConstantRangeCount = 0;

    VkPushConstantRange push_constant_range{};
    push_constant_range.size = push_constants_size;
    push_constant_range.offset = 0;
    push_constant_range.stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;

    if (push_constants_size != 0)
    {
        layout_info.pushConstantRangeCount = 1;
        layout_info.pPushConstantRanges = &push_constant_range;
    }

    if (vkCreatePipelineLayout(m_device, &layout_info, nullptr, &m_layout) != VK_SUCCESS)
    {
        std::cout << "Failed to create compute pipeline layout\n";
        exit(-1);
    }

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
    {
        std::cout << "Failed to create compute pipeline\n";
        exit(-1);
    }
}
ComputePipeline::ComputePipeline(const VulkanCore &core, VkShaderModule compute_shader, uint32_t push_constants_size)
    : m_device{core.device()}, m_push_constants_size{push_constants_size}
{
    VkPipelineLayoutCreateInfo layout_info{};
    layout_info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;

    VkPushConstantRange push_constant_range{};
    push_constant_range.size = push_constants_size;
    push_constant_range.offset = 0;
    push_constant_range.stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;
    layout_info.pushConstantRangeCount = 0;

    if (push_constants_size != 0)
    {
        layout_info.pushConstantRangeCount = 1;
        layout_info.pPushConstantRanges = &push_constant_range;
    }

    if (vkCreatePipelineLayout(m_device, &layout_info, nullptr, &m_layout) != VK_SUCCESS)
    {
        std::cout << "Failed to create compute pipeline layout. Do proper error handling.\n";
        exit(-1);
    }

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
    {
        std::cout << "Failed to create compute pipeline. Do proper error handling.";
    }
}

ComputePipeline::~ComputePipeline()
{
    if (m_device != VK_NULL_HANDLE)
    {
        vkDestroyPipelineLayout(m_device, m_layout, nullptr);
        vkDestroyPipeline(m_device, m_pipeline, nullptr);
    }
}

void ComputePipeline::dispatch(VkCommandBuffer cmd, const ShaderResources &resources, void *push_constants,
                               uint32_t group_count_x, uint32_t group_count_y, uint32_t group_count_z)
{
    vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_COMPUTE, m_pipeline);
    vkCmdBindDescriptorSets(cmd, VK_PIPELINE_BIND_POINT_COMPUTE, m_layout, 0, resources.sets_size(), resources.sets(),
                            0, nullptr);
    if (m_push_constants_size > 0)
        vkCmdPushConstants(cmd, m_layout, VK_SHADER_STAGE_COMPUTE_BIT, 0, m_push_constants_size, push_constants);

    vkCmdDispatch(cmd, group_count_x, group_count_y, group_count_z);
}

} // namespace ving
