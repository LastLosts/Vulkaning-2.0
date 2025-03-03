#pragma once

#include "glm/ext/vector_float2.hpp"

struct Particle
{
    glm::vec2 position;
    glm::vec2 velocity;
    float density;
    uint32_t cell_id;
};
