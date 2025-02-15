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

void PrimitivesRenderer::render(const FrameInfo &frame, PrimitiveType type, std::span<PrimitiveParameters> parameters,
                                glm::vec3 color)
{
    VkCommandBuffer cmd = frame.cmd;
    Texture2D *draw_img = frame.draw_img;

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

    vkCmdBindIndexBuffer(cmd, m_quad.index_buffer().buffer(), 0, VK_INDEX_TYPE_UINT32);

    // TODO: Write it using instancing
    for (auto &&param : parameters)
    {
        m_push.position = param.position;
        m_push.scale = param.scale;
        m_push.color = param.color;

        if (color != glm::vec3{-1})
        {
            m_push.color = color;
        }

        vkCmdPushConstants(cmd, m_circle_pipeline.layout(), VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
                           0, sizeof(PushConstants), &m_push);

        vkCmdDrawIndexed(cmd, m_quad.index_count(), 1, 0, 0, 0);
    }
}
} // namespace ving
