#pragma once

#include "cells_entry.hpp"
#include "glm/ext/scalar_constants.hpp"
#include "glm/ext/vector_float2.hpp"
#include "glm/ext/vector_float4.hpp"
#include "glm/geometric.hpp"
#include "glm/gtc/random.hpp"
#include "particle.hpp"

#include <cmath>
#include <cstdint>
#include <span>
#include <vector>

static float is_zero(float value)
{
    return value < std::numeric_limits<float>::epsilon();
}

static float smoothing_kernel_poly6(glm::vec2 position, float radius)
{
    float dst_squared = position.x * position.x + position.y * position.y;

    if (radius * radius > dst_squared)
    {
        float value = radius * radius - dst_squared;
        float poly6_scaling_factor = 4.0 / (glm::pi<float>() * pow(radius, 8));
        // float volume = pi * pow(radius, 8) / 4;

        return value * value * value * poly6_scaling_factor;
    }
    return 0.0;
}
static float smoothing_kernel_poly6_derivative(glm::vec2 position, float radius)
{
    float dst_squared = position.x * position.x + position.y * position.y;

    if (radius * radius > dst_squared)
    {
        float f = radius * radius - dst_squared;
        float scale = -24.0 / (glm::pi<float>() * pow(radius, 8));
        return scale * sqrt(dst_squared) * f * f;
    }
    return 0;
}

inline float smoothing_kernel_spiky(glm::vec2 position, float radius)
{
    float dst = glm::length(position);
    /**/
    /*if (dst_squared < radius * radius)*/
    /*{*/
    /*    float volume = (glm::pi<float>() * pow(radius, 4)) / 6.0f;*/
    /**/
    /*    return (radius - dst) * (radius - dst) / volume;*/
    /*}*/

    float radmindst = radius - dst;
    return dst * dst < radius * radius
               ? radmindst * radmindst / (glm::pi<float>() * radius * radius * radius * radius / 6.0f)
               : 0;
}
static float smoothing_kernel_spiky_derivative(glm::vec2 position, float radius)
{
    float dst_squared = position.x * position.x + position.y * position.y;
    float dst = sqrt(dst_squared);

    if (dst_squared < radius * radius)
    {
        float scale = 12.0f / (pow(radius, 4) * glm::pi<float>());
        return (dst - radius) * scale;
    }
    return 0;
}

inline float convert_density_to_pressure(float density, float target_density, float pressure_multiplier)
{
    return (density - target_density) * pressure_multiplier;
}

static float calculate_density(const std::vector<Particle> &particles, const std::vector<uint32_t> &indices,
                               uint32_t particle_index, float smoothing_radius)
{
    float density = 0.0f;

    const glm::vec2 &point = particles[particle_index].position;

    for (auto &&i : indices)
    {
        if (i == particle_index)
            continue;

        density += smoothing_kernel_spiky(particles[i].position - point, smoothing_radius);
    }

    return density;
}
static float calculate_density(std::span<Particle> particles, std::span<CellsEntry> entries, uint32_t particle_index,
                               float smoothing_radius)
{
    assert(particles.size() > 0);
    assert(entries.size() > 0);

    float density = 0.0f;
    glm::vec2 point = particles[particle_index].position;

    for (auto &&entry : entries)
    {
        for (uint32_t i = entry.start; i < entry.start + entry.count; ++i)
        {
            if (particle_index == i)
            {
                continue;
            }
            density += smoothing_kernel_spiky(particles[i].position - point, smoothing_radius);
        }
    }

    return density;
}

static float calculate_shared_pressure(float density_a, float density_b, float target_density,
                                       float pressure_multiplier)
{
    constexpr float damping = 1.0f;

    float pressure_a = convert_density_to_pressure(density_a, target_density, pressure_multiplier);
    float pressure_b = convert_density_to_pressure(density_b, target_density, pressure_multiplier);
    return (damping * pressure_a + damping * pressure_b) / 2.0f;
}
static glm::vec2 calculate_pressure_force(const std::vector<Particle> &particles, const std::vector<uint32_t> &indices,
                                          int particle_index, float smoothing_radius, float target_density,
                                          float pressure_multiplier)
{
    glm::vec2 pressure_force{0.0f};

    glm::vec2 point = particles[particle_index].position;

    for (auto &&i : indices)
    {
        if (i == particle_index)
            continue;

        glm::vec2 dir = glm::normalize(particles[i].position - point);

        float slope = smoothing_kernel_spiky_derivative(particles[i].position - point, smoothing_radius);

        float shared_pressure = calculate_shared_pressure(particles[i].density, particles[particle_index].density,
                                                          target_density, pressure_multiplier);
        /*pressure_force +=*/
        /*    shared_pressure * dir * slope / (particles[i].density + std::numeric_limits<float>::epsilon());*/
        pressure_force +=
            (particles[i].density == 0 ? glm::vec2{0.0f} : shared_pressure * dir * slope / particles[i].density);
    }

    assert(!std::isnan(pressure_force.x));
    assert(!std::isnan(pressure_force.y));

    return pressure_force;
}

static glm::vec2 calculate_pressure_force(std::span<Particle> particles, std::span<CellsEntry> entries,
                                          int particle_index, float smoothing_radius, float target_density,
                                          float pressure_multiplier)
{
    glm::vec2 pressure_force{0.0f};
    glm::vec2 point = particles[particle_index].position;

    for (auto &&entry : entries)
    {
        for (uint32_t i = entry.start; i < entry.start + entry.count; ++i)
        {
            if (particle_index == i)
            {
                continue;
            }
            glm::vec2 dir = glm::normalize(particles[i].position - point);

            float slope = smoothing_kernel_spiky_derivative(particles[i].position - point, smoothing_radius);

            float shared_pressure = calculate_shared_pressure(particles[i].density, particles[particle_index].density,
                                                              target_density, pressure_multiplier);
            /*pressure_force +=*/
            /*    shared_pressure * dir * slope / (particles[i].density + std::numeric_limits<float>::epsilon());*/
            pressure_force += (is_zero(particles[i].density) ? glm::vec2{0.0f}
                                                             : shared_pressure * dir * slope / particles[i].density);
        }
    }

    assert(!std::isnan(pressure_force.x));
    assert(!std::isnan(pressure_force.y));

    return pressure_force;
}
