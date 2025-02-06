#include "engine.hpp"
#include "imgui.h"
#include "primitives_renderer.hpp"
#include <iostream>

constexpr float gravity = 900.0f;

static float collision_damping = 0.9f;

static float particle_scale = 50.0f;
static float particle_spacing = 5.0f;
static int particle_count = 100;

static std::vector<glm::vec3> positions_scales{};
static std::vector<glm::vec2> velocities{};

static void generate_particles()
{
    positions_scales.resize(particle_count);
    velocities.resize(particle_count);

    uint32_t particle_per_row = (int)sqrt(particle_count);
    uint32_t particle_per_col = (particle_count - 1) / particle_per_row + 1;

    float spacing = particle_scale + particle_spacing;

    for (uint32_t i = 0; i < particle_count; ++i)
    {
        positions_scales[i].x = (i % particle_per_row) * spacing + particle_scale;
        positions_scales[i].y = (i / particle_per_row) * spacing + particle_scale;
        positions_scales[i].z = particle_scale;
    }
}

static void update_particles(const ving::Engine &engine)
{
    /*assert(positions_scales.size() == velocities.size() == particle_count);*/

    for (size_t i = 0; i < particle_count; ++i)
    {
        velocities[i] += glm::vec2{0.0f, 1.0f} * gravity * engine.delta_time();
        positions_scales[i].x += velocities[i].x * engine.delta_time();
        positions_scales[i].y += velocities[i].y * engine.delta_time();

        glm::vec2 bounds_max{(float)ving::Engine::initial_window_width - positions_scales[i].z / 2.0f,
                             (float)ving::Engine::initial_window_height - positions_scales[i].z / 2.0f};

        glm::vec2 bounds_min{positions_scales[i].z, positions_scales[i].z};

        if (positions_scales[i].x > bounds_max.x)
        {
            positions_scales[i].x = bounds_max.x;
            velocities[i].x *= -1 * collision_damping;
        }
        if (positions_scales[i].y > bounds_max.y)
        {
            positions_scales[i].y = bounds_max.y;
            velocities[i].y *= -1 * collision_damping;
        }
    }
}

int main()
{
    try
    {
        ving::Engine engine{};
        ving::PrimitivesRenderer renderer{engine.core()};

        generate_particles();

        bool simulate = false;

        while (engine.running())
        {
            if (engine.key_pressed(SDLK_R))
                simulate = true;
            if (engine.key_pressed(SDLK_G))
                generate_particles();

            if (simulate)
                update_particles(engine);

            ving::FrameInfo frame = engine.begin_frame();

            renderer.render(ving::PrimitiveType::Circle, positions_scales, frame);
            engine.imgui_renderer().render(frame, {[&engine]() {
                                               ImGui::Text("%f", engine.delta_time() * 1000.0f);
                                               ImGui::DragFloat("Particle Spacing", &particle_spacing);
                                               ImGui::DragFloat("Particle Scale", &particle_scale);
                                               ImGui::DragInt("Particle Count", &particle_count, 1, 1, 1000);
                                               /*ImGui::Text("%f %f", velocities[0].x, velocities[0].y);*/
                                               /*ImGui::Text("%f %f", positions_scales[0].x, positions_scales[0].y);*/
                                           }});

            engine.end_frame(frame);
        }
    }

    catch (const std::exception &e)
    {
        std::cout << e.what() << std::endl;
    }
    catch (...)
    {
        std::cout << "Unknown excpetion" << std::endl;
    }
}
