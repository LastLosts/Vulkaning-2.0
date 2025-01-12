#pragma once

#include "compute_pipeline.hpp"
#include "gpu_buffer.hpp"
#include "render_frames.hpp"
#include "shader_resources.hpp"
#include <array>

namespace ving
{
struct SlimeSettings
{
    float movement_speed = 0.05;
    int sensor_distance = 10;
    int sensor_size = 5;
    float sensor_angle_spacing = 0.25;
    float offset_size = 0.0001;
    float turn_speed = 0.1;
};

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

    // Must be multiple of 256
    static constexpr uint32_t slime_local_dispatch = 32;
    static constexpr uint32_t agent_multiplicator = 30000;
    static constexpr uint32_t agent_count = slime_local_dispatch * agent_multiplicator;

    SlimeSettings *settings;

  private:
    PushConstants m_push_constants;

    Texture2D m_slime_img;
    std::vector<Agent> m_agents;
    GPUBuffer m_agents_buffer;

    GPUBuffer m_settings_uniform;

    ShaderResources m_resources;
    ComputePipeline m_fade_diffuse_pipeline;
    ComputePipeline m_agent_update_pipeline;
};
} // namespace ving
