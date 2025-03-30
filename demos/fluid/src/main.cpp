#include "engine.hpp"
#include "fluid_simulation.hpp"
#include "primitives_renderer.hpp"
#include "spatial_particle_grid.hpp"

#include <tracy/Tracy.hpp>

static constexpr uint32_t particle_count = 700;
static float smoothing_radius = 50.0f;
static float target_density = 0.1f;
static float pressure_multiplier = 0.70f;
// clang-format off
#define ONE_TIME_LOG(x)                                                                                                \
    static bool a = true;                                                                                              \
    if (a) {                                                                                                           \
        std::cout << (x) << '\n';                                                                                      \
        a = false;                                                                                                     \
    }
// clang-format on

static bool gravity_on = false;

static float max_density = std::numeric_limits<float>::min();
static float max_velocity_length = std::numeric_limits<float>::min();

static std::vector<CellsEntry> marked_particle_neighbours;
static uint32_t marked_particle_index = 0;
static bool debug_stop = false;

static void update_particles(SpatialParticleGrid &grid, float delta_time)
{
    max_density = std::numeric_limits<float>::min();
    max_velocity_length = std::numeric_limits<float>::min();

    bool update_cache = true;

    std::vector<CellsEntry> cached_neighbours;
    static const float gravity = 9.0f;
    glm::vec2 gravity_direction{0.0f, 1.0f};
    glm::vec2 gravity_force{gravity * delta_time * gravity_direction};

    for (uint32_t i = 0; i < grid.particles().size(); ++i)
    {
        update_cache = (i == 0 || grid.particles()[i].cell_id != grid.particles()[i - 1].cell_id);
        if (update_cache)
        {
            cached_neighbours = grid.get_neighbour_particle_entries(grid.particles()[i]);
            update_cache = false;
        }

        grid.particles()[i].density = calculate_density(grid.particles(), cached_neighbours, i, smoothing_radius);
        max_density = std::max(max_density, grid.particles()[i].density);
    }
    update_cache = true;

    for (uint32_t i = 0; i < grid.particles().size(); ++i)
    {
        update_cache = (i == 0 || grid.particles()[i].cell_id != grid.particles()[i - 1].cell_id);
        if (update_cache)
        {
            cached_neighbours = grid.get_neighbour_particle_entries(grid.particles()[i]);
            update_cache = false;
        }

        grid.particles()[i].position = grid.particles()[i].position + delta_time * grid.particles()[i].velocity;
        grid.particles()[i].velocity = grid.particles()[i].velocity -
                                       calculate_pressure_force(grid.particles(), cached_neighbours, i,
                                                                smoothing_radius, target_density, pressure_multiplier);

        float particle_velocity_length = glm::length(grid.particles()[i].velocity);
        max_velocity_length = std::max(max_velocity_length, particle_velocity_length);

        if (gravity_on)
        {
            grid.particles()[i].velocity = grid.particles()[i].velocity + gravity_force;
        }

        constexpr bool collision = true;
        constexpr bool damping = true;

        if constexpr (!damping && collision)
        {
            grid.particles()[i].position.x = glm::clamp(grid.particles()[i].position.x, grid.particle_radius(),
                                                        ving::Engine::initial_window_width - grid.particle_radius());
            grid.particles()[i].position.y = glm::clamp(grid.particles()[i].position.y, grid.particle_radius(),
                                                        ving::Engine::initial_window_height - grid.particle_radius());
        }
        if constexpr (damping && collision)
        {
            constexpr float collision_damping = 0.7f;

            glm::vec2 bound_max{ving::Engine::initial_window_width - grid.particle_radius(),
                                ving::Engine::initial_window_height - grid.particle_radius()};

            glm::vec2 bound_min{grid.particle_radius(), grid.particle_radius()};

            if (grid.particles()[i].position.x > bound_max.x)
            {
                grid.particles()[i].position.x = bound_max.x;
                grid.particles()[i].velocity.x = -grid.particles()[i].velocity.x * collision_damping;
            }
            if (grid.particles()[i].position.y > bound_max.y)
            {
                grid.particles()[i].position.y = bound_max.y;
                grid.particles()[i].velocity.y = -grid.particles()[i].velocity.y * collision_damping;
            }
            if (grid.particles()[i].position.x < bound_min.x)
            {
                grid.particles()[i].position.x = bound_min.x;
                grid.particles()[i].velocity.x = -grid.particles()[i].velocity.x * collision_damping;
            }
            if (grid.particles()[i].position.y < bound_min.y)
            {
                grid.particles()[i].position.y = bound_min.y;
                grid.particles()[i].velocity.y = -grid.particles()[i].velocity.y * collision_damping;
            }
        }
    }
}

constexpr uint32_t number_of_iterations = 5;
static uint32_t highlighted_particle = 0;

