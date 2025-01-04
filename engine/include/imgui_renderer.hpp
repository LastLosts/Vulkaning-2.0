#pragma once

#include "render_frames.hpp"
#include "vulkan_core.hpp"

namespace ving
{
class ImGuiRenderer
{
  public:
    ImGuiRenderer(const VulkanCore &core, const Window &window, const RenderFrames &frames);
    ~ImGuiRenderer();

    ImGuiRenderer(const ImGuiRenderer &) = delete;
    ImGuiRenderer(ImGuiRenderer &&) = delete;
    ImGuiRenderer &operator=(const ImGuiRenderer &) = delete;
    ImGuiRenderer &operator=(ImGuiRenderer &&) = delete;

    void render(const RenderFrames::FrameInfo &frame, const std::vector<std::function<void()>> &imgui_functions);

  private:
    VkDevice m_device;

    VkDescriptorPool m_pool;
};
} // namespace ving
