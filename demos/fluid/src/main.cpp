#include <iostream>

#include "particle_grid.hpp"

#include "compute_pipeline.hpp"
#include "engine.hpp"
#include "primitives_renderer.hpp"

#include <glm/gtc/random.hpp>
#include <glm/gtc/type_ptr.hpp>

constexpr float gravity = 900.0f;
static glm::vec3 particle_color = {0.1f, 0.1f, 0.1f};

static float collision_damping = 0.9f;
static float particle_spacing = 15.0f;
static int particle_count = 400;

static std::vector<ving::PrimitiveParameters> primitive_parameters{};

struct PushConstants
{
    int particle_count;
    float smoothing_radius;
    float target_density;
    float pressure_multiplier;
};

// Minimum smoothing radius is 0.1f because radix sort set up for only 2 digit numbers
static PushConstants push{particle_count, 0.3f, 100.0f, 0.001f};

int main()
{
    ving::Engine engine{};
    ving::PrimitivesRenderer renderer{engine.core()};

    ParticleGrid grid{push.smoothing_radius};
    grid.generate_particles_random(particle_count);
    primitive_parameters.reserve(particle_count);

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

    ving::GPUBuffer particles_buffer{engine.core(), grid.particles().size() * sizeof(Particle),
                                     VK_BUFFER_USAGE_STORAGE_BUFFER_BIT, VMA_MEMORY_USAGE_CPU_TO_GPU};
    ving::Texture2D background{engine.core(),
                               VkExtent2D{ving::Engine::initial_window_width, ving::Engine::initial_window_height},
                               VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_STORAGE_BIT, VMA_MEMORY_USAGE_GPU_ONLY,
                               ving::RenderFrames::draw_image_format};

    particles_buffer.set_memory(grid.particles().data(), grid.particles().size() * sizeof(glm::vec4));

    ving::ShaderResources resources{engine.core().device(), bindings, VK_SHADER_STAGE_COMPUTE_BIT};

    resources.write_image(0, 0, background, VK_IMAGE_LAYOUT_GENERAL);
    resources.write_buffer(0, 1, particles_buffer);

    ving::ComputePipeline density_background_render{engine.core(), resources, fluid_density, sizeof(PushConstants)};

    void *gpu_particles_buffer = particles_buffer.map_and_get_memory();

    std::vector<ving::PrimitiveParameters> primitive_parameters_2;

    float smoothing_in_pixels_x = push.smoothing_radius * ving::Engine::initial_window_width;
    float smoothing_in_pixels_y = push.smoothing_radius * ving::Engine::initial_window_height;

    while (engine.running())
    {
        grid.update(push.smoothing_radius, push.target_density, push.pressure_multiplier, engine.delta_time());

        primitive_parameters.clear();

        for (auto &&p : grid.particles())
        {
            primitive_parameters.push_back({{p.position.x * ving::Engine::initial_window_width,
                                             p.position.y * ving::Engine::initial_window_height},
                                            ParticleGrid::particle_scale * ving::Engine::initial_window_height,
                                            particle_color});
        }

#if 0
        std::vector<std::span<Particle>> ps = grid.get_neighbour_particles(cell_coords);
        std::vector<uint32_t> ind = grid.get_neighbour_particle_indices(cell_coords);
        if (render_p)
        {
            uint32_t ctr = 0;
            for (auto &&range : ps)
            {
                for (auto &&p : range)
                {
                    primitive_parameters.push_back({{p.position.x * ving::Engine::initial_window_width,
                                                     p.position.y * ving::Engine::initial_window_height},
                                                    ParticleGrid::particle_scale * ving::Engine::initial_window_height,
                                                    particle_color});
                    ctr++;
                }
            }

            std::cout << "Actual: " << ctr << '\n';
        }
        else
        {
            for (auto &&i : ind)
            {
                const Particle &p = grid.particles()[i];
                primitive_parameters.push_back({{p.position.x * ving::Engine::initial_window_width,
                                                 p.position.y * ving::Engine::initial_window_height},
                                                ParticleGrid::particle_scale * ving::Engine::initial_window_height,
                                                particle_color});
            }
        }
#endif

        memcpy(gpu_particles_buffer, grid.particles().data(), grid.particles().size() * sizeof(glm::vec4));

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
        renderer.render(frame, ving::PrimitiveType::Circle, primitive_parameters);
        /*renderer.render(frame, ving::PrimitiveType::Square, primitive_parameters_2);*/

        engine.imgui_renderer().render(
            frame, {
                       [&engine, &grid]() {
                           ImGui::Text("%f", engine.delta_time() * 1000.0f);
                           ImGui::ColorEdit3("Particle Color", glm::value_ptr(particle_color));
                           ImGui::DragFloat("Target Density", &push.target_density, 0.1f, 0.0f);
                           ImGui::DragFloat("Pressure multiplier", &push.pressure_multiplier, 0.01f, 0.0f);

                           /*ImGui::Text("%f, %f", grid.particles()[0].position.x, grid.particles()[0].position.y);*/
                           /*ImGui::Text("%f, %f", grid.particles()[0].velocity.x, grid.particles()[0].velocity.y);*/
                           /*ImGui::Text("%f", grid.particles()[0].density);*/

                           /*ImGui::DragInt2("Coord: ", glm::value_ptr(cell_coords), 1, 0, grid.cells_size() - 1);*/
                           /*ImGui::Checkbox("Actual:", &render_p);*/

                           /*ImGui::DragFloat("Particle Spacing", &particle_spacing, 1, 1, 10);*/
                           /*ImGui::DragFloat("Particle Scale", &particle_scale);*/
                           /*ImGui::DragInt("Particle Count", &particle_count, 1, 1, 1000);*/
                           /*ImGui::DragFloat("Smoothing radius", &push.smoothing_radius, 0.001f,*/
                           /*                 0.01f, 5.0f);*/
                       },
                   });
        engine.end_rendering(frame);
        engine.end_frame(frame);
    }

#ifndef NDEBUG
    vkDestroyShaderModule(engine.core().device(), fluid_density, nullptr);
    vkDeviceWaitIdle(engine.core().device());
#endif
}
