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
    struct PushConstants
    {
        float time;
        float delta_time;
    };

  public:
    SlimeRenderer(const VulkanCore &core);

    void render(const RenderFrames::FrameInfo &frame, float time, float delta_time);

    static constexpr uint32_t agent_count = 100000;

  private:
    PushConstants m_push_constants;

    Texture2D m_slime_img;
    std::vector<Agent> m_agents;
    GPUBuffer m_agents_buffer;

    ShaderResources m_resources;
    ComputePipeline m_fade_diffuse_pipeline;
    ComputePipeline m_agent_update_pipeline;
};
} // namespace ving
