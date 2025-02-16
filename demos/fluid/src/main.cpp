#include <iostream>
#include <random>

#include "fluid_simulation.hpp"

#include "compute_pipeline.hpp"
#include "engine.hpp"
#include "primitives_renderer.hpp"

#include <glm/gtc/random.hpp>
#include <glm/gtc/type_ptr.hpp>

constexpr float gravity = 900.0f;
static glm::vec3 particle_color = {0.1f, 0.1f, 0.1f};

static float collision_damping = 0.9f;
static float particle_scale = 10.0f;
static float particle_spacing = 15.0f;
static int particle_count = 400;

static std::vector<ving::PrimitiveParameters> primitive_parameters{};
static std::vector<glm::vec2> velocities{};
static std::vector<glm::vec4> positions{};

struct PushConstants
{
    int particle_count;
    float smoothing_radius;
    float target_density;
    float pressure_multiplier;
};

static PushConstants push{particle_count, 0.5f, 100.0f, 3.5f};

static void generate_particles_random()
{
    velocities.clear();
    primitive_parameters.resize(particle_count);
    velocities.resize(particle_count);

    std::random_device dev{};
    std::default_random_engine el{dev()};

    std::uniform_real_distribution<float> uniform_dist_height{
        particle_scale / 2.0f, ving::Engine::initial_window_height - particle_scale / 2.0f};
    std::uniform_real_distribution<float> uniform_dist_width{particle_scale / 2.0f, ving::Engine::initial_window_width -
                                                                                        particle_scale / 2.0f};

    for (uint32_t i = 0; i < particle_count; ++i)
    {
        primitive_parameters[i].position = {uniform_dist_width(el), uniform_dist_height(el)};
        primitive_parameters[i].scale = particle_scale;
        primitive_parameters[i].color = {0.1f, 0.1f, 0.9f};
    }
}

static void generate_particles_cube()
{
    velocities.clear();
    primitive_parameters.resize(particle_count);
    velocities.resize(particle_count);

    uint32_t particle_per_row = (int)sqrt(particle_count);
    uint32_t particle_per_col = (particle_count - 1) / particle_per_row + 1;

    float spacing = particle_scale + particle_spacing;

    for (uint32_t i = 0; i < particle_count; ++i)
    {
        primitive_parameters[i].position = {(i % particle_per_row) * spacing + particle_scale,
                                            (i / particle_per_row) * spacing + particle_scale};
        primitive_parameters[i].scale = particle_scale;
        primitive_parameters[i].color = {0.1f, 0.1f, 0.9f};
    }
}

static void update_particles(const ving::Engine &engine, float smoothing_radius)
{
    for (size_t i = 0; i < particle_count; ++i)
    {
        positions[i].z = calculate_density(positions, i, smoothing_radius);
    }

    for (size_t i = 0; i < particle_count; ++i)
    {
        velocities[i] =
            calculate_pressure_force(positions, i, smoothing_radius, push.target_density, push.pressure_multiplier);

        constexpr bool gravity_on = true;

        if constexpr (gravity_on)
            velocities[i] += glm::vec2{0.0f, 1.0f} * gravity * engine.delta_time();

        primitive_parameters[i].position += velocities[i] * engine.delta_time();
        positions[i].x += velocities[i].x * engine.delta_time() / ving::Engine::initial_window_width;
        positions[i].y += velocities[i].y * engine.delta_time() / ving::Engine::initial_window_height;

        constexpr bool collision = true;
        if constexpr (collision)
        {
            glm::vec2 bounds_max{(float)ving::Engine::initial_window_width - primitive_parameters[i].scale / 2.0f,
                                 (float)ving::Engine::initial_window_height - primitive_parameters[i].scale / 2.0f};

            glm::vec2 bounds_min{primitive_parameters[i].scale, primitive_parameters[i].scale};

            if (primitive_parameters[i].position.x > bounds_max.x)
            {
                primitive_parameters[i].position.x = bounds_max.x;
                velocities[i].x *= -1 * collision_damping;
            }
            if (primitive_parameters[i].position.y > bounds_max.y)
            {
                primitive_parameters[i].position.y = bounds_max.y;
                velocities[i].y *= -1 * collision_damping;
            }
            if (primitive_parameters[i].position.x < bounds_min.x)
            {
                primitive_parameters[i].position.x = bounds_min.x;
                velocities[i].x *= -1 * collision_damping;
            }
            if (primitive_parameters[i].position.y < bounds_min.y)
            {
                primitive_parameters[i].position.y = bounds_min.y;
                velocities[i].y *= -1 * collision_damping;
            }
        }
    }
}

