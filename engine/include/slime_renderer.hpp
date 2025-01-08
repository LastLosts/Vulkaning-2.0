#pragma once

#include "compute_pipeline.hpp"
#include "gpu_buffer.hpp"
#include "render_frames.hpp"
#include "shader_resources.hpp"
#include <array>

namespace ving
{
class SlimeRenderer
{
    struct Agent
    {
        // From 0 to 1
        float x;
        float y;

        float angle;
        float dummy;
    };

  public:
    SlimeRenderer(const VulkanCore &core);

    void render(const RenderFrames::FrameInfo &frame);

    static constexpr uint32_t agent_count = 1000;

  private:
    Texture2D m_slime_img;
    std::array<Agent, agent_count> m_agents;
    GPUBuffer m_agents_buffer;

    ShaderResources m_resources;
    ComputePipeline m_fade_diffuse_pipeline;
    ComputePipeline m_agent_update_pipeline;
};
} // namespace ving
