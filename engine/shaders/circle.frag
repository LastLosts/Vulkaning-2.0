#version 450

layout (location = 0) in vec2 in_uv;

layout (location = 0) out vec4 out_frag_color;

void main()
{
    vec2 center_uv = in_uv * 2.0 - 1.0; 

    float fade = 0.01;

    out_frag_color = vec4(center_uv, 0.0, 1.0); 

    float distance = 1.0 - length(center_uv);
    vec4 color = vec4(smoothstep(0.0, fade, distance));

    out_frag_color = color; 

    // out_frag_color = vec4(1.0, 1.0, 1.0, 1.0);
}
