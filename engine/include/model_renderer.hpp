#pragma once

#include "graphics_pipeline.hpp"
#include "mesh.hpp"
#include "model.hpp"
#include "perspective_camera.hpp"
#include "render_frames.hpp"
#include "shader_resources.hpp"
#include <span>

namespace ving
{
class ModelRenderer
{
    struct PushConstants
    {
        mat4 pvm_matrix;
        VkDeviceAddress vertex_buffer_address;
    };
    struct LightingSettings
    {
        vec3 direction;
    };

  public:
    ModelRenderer(const VulkanCore &core);

    void render(const FrameInfo &frame, const PerspectiveCamera &camera, std::span<Model> models);

  private:
    ShaderResources m_resources;
    GraphicsPipeline m_mesh_pipeline;

    LightingSettings *m_lighting_settings;
    GPUBuffer m_lighting_settings_buffer;
};
} // namespace ving
