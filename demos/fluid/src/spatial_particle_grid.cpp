#include "spatial_particle_grid.hpp"
#include "glm/common.hpp"
#include <cmath>
#include <random>

static uint32_t count_digits(uint32_t number)
{
    return int(log10(number) + 1);
}
static uint32_t get_digit(uint32_t value, uint32_t digit_place)
{
    uint32_t powten = std::pow(10, digit_place);
    return (value % powten) / (powten / 10);
}

SpatialParticleGrid::SpatialParticleGrid(float width, float height, float radius, float particle_radius)
    : m_width{width}, m_height{height}, m_particle_radius{particle_radius}, m_cell_size_pixels{radius}
{
    assert(width > 0);
    assert(height > 0);
    assert(width > height);

    m_cells_width = std::ceil(width / radius);
    m_cells_height = std::ceil(height / radius);

    m_max_digits = count_digits(m_cells_width * m_cells_height);
    m_cells_entries.resize(m_cells_width * m_cells_height);
}

void SpatialParticleGrid::update()
{
    for (uint32_t i = 0; i < m_particles.size(); ++i)
    {
        m_particles[i].cell_id = get_particle_cell_id(m_particles[i]);
    }

    radix_sort_particles();

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
    uint32_t accum = 0;
    for (auto &&e : m_cells_entries)
    {
        accum += e.count;
    }
    assert(accum == m_particles.size());
#endif
}

void SpatialParticleGrid::generate_particles_random(size_t particle_count)
{
    assert(particle_count > 0);

    m_particles.resize(particle_count);

    std::random_device dev{};
    std::default_random_engine engine{dev()};

    std::uniform_real_distribution<float> width_dist{m_particle_radius / 2.0f, m_width - m_particle_radius / 2.0f};
    std::uniform_real_distribution<float> height_dist{m_particle_radius / 2.0f, m_height - m_particle_radius / 2.0f};

    for (uint32_t i = 0; i < particle_count; ++i)
    {
        m_particles[i].position = {width_dist(engine), height_dist(engine)};
        m_particles[i].velocity = {};
    }
}
std::vector<CellsEntry> SpatialParticleGrid::get_neighbour_particle_entries(const Particle &particle)
{
    std::vector<CellsEntry> entries;
    entries.reserve(9);

    glm::uvec2 cell_coords = particle_cell_coords(particle);

    for (int x = -1; x <= 1; ++x)
    {
        if ((int)cell_coords.x + x < 0 || (int)cell_coords.x + x >= m_cells_width)
            continue;

        for (int y = -1; y <= 1; ++y)
        {
            if ((int)cell_coords.y + y < 0 || (int)cell_coords.y + y >= m_cells_height)
                continue;

            assert(cell_id({cell_coords.x + x, cell_coords.y + y}) < m_cells_width * m_cells_height);

            entries.push_back(m_cells_entries[cell_id({cell_coords.x + x, cell_coords.y + y})]);
        }
    }

    return entries;
}
uint32_t SpatialParticleGrid::cell_id(glm::uvec2 cell_coords)
{
    return cell_coords.y * m_cells_width + cell_coords.x;
}
glm::uvec2 SpatialParticleGrid::particle_cell_coords(const Particle &particle)
{
    return {trunc(particle.position.x / m_cell_size_pixels), trunc(particle.position.y / m_cell_size_pixels)};
}
uint32_t SpatialParticleGrid::get_particle_cell_id(const Particle &particle)
{
    return cell_id(particle_cell_coords(particle));
}

static bool is_sorted(const std::vector<Particle> &particles)
{
    for (uint32_t i = 1; i < particles.size(); ++i)
    {
        if (particles[i].cell_id < particles[i - 1].cell_id)
        {
            return false;
        }
    }
    return true;
}

void SpatialParticleGrid::radix_sort_particles()
{
    for (uint32_t j = 1; j <= m_max_digits; ++j)
    {
        std::array<uint32_t, 10> digits{};

        for (size_t i = 0; i < m_particles.size(); ++i)
        {
            digits[get_digit(m_particles[i].cell_id, j)]++;
        }

        for (uint32_t i = 1; i < 10; ++i)
        {
            digits[i] = digits[i] + digits[i - 1];
        }

        for (uint32_t i = 9; i > 0; --i)
        {
            digits[i] = digits[i - 1];
        }
        digits[0] = 0;

        std::vector<Particle> copy{m_particles};
        for (size_t i = 0; i < m_particles.size(); ++i)
        {
            copy[digits[get_digit(m_particles[i].cell_id, j)]] = m_particles[i];
            digits[get_digit(m_particles[i].cell_id, j)]++;
        }
        m_particles = std::move(copy);
    }

    assert(is_sorted(m_particles));
}