std::vector<uint32_t> cells;

int main()
{
    ving::Engine engine{};
    ving::PrimitivesRenderer renderer{engine.core()};

    /*generate_particles_cube();*/
    generate_particles_random();

    bool simulate = false;

    VkShaderModule fluid_density{};
    if (!engine.load_shader("./demos/fluid/shaders/spirv/fluid_density.comp.spv", fluid_density))
    {
        std::cout << "Failed to load shader\n";
    }

    std::vector<ving::ShaderBindingSet> bindings{
        ving::ShaderBindingSet{
            {
                {0, VK_DESCRIPTOR_TYPE_STORAGE_IMAGE},
                {1, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER},
            },
        },
    };

    positions.reserve(particle_count);

    for (uint32_t i = 0; i < particle_count; ++i)
    {
        glm::vec2 particle_position_screen_space{
            primitive_parameters[i].position.x / (float)ving::Engine::initial_window_width,
            primitive_parameters[i].position.y / (float)ving::Engine::initial_window_height};

        positions.push_back({particle_position_screen_space, 0.0, 0.0});
    }

    ving::GPUBuffer particles_buffer{engine.core(), positions.size() * sizeof(glm::vec4),
                                     VK_BUFFER_USAGE_STORAGE_BUFFER_BIT, VMA_MEMORY_USAGE_CPU_TO_GPU};
    ving::Texture2D background{engine.core(),
                               VkExtent2D{ving::Engine::initial_window_width, ving::Engine::initial_window_height},
                               VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_STORAGE_BIT, VMA_MEMORY_USAGE_GPU_ONLY,
                               ving::RenderFrames::draw_image_format};

    particles_buffer.set_memory(positions.data(), positions.size() * sizeof(glm::vec4));

    ving::ShaderResources resources{engine.core().device(), bindings, VK_SHADER_STAGE_COMPUTE_BIT};

    resources.write_image(0, 0, background, VK_IMAGE_LAYOUT_GENERAL);
    resources.write_buffer(0, 1, particles_buffer);

    ving::ComputePipeline density_background_render{engine.core(), resources, fluid_density, sizeof(PushConstants)};

    void *gpu_particles_buffer = particles_buffer.map_and_get_memory();
    while (engine.running())
    {
        memcpy(gpu_particles_buffer, positions.data(), positions.size() * sizeof(glm::vec4));

        if (true)
            update_particles(engine, push.smoothing_radius);

        ving::FrameInfo frame = engine.begin_frame();

        // Background rendering
        background.transition_layout(frame.cmd, VK_IMAGE_LAYOUT_GENERAL);
        density_background_render.dispatch(frame.cmd, resources, &push, std::ceil(background.extent().width / 16.0f),
                                           std::ceil(background.extent().height / 16.0f));

        background.transition_layout(frame.cmd, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL);
        frame.draw_img->transition_layout(frame.cmd, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
        background.copy_to(frame.cmd, *frame.draw_img);

        // TODO: Some way to tell the engine not to clear the image
        engine.begin_rendering(frame, false);

        renderer.render(frame, ving::PrimitiveType::Circle, primitive_parameters, particle_color);
        engine.imgui_renderer().render(frame, {[&engine]() {
                                           ImGui::Text("%f", engine.delta_time() * 1000.0f);
                                           ImGui::ColorEdit3("Particle Color", glm::value_ptr(particle_color));
                                           ImGui::DragFloat("Target Density", &push.target_density, 0.1f, 0.0f);
                                           ImGui::DragFloat("Pressure multiplier", &push.pressure_multiplier, 0.01f,
                                                            0.0f);

                                           /*ImGui::DragFloat("Particle Spacing", &particle_spacing, 1, 1, 10);*/
                                           /*ImGui::DragFloat("Particle Scale", &particle_scale);*/
                                           /*ImGui::DragInt("Particle Count", &particle_count, 1, 1, 1000);*/
                                           /*ImGui::DragFloat("Smoothing radius", &push.smoothing_radius, 0.001f,*/
                                           /*                 0.01f, 5.0f);*/
                                       }});
        engine.end_rendering(frame);
        engine.end_frame(frame);
    }

#ifndef NDEBUG
    vkDestroyShaderModule(engine.core().device(), fluid_density, nullptr);
    vkDeviceWaitIdle(engine.core().device());
#endif
}
