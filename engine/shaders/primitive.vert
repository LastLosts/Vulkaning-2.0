#version 450
#extension GL_EXT_buffer_reference : require

layout (location = 0) out vec2 out_uv;
layout (location = 1) out vec3 out_color;

struct Vertex
{
    vec3 position;
    float uv_x;
    vec3 normal;
    float uv_y;
};
layout (buffer_reference, std430) readonly buffer VertexBuffer
{
    Vertex vertices[];
};
layout (push_constant) uniform constants
{
    mat4 ortho;
    vec2 position;
    vec2 dummy;
    vec3 color;
    float scale;
    VertexBuffer vertex_buffer;
} pc;

void main()
{
    Vertex v= pc.vertex_buffer.vertices[gl_VertexIndex];

    gl_Position = pc.ortho * vec4(vec3(pc.position, 0.0) + pc.scale * v.position, 1.0);

    out_uv.x = v.uv_x;
    out_uv.y = v.uv_y;
    out_color = pc.color;
}
