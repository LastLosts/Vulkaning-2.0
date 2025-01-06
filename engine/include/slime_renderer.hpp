#pragma once

#include "compute_pipeline.hpp"
#include "render_frames.hpp"
#include "shader_resources.hpp"

namespace ving
{
class SlimeRenderer
{
  public:
    SlimeRenderer(const VulkanCore &core);

    void render(const RenderFrames::FrameInfo &frame);

  private:
    Texture2D m_slime_img;

    ShaderResources m_resources;
    ComputePipeline m_pipeline;
};
} // namespace ving
