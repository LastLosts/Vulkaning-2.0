#include "primitives_renderer.hpp"

#include "engine.hpp"
#include "glm/ext/matrix_clip_space.hpp"
#include "glm/ext/matrix_transform.hpp"
#include "mesh_generator.hpp"
#include "utility//vulkan_utils.hpp"

namespace ving
{

PrimitivesRenderer::PrimitivesRenderer(const VulkanCore &core) : m_quad{generate_quad(core)}
{
    m_push.vertex_buffer_address = m_quad.vertex_address();

    VkShaderModule vertex;
    if (!load_vulkan_shader_module(core.device(), "./engine/shaders/spirv/primitive.vert.spv", vertex))
    {
        // Handle shader file not opening
        exit(-1);
    }

    VkShaderModule fragment;
    if (!load_vulkan_shader_module(core.device(), "./engine/shaders/spirv/circle.frag.spv", fragment))
    {
        exit(-1);
    }

    std::vector<ving::ShaderBindingSet> bindings{
        ving::ShaderBindingSet{{
            /*{0, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER},*/
        }},
    };

    m_resources = ShaderResources{core.device(), bindings, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT};

    m_circle_pipeline = GraphicsPipline{core, m_resources, sizeof(PushConstants), vertex, fragment};

    vkDestroyShaderModule(core.device(), vertex, nullptr);
    vkDestroyShaderModule(core.device(), fragment, nullptr);

    float half_width = (float)Engine::initial_window_width / 2.0f;
    float half_height = (float)Engine::initial_window_height / 2.0f;

    /*m_push.ortho = glm::ortho(-half_width, half_width, -half_height, half_height);*/
    m_push.ortho = glm::ortho(0.0f, (float)Engine::initial_window_width, 0.0f, (float)Engine::initial_window_height);
}

void PrimitivesRenderer::render(PrimitiveType type, std::span<PrimitiveParameters> parameters, const FrameInfo &frame)
{
    VkCommandBuffer cmd = frame.cmd;
    Texture2D *draw_img = frame.draw_img;

    draw_img->transition_layout(cmd, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);

    VkClearValue clear_color{};
    clear_color.color = VkClearColorValue{0.1f, 0.1f, 0.1f, 0.0f};

    VkRenderingAttachmentInfo color_attachment{};
    color_attachment.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO;
    color_attachment.imageView = draw_img->view();
    color_attachment.imageLayout = draw_img->layout();
    color_attachment.loadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    /*color_attachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;*/
    color_attachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    /*color_attachment.clearValue = clear_color;*/

    VkRenderingInfo render_info{};
    render_info.sType = VK_STRUCTURE_TYPE_RENDERING_INFO;
    render_info.renderArea = VkRect2D{{0, 0}, draw_img->extent()};
    render_info.layerCount = 1;
    render_info.colorAttachmentCount = 1;
    render_info.pColorAttachments = &color_attachment;

    vkCmdBeginRendering(cmd, &render_info);
    GraphicsPipline *chosen_pipeline;

    switch (type)
    {
    case PrimitiveType::Circle: {
        chosen_pipeline = &m_circle_pipeline;
        break;
    }
    };

    vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, chosen_pipeline->pipeline());
    vkCmdBindDescriptorSets(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, chosen_pipeline->layout(), 0, m_resources.sets_size(),
                            m_resources.sets(), 0, nullptr);

    VkViewport viewport{};
    viewport.width = render_info.renderArea.extent.width;
    viewport.height = render_info.renderArea.extent.height;
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;

    VkRect2D scissor{};
    scissor.offset = {0, 0};
    scissor.extent = render_info.renderArea.extent;

    vkCmdSetViewport(cmd, 0, 1, &viewport);
    vkCmdSetScissor(cmd, 0, 1, &scissor);
    vkCmdBindIndexBuffer(cmd, m_quad.index_buffer().buffer(), 0, VK_INDEX_TYPE_UINT32);

