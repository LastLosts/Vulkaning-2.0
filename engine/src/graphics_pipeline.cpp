#include "graphics_pipeline.hpp"
#include "render_frames.hpp"
#include <stdexcept>
#include <vulkan/vulkan_core.h>

namespace ving
{
GraphicsPipline::GraphicsPipline(const VulkanCore &core, const ShaderResources &resources, uint32_t push_constant_size,
                                 VkShaderModule vertex_shader, VkShaderModule fragment_shader)
    : m_device{core.device()}
{
    VkPushConstantRange push_constant_range{};
    push_constant_range.offset = 0;
    push_constant_range.size = push_constant_size;
    push_constant_range.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;

    VkPipelineLayoutCreateInfo layout_info{};
    layout_info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    layout_info.pushConstantRangeCount = 1;
    layout_info.pPushConstantRanges = &push_constant_range;
    layout_info.setLayoutCount = resources.layouts_size();
    layout_info.pSetLayouts = resources.layouts();

    if (vkCreatePipelineLayout(m_device, &layout_info, nullptr, &m_layout) != VK_SUCCESS)
        throw std::runtime_error("Failed to create pipeline layout. Do proper error handling");

    VkPipelineShaderStageCreateInfo vertex_shader_info{};
    vertex_shader_info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    vertex_shader_info.stage = VK_SHADER_STAGE_VERTEX_BIT;
    vertex_shader_info.module = vertex_shader;
    vertex_shader_info.pName = "main";

    VkPipelineShaderStageCreateInfo fragment_shader_info{};
    fragment_shader_info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    fragment_shader_info.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    fragment_shader_info.module = fragment_shader;
    fragment_shader_info.pName = "main";

    std::vector<VkPipelineShaderStageCreateInfo> shader_stages{vertex_shader_info, fragment_shader_info};

    VkPipelineInputAssemblyStateCreateInfo input_assembly{};
    input_assembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    input_assembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;

    VkPipelineVertexInputStateCreateInfo vertex_input{};
    vertex_input.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;

    VkPipelineViewportStateCreateInfo viewport_state{};
    viewport_state.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewport_state.scissorCount = 1;
    viewport_state.viewportCount = 1;

    VkPipelineRasterizationStateCreateInfo rasterizer{};
    rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
    rasterizer.lineWidth = 1.0f;
    rasterizer.cullMode = VK_CULL_MODE_NONE;
    rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE;

    VkPipelineMultisampleStateCreateInfo multisample{};
    multisample.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    multisample.sampleShadingEnable = VK_FALSE;
    multisample.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
    multisample.minSampleShading = 1.0f;
    multisample.alphaToCoverageEnable = VK_FALSE;
    multisample.alphaToOneEnable = VK_FALSE;

    VkPipelineColorBlendAttachmentState color_blend_attachment{};
    color_blend_attachment.colorWriteMask =
        VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
    color_blend_attachment.blendEnable = VK_FALSE;

    VkPipelineColorBlendStateCreateInfo color_blend{};
    color_blend.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    color_blend.logicOpEnable = VK_FALSE;
    color_blend.logicOp = VK_LOGIC_OP_COPY;
    color_blend.attachmentCount = 1;
    color_blend.pAttachments = &color_blend_attachment;

    VkPipelineDepthStencilStateCreateInfo depth_test{};
    depth_test.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;

    VkFormat color_attachment_format = RenderFrames::draw_image_format;

    VkPipelineRenderingCreateInfo render_info{};
    render_info.sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO;
    render_info.colorAttachmentCount = 1;
    render_info.pColorAttachmentFormats = &color_attachment_format;

    VkDynamicState states[] = {VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR};

    VkPipelineDynamicStateCreateInfo dynamic_info{};
    dynamic_info.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    dynamic_info.dynamicStateCount = 2;
    dynamic_info.pDynamicStates = states;

    VkGraphicsPipelineCreateInfo pipeline_info{};
    pipeline_info.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    pipeline_info.stageCount = shader_stages.size();
    pipeline_info.pStages = shader_stages.data();
    pipeline_info.pVertexInputState = &vertex_input;
    pipeline_info.pInputAssemblyState = &input_assembly;
    pipeline_info.pRasterizationState = &rasterizer;
    pipeline_info.pMultisampleState = &multisample;
    pipeline_info.pColorBlendState = &color_blend;
    pipeline_info.pDepthStencilState = &depth_test;
    pipeline_info.pDynamicState = &dynamic_info;
    pipeline_info.layout = m_layout;
    pipeline_info.pNext = &render_info;

    if (vkCreateGraphicsPipelines(m_device, VK_NULL_HANDLE, 1, &pipeline_info, nullptr, &m_pipeline) != VK_SUCCESS)
        throw std::runtime_error("Failed to create graphics pipeline. Do proper error handling");
}
GraphicsPipline::~GraphicsPipline()
{
    vkDestroyPipeline(m_device, m_pipeline, nullptr);
    vkDestroyPipelineLayout(m_device, m_layout, nullptr);
}
} // namespace ving
