#version 450

layout (location = 0) in vec2 in_uv;
layout (location = 1) in vec3 in_normal;

layout (location = 0) out vec4 out_frag_color;

layout (set = 0, binding = 0) uniform LightingSettingsUniform
{
    vec3 dir;
} lighting_settings;

const vec4 color = vec4(0.9f, 0.9f, 0.9f, 1.0f);

void main()
{
    // out_frag_color = vec4(in_normal, 1.0);
    // out_frag_color = vec4(1.0);
    
    vec4 shaded = dot(lighting_settings.dir, in_normal) * color;
    shaded.w = 1.0;
    out_frag_color = shaded;
    out_frag_color = vec4(1.0);

    // out_frag_color = vec4(in_uv, 0.0, 1.0);
    // out_frag_color = vec4(1.0);
}
