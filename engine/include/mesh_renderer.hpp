#pragma once

#include <glm/mat4x4.hpp>

#include "graphics_pipeline.hpp"
#include "mesh.hpp"
#include "perspective_camera.hpp"
#include "render_frames.hpp"
#include "shader_resources.hpp"
#include <span>

namespace ving
{
class MeshRenderer
{
    struct PushConstants
    {
        glm::mat4 pvm_matrix;
        VkDeviceAddress vertex_buffer_address;
    };

  public:
    MeshRenderer(const VulkanCore &core);

    void render(const FrameInfo &frame, const PerspectiveCamera &camera, std::span<Mesh> meshes);

  private:
    ShaderResources m_resources;
    GraphicsPipeline m_mesh_pipeline;
};
} // namespace ving
