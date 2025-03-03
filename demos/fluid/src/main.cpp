#include <iostream>

#include "particle_grid.hpp"

#include "compute_pipeline.hpp"
#include "engine.hpp"
#include "primitives_renderer.hpp"

#include <glm/gtc/random.hpp>
#include <glm/gtc/type_ptr.hpp>

static glm::vec3 particle_color = {0.1f, 0.1f, 0.1f};

// TODO: Fixed time step

static float collision_damping = 0.9f;
static float particle_spacing = 15.0f;
static int particle_count = 700;

static std::vector<ving::PrimitiveParameters> primitive_parameters{};

struct PushConstants
{
    int particle_count;
    float smoothing_radius;
    float target_density;
    float pressure_multiplier;
};

// Minimum smoothing radius is more then 0.1f because radix sort set up for only 2 digit numbers
static PushConstants push{particle_count, 0.05f, 100.0f, 0.0001f};

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

    ving::ShaderResources resources{engine.core().device(), bindings, VK_SHADER_STAGE_COMPUTE_BIT};

    resources.write_image(0, 0, background, VK_IMAGE_LAYOUT_GENERAL);
    resources.write_buffer(0, 1, particles_buffer);

    ving::ComputePipeline density_background_render{engine.core(), resources, fluid_density, sizeof(PushConstants)};

    void *gpu_particles_buffer = particles_buffer.map_and_get_memory();

    std::vector<ving::PrimitiveParameters> primitive_parameters_2;

    float smoothing_in_pixels_x = push.smoothing_radius * ving::Engine::initial_window_width;
    float smoothing_in_pixels_y = push.smoothing_radius * ving::Engine::initial_window_height;

    float cpu_update_time;

    assert(particles_buffer.size() == sizeof(Particle) * particle_count);

    while (engine.running())
    {
        primitive_parameters.clear();

        for (auto &&p : grid.particles())
        {
            primitive_parameters.push_back(
                {{p.position.x * ving::Engine::initial_window_width,
                  p.position.y * ving::Engine::initial_window_height},
                 ParticleGrid::particle_scale * ving::Engine::initial_window_height,
                 glm::mix(glm::vec3{0.1f, 0.1f, 0.8f}, glm::vec3{0.8f, 0.1f, 0.1f}, glm::length(p.velocity))});
        }

        ving::FrameInfo frame = engine.begin_frame();

        auto s = std::chrono::high_resolution_clock::now();
        memcpy(gpu_particles_buffer, grid.particles().data(), grid.particles().size() * sizeof(Particle));
        auto e = std::chrono::high_resolution_clock::now();
        std::chrono::duration<float> dur = e - s;
        float copying_buffer_time = dur.count();

        s = std::chrono::high_resolution_clock::now();
        grid.generate_grid();

        // Render particles close to mouse
        Particle part{
            {engine.cursor_pos().x / (float)ving::Engine::initial_window_width,
             engine.cursor_pos().y / (float)ving::Engine::initial_window_height},
            {},
            ParticleGrid::particle_scale * ving::Engine::initial_window_height,
        };

        std::vector<uint32_t> indices = grid.get_neighbour_particle_indices(part);
        for (auto &&i : indices)
        {
            primitive_parameters[i].color = {1.0f, 0.0f, 1.0f};
        }

        std::vector<TimeResult> results{};
        if (simulate)
        {
            results = grid.update_particles(push.smoothing_radius, push.target_density, push.pressure_multiplier,
                                            engine.delta_time());
        }

        e = std::chrono::high_resolution_clock::now();
        dur = e - s;
        cpu_update_time = dur.count();

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
                       [&engine, &grid, cpu_update_time, &simulate, copying_buffer_time, &results]() {
                           ImGui::Text("%f", engine.delta_time() * 1000.0f);
                           ImGui::Text("Cpu update: %f", cpu_update_time * 1000.0f);
                           ImGui::Text("Copying buffer: %f", copying_buffer_time * 1000.0f);
                           ImGui::ColorEdit3("Particle Color", glm::value_ptr(particle_color));
                           ImGui::DragFloat("Target Density", &push.target_density, 0.1f, 0.0f);
                           ImGui::DragFloat("Pressure multiplier", &push.pressure_multiplier, 0.01f, 0.0f);

                           ImGui::Text("%f, %f", engine.cursor_pos().x, engine.cursor_pos().y);

                           for (auto &&r : results)
                           {
                               std::string format = r.name + ": %f";
                               ImGui::Text(format.c_str(), r.time);
                           }

                           /*ImGui::Text("%f, %f", grid.particles()[0].position.x, grid.particles()[0].position.y);*/
                           /*ImGui::Text("%f, %f", grid.particles()[0].velocity.x, grid.particles()[0].velocity.y);*/
                           /*ImGui::Text("%f", grid.particles()[0].density);*/

                           /*ImGui::DragInt2("Coord: ", glm::value_ptr(cell_coords), 1, 0, grid.cells_size() - 1);*/
                           ImGui::Checkbox("Simulate:", &simulate);

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

    vkDestroyShaderModule(engine.core().device(), fluid_density, nullptr);
    vkDeviceWaitIdle(engine.core().device());
}
