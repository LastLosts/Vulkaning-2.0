#include "particle_grid.hpp"
#include "fluid_simulation.hpp"

#include "glm/ext/vector_int2.hpp"
#include "radix_sort.hpp"

#include <iostream>
#include <random>

ParticleGrid::ParticleGrid(float grid_size)
{
    m_cell_size = ceil(1.0f / grid_size);
    m_grid_cell_size = grid_size;
    m_cells_entries.resize(m_cell_size * m_cell_size);
}

void ParticleGrid::generate_particles_random(size_t particle_count)
{
    m_particles.resize(particle_count);

    std::random_device dev{};
    std::default_random_engine el{dev()};

    std::uniform_real_distribution<float> uniform_dist{0.0f + offset + particle_scale / 2.0f,
                                                       1.0f - offset - particle_scale / 2.0f};

    for (uint32_t i = 0; i < particle_count; ++i)
    {
        m_particles[i].position = {uniform_dist(el), uniform_dist(el)};
        m_particles[i].velocity = {};
    }
}
void ParticleGrid::generate_particles_cube(size_t particle_count)
{
    m_particles.resize(particle_count);

    uint32_t particle_per_row = (int)sqrt(particle_count);
    uint32_t particle_per_col = (particle_count - 1) / particle_per_row + 1;

    float spacing = particle_scale + 0.001f;

    for (uint32_t i = 0; i < particle_count; ++i)
    {
        m_particles[i].position = {(i % particle_per_row) * spacing + particle_scale,
                                   floor(static_cast<float>(i) / particle_per_row) * spacing + particle_scale};
    }
}

void ParticleGrid::update(float smoothing_radius, float target_density, float pressure_multiplier, float delta_time)
{
    sort_particles();
    update_particles(smoothing_radius, target_density, pressure_multiplier, delta_time);
}

void ParticleGrid::update_particles(float smoothing_radius, float target_density, float pressure_multiplier,
                                    float delta_time)
{
    for (size_t i = 0; i < m_particles.size(); ++i)
    {
        std::vector<uint32_t> neighbour_indices = get_neighbour_particle_indices(particle_cell_coords(m_particles[i]));
        m_particles[i].density = calculate_density(m_particles, neighbour_indices, i, smoothing_radius);
    }

    for (size_t i = 0; i < m_particles.size(); ++i)
    {
        std::vector<uint32_t> neighbour_indices = get_neighbour_particle_indices(particle_cell_coords(m_particles[i]));

        m_particles[i].velocity = calculate_pressure_force(m_particles, neighbour_indices, i, smoothing_radius,
                                                           target_density, pressure_multiplier);

        constexpr bool gravity_on = true;

        if constexpr (gravity_on)
            m_particles[i].velocity -= glm::vec2{0.0f, 1.0f} * gravity * delta_time;

        m_particles[i].position = m_particles[i].position + delta_time * m_particles[i].velocity;

        constexpr bool collision = true;

        if constexpr (collision)
        {
            float bound_max{1.0f - offset - particle_scale / 2.0f};

            float bound_min{particle_scale + offset};

            if (m_particles[i].position.x > bound_max)
            {
                m_particles[i].position.x = bound_max;
                m_particles[i].velocity.x *= -1 * collision_damping;
            }
            if (m_particles[i].position.y > bound_max)
            {
                m_particles[i].position.y = bound_max;
                m_particles[i].velocity.y *= -1 * collision_damping;
            }
            if (m_particles[i].position.x < bound_min)
            {
                m_particles[i].position.x = bound_min;
                m_particles[i].velocity.x *= -1 * collision_damping;
            }
            if (m_particles[i].position.y < bound_min)
            {
                m_particles[i].position.y = bound_min;
                m_particles[i].velocity.y *= -1 * collision_damping;
            }
        }
    }
}
std::vector<uint32_t> ParticleGrid::get_neighbour_particle_indices(glm::uvec2 cell_coords)
{
    std::vector<uint32_t> indices{};

    std::vector<CellsEntry> neighbour_entries{};
    neighbour_entries.reserve(9);

    /*std::cout << "Indices entries: \n";*/

    for (int x = -1; x <= 1; ++x)
    {
        if ((int)cell_coords.x + x < 0 || (int)cell_coords.x + x >= m_cell_size)
            continue;

        for (int y = -1; y <= 1; ++y)
        {
            if ((int)cell_coords.y + y < 0 || (int)cell_coords.y + y >= m_cell_size)
                continue;

            assert(cell_id({cell_coords.x + x, cell_coords.y + y}) < m_cell_size * m_cell_size);

            neighbour_entries.push_back(m_cells_entries[cell_id({cell_coords.x + x, cell_coords.y + y})]);

            /*std::cout << m_cells_entries[cell_id({cell_coords.x + x, cell_coords.y + y})].start << ' '*/
            /*          << m_cells_entries[cell_id({cell_coords.x + x, cell_coords.y + y})].count << '\n';*/
        }
    }

    uint32_t reservation{};

    for (auto &&e : neighbour_entries)
    {
        reservation += e.count;
    }

    for (auto &&e : neighbour_entries)
    {
        for (uint32_t i = e.start; i < e.start + e.count; ++i)
        {
            indices.push_back(i);
        }
    }

    return indices;
}

