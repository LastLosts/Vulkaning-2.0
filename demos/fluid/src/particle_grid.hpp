#pragma once

#include "particle.hpp"
#include "timings.hpp"
#include <glm/ext/vector_uint2.hpp>
#include <span>
#include <vector>

struct CellsEntry
{
    uint32_t start;
    uint32_t count;
};

// TODO: Refactor this to 2 classes one for the grid, one for simulating particles
class ParticleGrid
{
  public:
    ParticleGrid(float grid_size);

    void generate_particles_random(size_t particle_count);
    void generate_particles_cube(size_t particle_count);
    void generate_grid();
    std::vector<TimeResult> update_particles(float smoothing_radius, float target_density, float pressure_multiplier,
                                             float delta_time);

    [[nodiscard]] std::span<const Particle> particles() { return m_particles; }
    [[nodiscard]] uint32_t cells_size() { return m_cell_size; }
    std::vector<uint32_t> get_neighbour_particle_indices(const Particle &particle);

    static constexpr float particle_scale = 0.01f;

  private:
    static constexpr float offset = 0.001f;
    static constexpr float gravity = 9.0f;
    static constexpr float collision_damping = 0.7f;
    static constexpr uint32_t max_digits = 3;

    std::vector<Particle> m_particles;
    std::vector<CellsEntry> m_cells_entries;

    float m_cell_size;
    float m_grid_cell_size;

    std::span<Particle> get_cell_particles(glm::uvec2 cell_coords);
    std::vector<std::span<Particle>> get_neighbour_particles(glm::uvec2 cell_coords);

    glm::uvec2 particle_cell_coords(const Particle &particle);
    uint32_t cell_id(glm::uvec2 cell_coords);
    uint32_t get_particle_cell_id(const Particle &particle);

    void radix_sort_particles(std::vector<Particle> &arr);
};
