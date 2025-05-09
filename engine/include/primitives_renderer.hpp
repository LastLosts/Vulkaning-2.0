#pragma once

#include "graphics_pipeline.hpp"
#include "math/vec2.hpp"
#include "mesh.hpp"
#include "perspective_camera.hpp"
#include "render_frames.hpp"

namespace ving
{
struct PrimitiveParameters
{
    vec2 position;
    float scale;
    vec3 color;
};

struct PrimitiveParameters3D
{
    vec3 position;
    float scale;
    vec3 color;
};

enum class PrimitiveType
{
    Circle,
    Square
};
enum class PrimitiveType3D
{
    Sphere
};
class PrimitivesRenderer
{
    struct PushConstants
    {
        mat4 ortho;

        vec2 position;
        vec2 dummy;

        vec3 color;
        float scale;

        VkDeviceAddress vertex_buffer_address;
    };
    struct PushConstants3D
    {
        mat4 pvm_matrix; // Projection View Model

        float scale;
        vec3 color;

        VkDeviceAddress vertex_buffer_address;
    };

  public:
    PrimitivesRenderer(const VulkanCore &core);

    // set color if you want to override the color
    void render(const FrameInfo &frame, PrimitiveType type, std::span<PrimitiveParameters> parameters,
                vec3 color = vec3{-1});

    void render_3d(PrimitiveType3D type, std::span<PrimitiveParameters3D> parameters, const FrameInfo &frame,
                   PerspectiveCamera &camera);

  private:
    PushConstants m_push;
    PushConstants3D m_push3d;

    Mesh m_quad;

    ShaderResources m_resources;
    GraphicsPipeline m_circle_pipeline;
    GraphicsPipeline m_square_wire_pipeline;

    GraphicsPipeline m_sphere_pipeline;
};
} // namespace ving