int main()
{
    ving::Engine engine{};
    ving::PrimitivesRenderer renderer{engine.core()};

    SpatialParticleGrid grid{ving::Engine::initial_window_width, ving::Engine::initial_window_height, smoothing_radius};
    grid.generate_particles_random(particle_count);

    std::vector<ving::PrimitiveParameters> parameters{};
    parameters.resize(particle_count);

    while (engine.running())
    {
        ving::FrameInfo frame = engine.begin_frame();

        if (glfwGetKey(engine.window().window(), GLFW_KEY_P) == GLFW_PRESS)
        {
            float min_distance = std::numeric_limits<float>::max();

            for (uint32_t i = 0; i < particle_count; ++i)
            {
                float distance = glm::length(grid.particles()[i].position - engine.cursor_pos());

                if (distance < min_distance)
                {
                    min_distance = distance;
                    marked_particle_index = i;
                }
            }

            marked_particle_neighbours = grid.get_neighbour_particle_entries(grid.particles()[marked_particle_index]);

            debug_stop = true;
        }

        if (!debug_stop)
        {
            grid.update();
            for (uint32_t i = 0; i < number_of_iterations; ++i)
            {
                update_particles(grid, engine.delta_time() / (float)number_of_iterations);
            }
        }

        for (uint32_t i = 0; i < particle_count; ++i)
        {
            parameters[i].position = grid.particles()[i].position;
            /*parameters[i].color = glm::mix(glm::vec3{0.0f, 0.0f, 1.0f}, glm::vec3{1.0f, 0.0f, 0.0f},*/
            /*                               glm::length(grid.particles()[i].velocity) / max_velocity_length);*/
            parameters[i].color = glm::mix(glm::vec3{0.0f, 0.0f, 1.0f}, glm::vec3{1.0f, 0.0f, 0.0f},
                                           grid.particles()[i].density / max_density);
            if (debug_stop)
            {
                parameters[i].color = {};
            }
            parameters[i].scale = 7.0f;
        }

        if (debug_stop)
        {
            for (auto &&entry : marked_particle_neighbours)
            {
                for (uint32_t i = entry.start; i < entry.start + entry.count; ++i)
                {
                    parameters[i].color = glm::vec3{1.0f, 0.0f, 0.0f};
                }
            }
            parameters[marked_particle_index].color = glm::vec3{0.0f, 1.0f, 0.0f};

            // std::cout << "Density: " << grid.particles()[marked_particle_index].density << '\n';
            // std::cout << "Velocity: " << grid.particles()[marked_particle_index].velocity.x << ' '
            //           << grid.particles()[marked_particle_index].velocity.y << '\n';
            // std::cout << "Position" << grid.particles()[marked_particle_index].position.x << ' '
            //           << grid.particles()[marked_particle_index].position.y << '\n';
        }

        // Cursor
        //
        /*Particle particle{.position = engine.cursor_pos(), .velocity = {}, .density = 0.0f, .cell_id = 0};*/
        /*std::vector<CellsEntry> entries = grid.get_neighbour_particle_entries(particle);*/
        /*for (auto &&entry : entries)*/
        /*{*/
        /*    for (uint32_t i = entry.start; i < entry.start + entry.count; ++i)*/
        /*    {*/
        /*        parameters[i].color = glm::vec3{1.0f, 0.0f, 0.0f};*/
        /*    }*/
        /*}*/

        engine.begin_rendering(frame, true);
        renderer.render(frame, ving::PrimitiveType::Circle, parameters);
        engine.imgui_renderer().render(
            frame, {[&engine, &grid]() {
                ImGui::Text("Engine delta time: %f", engine.delta_time() * 1000.0f);
                /*ImGui::InputFloat("Smoothing radius", &smoothing_radius);*/
                ImGui::InputFloat("Target density", &target_density);
                ImGui::InputFloat("Pressure multiplier", &pressure_multiplier);
                ImGui::Text("Marked particle particle: %d", marked_particle_index);
                ImGui::Text("Max density: %f", max_density);
                if (debug_stop)
                {
                    ImGui::Text("Marked Particle");
                    ImGui::Text("Position %f %f", grid.particles()[marked_particle_index].position.x,
                                grid.particles()[marked_particle_index].position.y);
                    ImGui::Text("Velocity %f %f", grid.particles()[marked_particle_index].velocity.x,
                                grid.particles()[marked_particle_index].velocity.y);
                    ImGui::Text("Density %f", grid.particles()[marked_particle_index].density);
                }
            }});
        engine.end_rendering(frame);

        engine.end_frame(frame);
        FrameMark;
    }
    std::cout << "======== End of main so ignore if some Vulkan objects are destroyed before device done it's work. "
                 "========\n";
}
