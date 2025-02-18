#version 450

layout (location = 0) in vec2 in_uv;
layout (location = 1) in vec3 in_color;

layout (location = 0) out vec4 out_frag_color;

void main()
{
    out_frag_color = vec4(in_color, 1.0); 
}
