#pragma once

#include "glm/mat4x4.hpp"
#include "graphics_pipeline.hpp"
#include "mesh.hpp"
#include "render_frames.hpp"

namespace ving
{
enum class PrimitiveType
{
    Circle
};
class PrimitivesRenderer
{

    struct PushConstants
    {
        glm::mat4 ortho;
        glm::vec2 position;
        float scale;
        float dummy;
        VkDeviceAddress vertex_buffer_address;
    };

  public:
    PrimitivesRenderer(const VulkanCore &core);

    void render(PrimitiveType type, std::span<glm::vec3> positions_scales, const FrameInfo &frame);

  private:
    PushConstants m_push;

    Mesh m_quad;

    ShaderResources m_resources;
    GraphicsPipline m_circle_pipeline;
};
} // namespace ving
