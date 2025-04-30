#include "mesh_renderer.hpp"
#include "utility/vulkan_utils.hpp"

namespace ving
{
MeshRenderer::MeshRenderer(const VulkanCore &core)
{
    std::vector<ving::ShaderBindingSet> bindings{
        ving::ShaderBindingSet{{}},
    };

    VkShaderModule mesh_vertex_shader;
    if (!load_vulkan_shader_module(core.device(), "./engine/shaders/spirv/mesh.vert.spv", mesh_vertex_shader))
    {
        exit(-1);
    }

    VkShaderModule fragment;
    if (!load_vulkan_shader_module(core.device(), "./engine/shaders/spirv/mesh.frag.spv", fragment))
    {
        exit(-1);
    }

    m_resources = ShaderResources{core.device(), bindings, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT};

    m_mesh_pipeline =
        GraphicsPipeline{core, m_resources, mesh_vertex_shader, fragment, sizeof(PushConstants), VK_POLYGON_MODE_FILL};
    vkDestroyShaderModule(core.device(), mesh_vertex_shader, nullptr);
    vkDestroyShaderModule(core.device(), fragment, nullptr);
}

void MeshRenderer::render(const FrameInfo &frame, const PerspectiveCamera &camera, std::span<Mesh> meshes)
{
    VkCommandBuffer cmd = frame.cmd;
    Texture2D *draw_img = frame.draw_img;

    vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, m_mesh_pipeline.pipeline());
    vkCmdBindDescriptorSets(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, m_mesh_pipeline.layout(), 0, m_resources.sets_size(),
                            m_resources.sets(), 0, nullptr);

    PushConstants push{};

    for (auto &&mesh : meshes)
    {
        push.pvm_matrix = camera.projection() * camera.view();
        push.vertex_buffer_address = mesh.vertex_address();
        vkCmdBindIndexBuffer(cmd, mesh.index_buffer().buffer(), 0, VK_INDEX_TYPE_UINT32);
        vkCmdPushConstants(cmd, m_mesh_pipeline.layout(), VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 0,
                           sizeof(PushConstants), &push);
        vkCmdDrawIndexed(cmd, mesh.index_count(), 1, 0, 0, 0);
    }
}
} // namespace ving
