#pragma once

#include "glm/ext/scalar_constants.hpp"
#include "glm/ext/vector_float2.hpp"
#include "glm/ext/vector_float4.hpp"
#include "glm/geometric.hpp"

#include <cmath>
#include <cstdint>
#include <vector>

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

static float smoothing_kernel_spiky(glm::vec2 position, float radius)
{
    float dst_squared = position.x * position.x + position.y * position.y;
    float dst = sqrt(dst_squared);

    if (radius * radius > dst_squared)
    {
        float volume = (glm::pi<float>() * pow(radius, 4)) / 6.0f;

        return (radius - dst) * (radius - dst) / volume;
    }
    return 0;
}
static float smoothing_kernel_spiky_derivative(glm::vec2 position, float radius)
{
    float dst_squared = position.x * position.x + position.y * position.y;
    float dst = sqrt(dst_squared);

    if (radius * radius > dst_squared)
    {
        float scale = 12.0f / (pow(radius, 4) * glm::pi<float>());
        return (dst - radius) * scale;
    }
    return 0;
}

static float convert_density_to_pressure(float density, float target_density, float pressure_multiplier)
{
    float density_delta = density - target_density;
    float pressure = density_delta * pressure_multiplier;
    return pressure;
}

static float calculate_density(const std::vector<glm::vec4> &positions, uint32_t particle_index, float smoothing_radius)
{
    float density = 0.0;

    glm::vec2 point{positions[particle_index].x, positions[particle_index].y};

    for (int i = 0; i < positions.size(); i++)
    {
        if (i == particle_index)
            continue;

        /*density += smoothing_kernel_poly6(glm::vec2{positions[i].x, positions[i].y} - point, smoothing_radius);*/
        density += smoothing_kernel_spiky(glm::vec2{positions[i].x, positions[i].y} - point, smoothing_radius);
    }

    return density;
}

static float calculate_shared_pressure(float density_a, float density_b, float target_density,
                                       float pressure_multiplier)
{
    float pressure_a = convert_density_to_pressure(density_a, target_density, pressure_multiplier);
    float pressure_b = convert_density_to_pressure(density_b, target_density, pressure_multiplier);
    return (pressure_a + pressure_b) / 2.0f;
}
static glm::vec2 calculate_pressure_force(const std::vector<glm::vec4> &positions, int particle_index,
                                          float smoothing_radius, float target_density, float pressure_multiplier)
{
    glm::vec2 pressure_force{0.0f};

    glm::vec2 point{positions[particle_index].x, positions[particle_index].y};
    for (int i = 0; i < positions.size(); ++i)
    {
        if (i == particle_index)
            continue;

        glm::vec2 particle_position{positions[i].x, positions[i].y};
        glm::vec2 dir = glm::normalize(particle_position - point);

        /*if (glm::distance(particle_position, point) == 0)*/
        /*{*/
        /*    dir = glm::circularRand(1.0f);*/
        /*}*/

        /*float slope = smoothing_kernel_poly6_derivative(particle_position - point, smoothing_radius);*/
        float slope = smoothing_kernel_spiky_derivative(particle_position - point, smoothing_radius);
        float density = positions[i].z;

        assert(density != 0);

        float shared_pressure =
            calculate_shared_pressure(density, positions[particle_index].z, target_density, pressure_multiplier);

        pressure_force += shared_pressure * dir * slope / density;
    }

    return pressure_force;
}
