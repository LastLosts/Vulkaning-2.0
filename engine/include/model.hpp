#pragma once

#include "math/mat4.hpp"
#include "mesh.hpp"

namespace ving
{
struct Model
{
    Mesh *mesh;
    vec3 position;
    float scale{1.0f};
    vec3 rotate;

    mat4 mat();
};
} // namespace ving