    // TODO: Write it using instancing
    for (auto &&param : parameters)
    {
        m_push.position = param.position;
        m_push.scale = param.scale;
        m_push.color = param.color;

        vkCmdPushConstants(cmd, m_circle_pipeline.layout(), VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
                           0, sizeof(PushConstants), &m_push);

        vkCmdDrawIndexed(cmd, m_quad.index_count(), 1, 0, 0, 0);
    }
    vkCmdEndRendering(cmd);
}
void PrimitivesRenderer::render_3d(PrimitiveType3D type, std::span<PrimitiveParameters3D> parameters,
                                   const FrameInfo &frame, PerspectiveCamera &camera)
{
    VkCommandBuffer cmd = frame.cmd;
    Texture2D *draw_img = frame.draw_img;
    Texture2D *depth_img = frame.depth_img;

    draw_img->transition_layout(cmd, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);
    depth_img->transition_layout(cmd, VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL);

    VkClearValue clear_color{};
    clear_color.color = VkClearColorValue{0.1f, 0.1f, 0.1f, 0.0f};

    VkClearValue clear_depth{};
    clear_depth.depthStencil = VkClearDepthStencilValue{0.0f};

    VkRenderingAttachmentInfo color_attachment{};
    color_attachment.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO;
    color_attachment.imageView = draw_img->view();
    color_attachment.imageLayout = draw_img->layout();
    color_attachment.loadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    /*color_attachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;*/
    color_attachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    /*color_attachment.clearValue = clear_color;*/

    VkRenderingAttachmentInfo depth_attachment{};
    depth_attachment.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO;
    depth_attachment.imageView = depth_img->view();
    depth_attachment.imageLayout = depth_img->layout();
    depth_attachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    depth_attachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    depth_attachment.clearValue = clear_depth;

    VkRenderingInfo render_info{};
    render_info.sType = VK_STRUCTURE_TYPE_RENDERING_INFO;
    render_info.renderArea = VkRect2D{{0, 0}, draw_img->extent()};
    render_info.layerCount = 1;
    render_info.colorAttachmentCount = 1;
    render_info.pColorAttachments = &color_attachment;
    render_info.pDepthAttachment = &depth_attachment;

    vkCmdBeginRendering(cmd, &render_info);
    GraphicsPipline *chosen_pipeline;

    switch (type)
    {
    case PrimitiveType3D::Sphere: {
        chosen_pipeline = &m_sphere_pipeline;
        break;
    }
    };

    vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, chosen_pipeline->pipeline());
    vkCmdBindDescriptorSets(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, chosen_pipeline->layout(), 0, m_resources.sets_size(),
                            m_resources.sets(), 0, nullptr);

    VkViewport viewport{};
    viewport.width = render_info.renderArea.extent.width;
    viewport.height = render_info.renderArea.extent.height;
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;

    VkRect2D scissor{};
    scissor.offset = {0, 0};
    scissor.extent = render_info.renderArea.extent;

    vkCmdSetViewport(cmd, 0, 1, &viewport);
    vkCmdSetScissor(cmd, 0, 1, &scissor);
    vkCmdBindIndexBuffer(cmd, m_quad.index_buffer().buffer(), 0, VK_INDEX_TYPE_UINT32);

    glm::mat4 perspective = camera.projection();
    glm::mat4 view = camera.view();

    // TODO: Write it using instancing
    for (auto &&param : parameters)
    {
        m_push3d.color = param.color;

        glm::mat4 model;
        model = glm::translate(model, param.position);

        model[0][0] *= param.scale;
        model[1][1] *= param.scale;
        model[2][2] *= param.scale;

        m_push3d.pvm_matrix = perspective * view * model;

        vkCmdPushConstants(cmd, m_circle_pipeline.layout(), VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
                           0, sizeof(PushConstants3D), &m_push3d);

        vkCmdDrawIndexed(cmd, m_quad.index_count(), 1, 0, 0, 0);
    }
    vkCmdEndRendering(cmd);
}
} // namespace ving
