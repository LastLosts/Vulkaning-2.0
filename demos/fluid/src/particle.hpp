#pragma once

#include "math/vec2.hpp"
#include <cstdint>

struct Particle
{
    ving::vec2 position;
    ving::vec2 velocity;
    float density;
    uint32_t cell_id;
};
