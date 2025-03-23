#include "engine.hpp"
#include "primitives_renderer.hpp"
#include "spatial_particle_grid.hpp"

#include <tracy/Tracy.hpp>

static constexpr uint32_t particle_count = 700;

int main()
{
    ving::Engine engine{};
    ving::PrimitivesRenderer renderer{engine.core()};

    SpatialParticleGrid grid{ving::Engine::initial_window_width, ving::Engine::initial_window_height, 100.0f};
    grid.generate_particles_random(particle_count);

    std::vector<ving::PrimitiveParameters> parameters{};
    parameters.resize(particle_count);

    for (auto &&p : grid.particles())
    {
        /*std::cout << p.position.x << ' ' << p.position.y << '\n';*/
    }

    while (engine.running())
    {
        ving::FrameInfo frame = engine.begin_frame();
        grid.update();

        for (uint32_t i = 0; i < particle_count; ++i)
        {
            parameters[i].position = grid.particles()[i].position;
            parameters[i].color = glm::vec3{1.0f};
            parameters[i].scale = 7.0f;
        }
        Particle particle{engine.cursor_pos(), {}, 0.0f, 0};
        std::vector<CellsEntry> entries = grid.get_neighbour_particle_entries(particle);

        for (auto &&e : entries)
        {
            for (uint32_t i = e.start; i < e.start + e.count; ++i)
            {
                parameters[i].color = glm::vec3{1.0f, 0.0f, 0.0f};
            }
        }

        engine.begin_rendering(frame, true);

        renderer.render(frame, ving::PrimitiveType::Circle, parameters);

        engine.imgui_renderer().render(
            frame, {[&engine]() { ImGui::Text("Engine delta time: %f", engine.delta_time() * 1000.0f); }});

        engine.end_rendering(frame);
        engine.end_frame(frame);
        FrameMark;
    }
    std::cout << "======== End of main so ignore if some Vulkan objects are destroyed before device done it's work. "
                 "========\n";
}
