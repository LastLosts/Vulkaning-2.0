#version 450

layout (location = 0) in vec2 in_uv;
layout (location = 1) in vec3 in_color;

layout (location = 0) out vec4 out_frag_color;

void main()
{
    vec2 center_uv = in_uv * 2.0 - 1.0; 

    float fade = 0.01;

    float distance = 1.0 - length(center_uv);
    vec4 circle = vec4(smoothstep(0.0, fade, distance));

    out_frag_color = vec4(circle.x * in_color.x, circle.y * in_color.y, circle.z * in_color.z, circle.w);
}