void ParticleGrid::sort_particles()
{
    for (uint32_t i = 0; i < m_particles.size(); ++i)
    {
        m_particles[i].cell_id = get_particle_cell_id(m_particles[i]);
    }

    radix_sort_particles(m_particles);

    uint32_t current_cell_id = 0, previous_cell_id = m_particles[0].cell_id, previous_index = 0;

    for (uint32_t i = 0; i < m_particles.size(); ++i)
    {
        current_cell_id = m_particles[i].cell_id;
        if (current_cell_id != previous_cell_id)
        {
            m_cells_entries[previous_cell_id] = {previous_index, i - previous_index};
            previous_cell_id = m_particles[i].cell_id;
            previous_index = i;
        }
    }

    m_cells_entries[current_cell_id] = {previous_index, static_cast<uint32_t>(m_particles.size() - previous_index)};

    /*for (auto &&e : m_cells_entries)*/
    /*{*/
    /*    std::cout << e.start << ' ' << e.count << '\n';*/
    /*}*/
    /*std::cout << '\n';*/
    /**/
    /*uint32_t ctr = 0;*/
    /**/
    /*for (auto &&p : m_particles)*/
    /*{*/
    /*    if (ctr == 15)*/
    /*    {*/
    /*        ctr = 0;*/
    /*        std::cout << '\n';*/
    /*    }*/
    /*    std::cout << p.cell_id << ' ';*/
    /*    ++ctr;*/
    /*}*/
    /*std::cout << '\n';*/
}
glm::uvec2 ParticleGrid::particle_cell_coords(const Particle &particle)
{
    return {trunc(particle.position.x / m_grid_cell_size), trunc(particle.position.y / m_grid_cell_size)};
}

uint32_t ParticleGrid::cell_id(glm::uvec2 cell_coords)
{
    return cell_coords.y * m_cell_size + cell_coords.x;
}
uint32_t ParticleGrid::get_particle_cell_id(const Particle &particle)
{
    glm::uvec2 cell_coords = particle_cell_coords(particle);

    return cell_id(cell_coords);
}

std::span<Particle> ParticleGrid::get_cell_particles(glm::uvec2 cell_coords)
{
    return {m_particles.begin() + m_cells_entries[cell_id(cell_coords)].start,
            m_cells_entries[cell_id(cell_coords)].count};
}

std::vector<std::span<Particle>> ParticleGrid::get_neighbour_particles(glm::uvec2 cell_coords)
{
    std::vector<std::span<Particle>> result{};

    for (int x = -1; x <= 1; ++x)
    {
        if ((int)cell_coords.x + x < 0 || (int)cell_coords.x + x >= m_cell_size)
            continue;

        for (int y = -1; y <= 1; ++y)
        {
            if ((int)cell_coords.y + y < 0 || (int)cell_coords.y + y >= m_cell_size)
                continue;

            /*std::cout << cell_coords.x + x << ' ' << cell_coords.y + y << ' '*/
            /*          << cell_id({cell_coords.x + x, cell_coords.y + y}) << '\n';*/

            assert(cell_id({cell_coords.x + x, cell_coords.y + y}) < m_cell_size * m_cell_size);

            result.push_back(get_cell_particles({cell_coords.x + x, cell_coords.y + y}));
        }
    }
    return result;
}

void ParticleGrid::radix_sort_particles(std::vector<Particle> &arr)
{
    for (uint32_t j = 1; j <= max_digits; ++j)
    {
        uint32_t digits[10]{};

        // Count digits
        for (size_t i = 0; i < arr.size(); ++i)
        {
            digits[get_digit(arr[i].cell_id, j)]++;
        }
        // Accumulate
        for (uint32_t i = 1; i < 10; ++i)
        {
            digits[i] = digits[i] + digits[i - 1];
        }
        // Shift
        for (uint32_t i = 9; i > 0; --i)
        {
            digits[i] = digits[i - 1];
        }
        digits[0] = 0;

        std::vector<Particle> copy{arr};
        for (size_t i = 0; i < arr.size(); ++i)
        {
            copy[digits[get_digit(arr[i].cell_id, j)]] = arr[i];
            digits[get_digit(arr[i].cell_id, j)]++;
        }
        arr = std::move(copy);
    }
}
