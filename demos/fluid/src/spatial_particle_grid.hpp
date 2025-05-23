#pragma once

#include "cells_entry.hpp"
#include "particle.hpp"
#include <span>
#include <vector>

class SpatialParticleGrid final
{
  public:
    SpatialParticleGrid(float width, float height, float radius, float particle_radius = 7.0f);

    void update();

    void generate_particles_random(size_t particle_count);
    std::array<CellsEntry, 9> get_neighbour_particle_entries(const Particle &particle);

    [[nodiscard]] std::span<Particle> particles() { return m_particles; }
    [[nodiscard]] float particle_radius() { return m_particle_radius; }

  private:
    std::vector<Particle> m_particles;
    std::vector<CellsEntry> m_cells_entries;

    uint32_t m_cells_width;
    uint32_t m_cells_height;
    uint32_t m_max_digits;

    float m_cell_size_pixels;
    float m_width;
    float m_height;

    float m_particle_radius;

  private:
    uint32_t cell_id(uint32_t x, uint32_t y);
    // TODO May use uvec2 again
    std::pair<uint32_t, uint32_t> particle_cell_coords(const Particle &particle);
    uint32_t get_particle_cell_id(const Particle &particle);

    void radix_sort_particles();
};
