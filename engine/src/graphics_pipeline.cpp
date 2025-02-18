#include "graphics_pipeline.hpp"
#include "render_frames.hpp"

namespace ving
{
GraphicsPipline::GraphicsPipline() : m_device{nullptr}, m_pipeline{nullptr}, m_layout{nullptr}, m_push_constants_size{0}
{
}
GraphicsPipline::GraphicsPipline(const VulkanCore &core, const ShaderResources &resources, uint32_t push_constant_size,
                                 VkShaderModule vertex_shader, VkShaderModule fragment_shader,
                                 VkPolygonMode polygon_mode)
    : m_device{core.device()}, m_push_constants_size{push_constant_size}
{
    VkPipelineLayoutCreateInfo layout_info{};
    layout_info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    layout_info.setLayoutCount = resources.layouts_size();
    layout_info.pSetLayouts = resources.layouts();

    if (push_constant_size != 0)
    {
        VkPushConstantRange push_constant_range{};
        push_constant_range.offset = 0;
        push_constant_range.size = push_constant_size;
        push_constant_range.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;

        layout_info.pushConstantRangeCount = 1;
        layout_info.pPushConstantRanges = &push_constant_range;
    }

    if (vkCreatePipelineLayout(m_device, &layout_info, nullptr, &m_layout) != VK_SUCCESS)
    {
        std::cout << "Failed to create pipeline layout\n";
    }

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
    rasterizer.polygonMode = polygon_mode;
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

    // Enabled alpha blending by default
    VkPipelineColorBlendAttachmentState color_blend_attachment{};
    color_blend_attachment.colorWriteMask =
        VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
    color_blend_attachment.blendEnable = VK_TRUE;
    color_blend_attachment.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
    color_blend_attachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
    color_blend_attachment.colorBlendOp = VK_BLEND_OP_ADD;

    color_blend_attachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
    color_blend_attachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
    color_blend_attachment.alphaBlendOp = VK_BLEND_OP_ADD;

    VkPipelineColorBlendStateCreateInfo color_blend{};
    color_blend.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    color_blend.logicOpEnable = VK_FALSE;
    color_blend.logicOp = VK_LOGIC_OP_COPY;
    color_blend.attachmentCount = 1;
    color_blend.pAttachments = &color_blend_attachment;

    VkPipelineDepthStencilStateCreateInfo depth_test{};
    depth_test.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
    depth_test.depthWriteEnable = VK_TRUE;
    depth_test.depthCompareOp = VK_COMPARE_OP_GREATER_OR_EQUAL;
    depth_test.depthBoundsTestEnable = VK_FALSE;
    depth_test.stencilTestEnable = VK_FALSE;
    depth_test.minDepthBounds = 0.0f;
    depth_test.maxDepthBounds = 1.0f;

    VkFormat color_attachment_format = RenderFrames::draw_image_format;

    VkPipelineRenderingCreateInfo render_info{};
    render_info.sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO;
    render_info.colorAttachmentCount = 1;
    render_info.pColorAttachmentFormats = &color_attachment_format;
    render_info.depthAttachmentFormat = VK_FORMAT_D32_SFLOAT;

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
    pipeline_info.pViewportState = &viewport_state;
    pipeline_info.pInputAssemblyState = &input_assembly;
    pipeline_info.pRasterizationState = &rasterizer;
    pipeline_info.pMultisampleState = &multisample;
    pipeline_info.pColorBlendState = &color_blend;
    pipeline_info.pDepthStencilState = &depth_test;
    pipeline_info.pDynamicState = &dynamic_info;
    pipeline_info.layout = m_layout;
    pipeline_info.pNext = &render_info;

    if (vkCreateGraphicsPipelines(m_device, VK_NULL_HANDLE, 1, &pipeline_info, nullptr, &m_pipeline) != VK_SUCCESS)
        std::cout << "Failed to create graphics pipeline. Do proper error handling\n";
}
GraphicsPipline::~GraphicsPipline()
{
    if (m_device != VK_NULL_HANDLE)
    {
        vkDestroyPipeline(m_device, m_pipeline, nullptr);
        vkDestroyPipelineLayout(m_device, m_layout, nullptr);
    }
}

/*void GraphicsPipline::draw(VkCommandBuffer cmd, VkRenderingInfo render_info, const ShaderResources &resources,*/
/*                           void *push_constants)*/
/*{*/
/*    VkViewport viewport{};*/
/*    viewport.width = render_info.renderArea.extent.width;*/
/*    viewport.height = render_info.renderArea.extent.height;*/
/*    viewport.minDepth = 0.0f;*/
/*    viewport.maxDepth = 1.0f;*/
/**/
/*    VkRect2D scissor{};*/
/*    scissor.offset = {0, 0};*/
/*    scissor.extent = render_info.renderArea.extent;*/
/**/
/*    vkCmdBeginRendering(cmd, &render_info);*/
/*    vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipeline);*/
/*    vkCmdBindDescriptorSets(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, m_layout, 0, resources.sets_size(),
 * resources.sets(),*/
/*                            0, nullptr);*/
/*    vkCmdPushConstants(cmd, m_layout, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 0,*/
/*                       m_push_constants_size, push_constants);*/
/*    vkCmdSetViewport(cmd, 0, 1, &viewport);*/
/*    vkCmdSetScissor(cmd, 0, 1, &scissor);*/
/**/
/*    vkCmdEndRendering(cmd);*/
/*}*/
} // namespace ving
