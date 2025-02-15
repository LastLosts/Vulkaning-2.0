#pragma once

#include "glm/mat4x4.hpp"
#include "graphics_pipeline.hpp"
#include "mesh.hpp"
#include "perspective_camera.hpp"
#include "render_frames.hpp"

namespace ving
{
struct PrimitiveParameters
{
    glm::vec2 position;
    float scale;
    glm::vec3 color;
};

struct PrimitiveParameters3D
{
    glm::vec3 position;
    float scale;
    glm::vec3 color;
};

enum class PrimitiveType
{
    Circle
};
enum class PrimitiveType3D
{
    Sphere
};
class PrimitivesRenderer
{
    struct PushConstants
    {
        glm::mat4 ortho;

        glm::vec2 position;
        glm::vec2 dummy;

        glm::vec3 color;
        float scale;

        VkDeviceAddress vertex_buffer_address;
    };
    struct PushConstants3D
    {
        glm::mat4 pvm_matrix; // Projection View Model

        float scale;
        glm::vec3 color;

        VkDeviceAddress vertex_buffer_address;
    };

  public:
    PrimitivesRenderer(const VulkanCore &core);

    // set color if you want to override the color
    void render(const FrameInfo &frame, PrimitiveType type, std::span<PrimitiveParameters> parameters,
                glm::vec3 color = glm::vec3{-1});

    void render_3d(PrimitiveType3D type, std::span<PrimitiveParameters3D> parameters, const FrameInfo &frame,
                   PerspectiveCamera &camera);

  private:
    PushConstants m_push;
    PushConstants3D m_push3d;

    Mesh m_quad;

    ShaderResources m_resources;
    GraphicsPipline m_circle_pipeline;
    GraphicsPipline m_sphere_pipeline;
};
} // namespace ving
