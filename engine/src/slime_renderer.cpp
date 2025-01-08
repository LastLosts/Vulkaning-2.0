#include "slime_renderer.hpp"
#include "utility/vulkan_utils.hpp"
#include <cassert>
#include <numbers>
#include <random>
#include <vulkan/vulkan_core.h>

namespace ving
{

SlimeRenderer::SlimeRenderer(const VulkanCore &core)
    : m_slime_img{core,
                  {1280, 720},
                  VK_IMAGE_USAGE_STORAGE_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT,
                  VMA_MEMORY_USAGE_GPU_ONLY,
                  VK_FORMAT_R32G32B32A32_SFLOAT},
      m_agents_buffer{core, sizeof(Agent) * agent_count,
                      VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT, VMA_MEMORY_USAGE_GPU_ONLY}
{
    std::default_random_engine gen;
    std::uniform_real_distribution<float> dist(0.0f, 1.0f);
    std::uniform_real_distribution<float> angle_dist(0.0f, std::numbers::pi * 2);

    for (size_t i = 0; i < m_agents.size(); ++i)
    {
        m_agents[i].x = dist(gen);
        m_agents[i].y = dist(gen);

        m_agents[i].angle = angle_dist(gen);

        assert(m_agents[i].x >= 0.0f && m_agents[i].x <= 1.0f);
        assert(m_agents[i].y >= 0.0f && m_agents[i].y <= 1.0f);
    }

    std::vector<ShaderBindingSet> sets{
        ShaderBindingSet{{
            {0, VK_DESCRIPTOR_TYPE_STORAGE_IMAGE},
            {1, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER},
        }},
    };

    // Working directory must be vulkaning
    VkShaderModule update_slime_shader =
        load_vulkan_shader_module(core.device(), "./engine/shaders/spirv/slime.comp.spv");
    VkShaderModule fade_diffuse_shader =
        load_vulkan_shader_module(core.device(), "./engine/shaders/spirv/fade_and_diffuse.comp.spv");

    m_resources = ShaderResources{core.device(), sets, VK_SHADER_STAGE_COMPUTE_BIT};
    m_resources.write_image(0, 0, m_slime_img, VK_IMAGE_LAYOUT_GENERAL);
    m_resources.write_buffer(0, 1, m_agents_buffer);

    GPUBuffer staging_buffer{core, sizeof(Agent) * agent_count, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                             VMA_MEMORY_USAGE_CPU_ONLY};
    staging_buffer.set_memory(m_agents.data(), sizeof(Agent) * m_agents.size());

    core.immediate_transfer([&staging_buffer, this](VkCommandBuffer cmd) {
        VkBufferCopy copy{};
        copy.size = sizeof(Agent) * agent_count;
        copy.dstOffset = 0;
        copy.srcOffset = 0;

        vkCmdCopyBuffer(cmd, staging_buffer.buffer(), m_agents_buffer.buffer(), 1, &copy);
    });

    m_fade_diffuse_pipeline = ComputePipeline{core, m_resources, fade_diffuse_shader};
    m_agent_update_pipeline = ComputePipeline{core, m_resources, update_slime_shader};

    vkDestroyShaderModule(core.device(), update_slime_shader, nullptr);
    vkDestroyShaderModule(core.device(), fade_diffuse_shader, nullptr);
}
void SlimeRenderer::render(const RenderFrames::FrameInfo &frame)
{
    VkCommandBuffer cmd = frame.cmd;
    Texture2D *draw_image = frame.draw_img;

    m_slime_img.transition_layout(cmd, VK_IMAGE_LAYOUT_GENERAL);

    vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_COMPUTE, m_agent_update_pipeline.pipeline());
    vkCmdBindDescriptorSets(cmd, VK_PIPELINE_BIND_POINT_COMPUTE, m_agent_update_pipeline.layout(), 0,
                            m_resources.sets_size(), m_resources.sets(), 0, nullptr);
    vkCmdDispatch(cmd, agent_count, 1, 1);

    vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_COMPUTE, m_fade_diffuse_pipeline.pipeline());
    vkCmdBindDescriptorSets(cmd, VK_PIPELINE_BIND_POINT_COMPUTE, m_fade_diffuse_pipeline.layout(), 0,
                            m_resources.sets_size(), m_resources.sets(), 0, nullptr);
    vkCmdDispatch(cmd, std::ceil(1280.0f / 16.0f), std::ceil(720.0f / 16.0f), 1);

    draw_image->transition_layout(cmd, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
    m_slime_img.transition_layout(cmd, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL);
    copy_image_to_image(cmd, m_slime_img.image(), draw_image->image(), m_slime_img.extent(), draw_image->extent());
}
} // namespace ving
