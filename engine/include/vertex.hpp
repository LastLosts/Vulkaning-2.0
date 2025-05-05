#pragma once

#include "math/vec3.hpp"

namespace ving
{
struct Vertex
{
    vec3 position;
    float uv_x;
    vec3 normal;
    float uv_y;
};
} // namespace ving
