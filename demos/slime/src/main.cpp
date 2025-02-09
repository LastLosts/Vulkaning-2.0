#include "compute_pipeline.hpp"
#include "engine.hpp"
#include "shader_resources.hpp"
#include <chrono>
#include <numbers>
#include <random>

static constexpr uint32_t slime_local_dispatch = 32;
static constexpr uint32_t agent_multiplicator = 30000;
static constexpr uint32_t agent_count = slime_local_dispatch * agent_multiplicator;

struct Agent
{
    // From 0 to 1
    float x;
    float y;

    float angle;
    float dummy;
};

struct SlimeSettings
{
    float movement_speed = 0.05;
    int sensor_distance = 10;
    int sensor_size = 5;
    float sensor_angle_spacing = 0.25;
    float offset_size = 0.0001;
    float turn_speed = 0.1;
};
struct PushConstants
{
    float time;
    float delta_time;
};

int main()
{
    float time, delta_time;

    ving::Engine engine{};
    const ving::ImGuiRenderer &imgui_renderer = engine.imgui_renderer();

    std::vector<Agent> agents{};
    agents.resize(agent_count);

    std::default_random_engine gen;
    std::uniform_real_distribution<float> dist(0.0f, 1.0f);
    std::uniform_real_distribution<float> angle_dist(0.0f, std::numbers::pi * 2);

    for (size_t i = 0; i < agents.size(); ++i)
    {
        agents[i].x = dist(gen);
        agents[i].y = dist(gen);

        agents[i].angle = angle_dist(gen);

        assert(agents[i].x >= 0.0f && agents[i].x <= 1.0f);
        assert(agents[i].y >= 0.0f && agents[i].y <= 1.0f);
    }

    // Working directory must be vulkaning

    VkShaderModule slime_shader;
    if (!engine.load_shader("./demos/slime/shaders/spirv/slime.comp.spv", slime_shader))
    {
        // Handle shader file not opening
        exit(-1);
    }

    VkShaderModule fade_diffuse_shader;
    if (!engine.load_shader("./demos/slime/shaders/spirv/fade_and_diffuse.comp.spv", fade_diffuse_shader))
    {
        exit(-1);
    }

    ving::GPUBuffer staging_buffer{engine.core(), sizeof(Agent) * agent_count, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                                   VMA_MEMORY_USAGE_CPU_ONLY};
    ving::GPUBuffer agents_buffer{engine.core(), sizeof(Agent) * agent_count,
                                  VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT,
                                  VMA_MEMORY_USAGE_GPU_ONLY};
    ving::GPUBuffer settings_uniform{engine.core(), sizeof(SlimeSettings), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                                     VMA_MEMORY_USAGE_CPU_TO_GPU};

    ving::Texture2D slime_img{engine.core(),
                              {2560, 1440},
                              VK_IMAGE_USAGE_STORAGE_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT,
                              VMA_MEMORY_USAGE_GPU_ONLY,
                              VK_FORMAT_R32G32B32A32_SFLOAT};

    staging_buffer.set_memory(agents.data(), sizeof(Agent) * agents.size());
    engine.copy_buffer_to_buffer_immediate(staging_buffer, agents_buffer);

    SlimeSettings *settings;
    void *data = settings_uniform.map_and_get_memory();
    settings = static_cast<SlimeSettings *>(data);

    settings->movement_speed = 0.05;
    settings->sensor_distance = 10;
    settings->sensor_size = 5;
    settings->sensor_angle_spacing = 0.25;
    settings->offset_size = 0.0001;
    settings->turn_speed = 0.1;

    std::vector<ving::ShaderBindingSet> sets{
        ving::ShaderBindingSet{{
            {0, VK_DESCRIPTOR_TYPE_STORAGE_IMAGE},
            {1, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER},
            {2, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER},
        }},
    };
    ving::ShaderResources resources{engine.core().device(), sets, VK_SHADER_STAGE_COMPUTE_BIT};

    resources.write_image(0, 0, slime_img, VK_IMAGE_LAYOUT_GENERAL);
    resources.write_buffer(0, 1, agents_buffer);
    resources.write_buffer(0, 2, settings_uniform);

    ving::ComputePipeline fade_diffuse_pipeline{engine.core(), resources, fade_diffuse_shader, sizeof(PushConstants)};
    ving::ComputePipeline agent_update_pipeline{
        engine.core(),
        resources,
        slime_shader,
        sizeof(PushConstants),
    };

    vkDestroyShaderModule(engine.core().device(), slime_shader, nullptr);
    vkDestroyShaderModule(engine.core().device(), fade_diffuse_shader, nullptr);

    PushConstants push_constants{};

    while (engine.running())
    {
        auto start = std::chrono::high_resolution_clock::now();

        ving::FrameInfo frame = engine.begin_frame();
        ving::Texture2D *draw_img = frame.draw_img;
        VkCommandBuffer cmd = frame.cmd;

        draw_img->transition_layout(cmd, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);

        push_constants.time = time;
        push_constants.delta_time = delta_time;

        slime_img.transition_layout(cmd, VK_IMAGE_LAYOUT_GENERAL);

        agent_update_pipeline.dispatch(cmd, resources, &push_constants, agent_multiplicator);

        fade_diffuse_pipeline.dispatch(cmd, resources, &push_constants, std::ceil(slime_img.extent().width / 16.0f),
                                       std::ceil(slime_img.extent().height / 16.0f));

        draw_img->transition_layout(cmd, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
        slime_img.transition_layout(cmd, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL);

        slime_img.copy_to(cmd, *draw_img);

        imgui_renderer.render(frame, {[]() { ImGui::Text("Helo"); }});

        engine.end_frame(frame);

        auto end = std::chrono::high_resolution_clock::now();

        std::chrono::duration<float> dur = end - start;
        delta_time = dur.count();
        time += delta_time;
    }

    return 0;
}
