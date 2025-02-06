#pragma once
#include "mesh.hpp"

#include <array>
#include <inttypes.h>

namespace ving
{
static Mesh generate_quad(const VulkanCore &core)
{
    std::array<Vertex, 4> vertices{
        Vertex{{-0.5f, 0.5f, 0.0f}, 0.0f, {}, 0.0f},  // Bottom Left
        Vertex{{-0.5f, -0.5f, 0.0f}, 0.0f, {}, 1.0f}, //
        Vertex{{0.5f, -0.5f, 0.0f}, 1.0f, {}, 1.0f},  //
        Vertex{{0.5f, 0.5f, 0.0f}, 1.0f, {}, 0.0f},   // Bottom Right
    };

    std::array<uint32_t, 6> indices{0, 1, 2, 2, 3, 0};

    return Mesh{core, vertices, indices};
};

} // namespace ving
