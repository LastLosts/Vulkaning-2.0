#include "particle_grid.hpp"
#include "fluid_simulation.hpp"

#include "glm/ext/vector_int2.hpp"
#include "radix_sort.hpp"

#include <iostream>
#include <random>

#include <chrono>

ParticleGrid::ParticleGrid(float grid_size)
{
    m_cell_size = ceil(1.0f / grid_size);
    std::cout << m_cell_size << '\n';
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
void ParticleGrid::generate_particles_cube(float spacing, size_t particle_count)
{
    spacing += particle_scale;
    m_particles.resize(particle_count);

    uint32_t particle_per_row = (int)sqrt(particle_count);
    uint32_t particle_per_col = (particle_count - 1) / particle_per_row + 1;

    for (uint32_t i = 0; i < particle_count; ++i)
    {
        m_particles[i].position = {(i % particle_per_row) * spacing + particle_scale,
                                   floor(static_cast<float>(i) / particle_per_row) * spacing + particle_scale};
    }
}

// TODO: Predicted positions
std::vector<TimeResult> ParticleGrid::update_particles(float smoothing_radius, float target_density,
                                                       float pressure_multiplier, float delta_time)
{
    // TODO: Might create an arena if performance is too bad
    Timer timer;
    std::vector<TimeResult> results;

    glm::vec2 gravity_dir{0.0f, 1.0f};
    glm::vec2 gravity_force{gravity * delta_time * gravity_dir};

    std::vector<std::vector<uint32_t>> particle_neighbours;
    particle_neighbours.resize(m_particles.size());
    for (uint32_t i = 0; i < m_particles.size(); ++i)
    {
        particle_neighbours[i] = get_neighbour_particle_indices(m_particles[i]);
    }
    results.push_back(timer.get_result("Pregen neighbours"));

    for (size_t i = 0; i < m_particles.size(); ++i)
    {
        m_particles[i].density = calculate_density(m_particles, particle_neighbours[i], i, smoothing_radius);
    }
    results.push_back(timer.get_result("Calculate densities"));

    max_velocity = std::numeric_limits<float>::min();

    for (size_t i = 0; i < m_particles.size(); ++i)
    {
        m_particles[i].position = m_particles[i].position + delta_time * m_particles[i].velocity;

        m_particles[i].velocity =
            m_particles[i].velocity + calculate_pressure_force(m_particles, particle_neighbours[i], i, smoothing_radius,
                                                               target_density, pressure_multiplier) *
                                          delta_time;
        max_velocity = glm::max(max_velocity, glm::length(m_particles[i].velocity));

        if (gravity_on)
        {
            m_particles[i].velocity = m_particles[i].velocity + gravity_force;
        }

        constexpr bool collision = true;
        constexpr bool damping = true;

        if constexpr (!damping && collision)
        {
            m_particles[i].position.x =
                glm::clamp(m_particles[i].position.x, particle_scale + offset, 1.0f - particle_scale - offset);
            m_particles[i].position.y =
                glm::clamp(m_particles[i].position.y, particle_scale + offset, 1.0f - particle_scale - offset);
        }

        if constexpr (damping && collision)
        {
            float bound_max{1.0f - offset - particle_scale / 2.0f};

            float bound_min{particle_scale + offset};

            if (m_particles[i].position.x > bound_max)
            {
                m_particles[i].position.x = bound_max;
                m_particles[i].velocity.x = -m_particles[i].velocity.x * collision_damping;
            }
            if (m_particles[i].position.y > bound_max)
            {
                m_particles[i].position.y = bound_max;
                m_particles[i].velocity.y = -m_particles[i].velocity.y * collision_damping;
            }
            if (m_particles[i].position.x < bound_min)
            {
                m_particles[i].position.x = bound_min;
                m_particles[i].velocity.x = -m_particles[i].velocity.x * collision_damping;
            }
            if (m_particles[i].position.y < bound_min)
            {
                m_particles[i].position.y = bound_min;
                m_particles[i].velocity.y = -m_particles[i].velocity.y * collision_damping;
            }
        }
    }
    results.push_back(timer.get_result("Gravity, Collision, and pressure force"));

    return results;
}

std::vector<uint32_t> ParticleGrid::get_neighbour_particle_indices(glm::vec2 particle_position)
{
    std::vector<uint32_t> indices{};

    std::vector<CellsEntry> neighbour_entries{};
    neighbour_entries.reserve(9);

    /*std::cout << "Indices entries: \n";*/

    glm::uvec2 cell_coords = particle_cell_coords(particle_position);

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
std::vector<uint32_t> ParticleGrid::get_neighbour_particle_indices(const Particle &particle)
{
    std::vector<uint32_t> indices{};

    std::vector<CellsEntry> neighbour_entries{};
    neighbour_entries.reserve(9);

    /*std::cout << "Indices entries: \n";*/

    glm::uvec2 cell_coords = particle_cell_coords(particle);

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

void ParticleGrid::generate_grid()
{
    for (uint32_t i = 0; i < m_particles.size(); ++i)
    {
        m_particles[i].cell_id = get_particle_cell_id(m_particles[i]);
    }

    radix_sort_particles(m_particles);

    uint32_t current_cell_id = 0, previous_cell_id = m_particles[0].cell_id, previous_index = 0;

    std::fill(m_cells_entries.begin(), m_cells_entries.end(), CellsEntry{});

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

#ifndef NDEBUG
    /*std::cout << "Cell entries\n";*/
    uint32_t accum = 0;
    for (auto &&e : m_cells_entries)
    {
        /*std::cout << e.start << ' ' << e.count << '\n';*/
        accum += e.count;
    }
    assert(accum == m_particles.size());

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
#endif
}
glm::uvec2 ParticleGrid::particle_cell_coords(glm::vec2 particle_position)
{
    return {trunc(particle_position.x / m_grid_cell_size), trunc(particle_position.y / m_grid_cell_size)};
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
