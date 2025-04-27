#version 450
#extension GL_EXT_buffer_reference : require

layout (location = 0) out vec2 out_uv;

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
    mat4 pvm_matrix;
    VertexBuffer vertex_buffer;
} pc;

void main()
{
    Vertex v = pc.vertex_buffer.vertices[gl_VertexIndex];

    gl_Position = pc.pvm_matrix * vec4(v.position, 1.0);

    out_uv.x = v.uv_x;
    out_uv.y = v.uv_y;
}
