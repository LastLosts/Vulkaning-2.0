#pragma once


struct Particle
{
    vec2 position;
    vec2 velocity;
    float density;
    uint32_t cell_id;
};
