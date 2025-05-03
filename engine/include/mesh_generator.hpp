#pragma once
#include "glm/ext/vector_float4.hpp"
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
static Mesh generate_cube(const VulkanCore &core)
{
    // clang-format off
    std::array<uint32_t, 36> indices{
        0,4,5,5,1,0,
        2,6,7,7,3,2,
        1,5,6,6,2,1,
        0,3,7,7,4,0,
        4,7,6,6,5,4,
        0,1,2,2,3,0,
    };
    // clang-format on

    std::array<Vertex, 8> vertices{
        // Bottom
        Vertex{{-1.0f, -1.0f, 1.0f}, 0.0f, {0.0f, -1.0f, 0.0f}, 0.0f},
        Vertex{{1.0f, -1.0f, 1.0f}, 1.0f, {0.0f, -1.0f, 0.0f}, 0.0f},
        Vertex{{1.0f, -1.0f, -1.0f}, 1.0f, {0.0f, -1.0f, 0.0f}, 1.0f},
        Vertex{{-1.0f, -1.0f, -1.0f}, 0.0f, {0.0f, -1.0f, 0.0f}, 1.0f},
        // Top
        Vertex{{-1.0f, 1.0f, 1.0f}, 0.0f, {0.0f, 1.0f, 0.0f}, 1.0f},
        Vertex{{1.0f, 1.0f, 1.0f}, 1.0f, {0.0f, 1.0f, 0.0f}, 1.0f},
        Vertex{{1.0f, 1.0f, -1.0f}, 1.0f, {0.0f, 1.0f, 0.0f}, 0.0f},
        Vertex{{-1.0f, 1.0f, -1.0f}, 0.0f, {0.0f, 1.0f, 0.0f}, 0.0f},
    };

    return Mesh{core, vertices, indices};
};

} // namespace ving
