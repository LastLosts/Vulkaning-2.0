#include "slime_renderer.hpp"
#include "utility/vulkan_utils.hpp"
#include <vulkan/vulkan_core.h>

namespace ving
{

SlimeRenderer::SlimeRenderer(const VulkanCore &core)
    : m_slime_img{core,
                  {1280, 720},
                  VK_IMAGE_USAGE_STORAGE_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT,
                  VMA_MEMORY_USAGE_GPU_ONLY,
                  VK_FORMAT_R32G32B32A32_SFLOAT}
{
    std::vector<ShaderBindingSet> sets{
        ShaderBindingSet{"",
                         {
                             {0, VK_DESCRIPTOR_TYPE_STORAGE_IMAGE},
                         }},
    };

    VkShaderModule shader =
        load_vulkan_shader_module(core.device(), "C:/Users/Artem/coding/vulkaning/engine/shaders/spirv/slime.comp.spv");
    m_resources = ShaderResources{core.device(), sets, VK_SHADER_STAGE_COMPUTE_BIT};
    m_resources.write_image(0, 0, m_slime_img, VK_IMAGE_LAYOUT_GENERAL);

    m_pipeline = ComputePipeline{core, m_resources, shader};
    vkDestroyShaderModule(core.device(), shader, nullptr);
}
void SlimeRenderer::render(const RenderFrames::FrameInfo &frame)
{
    VkCommandBuffer cmd = frame.cmd;
    Texture2D *draw_image = frame.draw_img;

    m_slime_img.transition_layout(cmd, VK_IMAGE_LAYOUT_GENERAL);

    vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_COMPUTE, m_pipeline.pipeline());
    vkCmdBindDescriptorSets(cmd, VK_PIPELINE_BIND_POINT_COMPUTE, m_pipeline.layout(), 0, m_resources.sets_size(),
                            m_resources.sets(), 0, nullptr);
    vkCmdDispatch(cmd, std::ceil(m_slime_img.extent().width / 16.0), std::ceil(m_slime_img.extent().height / 16.0), 1);

    draw_image->transition_layout(cmd, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
    m_slime_img.transition_layout(cmd, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL);
    copy_image_to_image(cmd, m_slime_img.image(), draw_image->image(), m_slime_img.extent(), draw_image->extent());
}
} // namespace ving
