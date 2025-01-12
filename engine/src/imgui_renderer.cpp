#include "imgui_renderer.hpp"
#include "backends/imgui_impl_sdl3.h"
#include "backends/imgui_impl_vulkan.h"
#include "imgui.h"
#include <stdexcept>
#include <vulkan/vulkan_core.h>

namespace ving
{

ImGuiRenderer::ImGuiRenderer(const VulkanCore &core, const Window &window, const RenderFrames &frames)
    : m_device{core.device()}
{
    VkDescriptorPoolSize pool_sizes[] = {
        VkDescriptorPoolSize{VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1000},
        VkDescriptorPoolSize{VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 1000},
        VkDescriptorPoolSize{VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1000},
        VkDescriptorPoolSize{VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, 1000},
        VkDescriptorPoolSize{VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, 1000},
        VkDescriptorPoolSize{VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1000},
        VkDescriptorPoolSize{VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1000},
        VkDescriptorPoolSize{VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1000},
        VkDescriptorPoolSize{VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 1000},
        VkDescriptorPoolSize{VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 1000},
    };

    VkDescriptorPoolCreateInfo pool_info{};
    pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    pool_info.maxSets = 1000;
    pool_info.poolSizeCount = 10;
    pool_info.pPoolSizes = pool_sizes;
    pool_info.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;

    if (vkCreateDescriptorPool(m_device, &pool_info, nullptr, &m_pool) != VK_SUCCESS)
        throw std::runtime_error("Failed to create descriptor pool for imgui. Do proper error handling");

    ImGui::CreateContext();
    ImGui_ImplSDL3_InitForVulkan(window.window());

    ImGui_ImplVulkan_InitInfo init_info{};
    init_info.Instance = core.instance();
    init_info.PhysicalDevice = core.physical_device();
    init_info.Device = core.device();
    init_info.QueueFamily = core.graphics_queue_family();
    init_info.Queue = frames.graphics_queue();
    init_info.DescriptorPool = m_pool;
    init_info.MinImageCount = 3;
    init_info.ImageCount = 3;
    init_info.MSAASamples = VK_SAMPLE_COUNT_1_BIT;
    init_info.UseDynamicRendering = true;
    init_info.PipelineRenderingCreateInfo = VkPipelineRenderingCreateInfoKHR{};
    init_info.PipelineRenderingCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO;
    init_info.PipelineRenderingCreateInfo.colorAttachmentCount = 1;
    init_info.PipelineRenderingCreateInfo.pColorAttachmentFormats = &RenderFrames::draw_image_format;

    ImGui_ImplVulkan_Init(&init_info);

    ImGui_ImplVulkan_CreateFontsTexture();
}
ImGuiRenderer::~ImGuiRenderer()
{
    vkDeviceWaitIdle(m_device);

    ImGui_ImplVulkan_Shutdown();
    ImGui_ImplSDL3_Shutdown();
    ImGui::DestroyContext();
    vkDestroyDescriptorPool(m_device, m_pool, nullptr);
}

void ImGuiRenderer::render(const FrameInfo &frame, const std::vector<std::function<void()>> &imgui_functions) const
{
    ImGui_ImplVulkan_NewFrame();
    ImGui_ImplSDL3_NewFrame();
    ImGui::NewFrame();

    for (auto &&func : imgui_functions)
    {
        func();
    }

    ImGui::EndFrame();

    ImGui::Render();

    VkCommandBuffer cmd = frame.cmd;

    VkRenderingAttachmentInfo color_attachment{};
    color_attachment.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO;
    color_attachment.imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
    color_attachment.loadOp = VK_ATTACHMENT_LOAD_OP_LOAD;
    color_attachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    color_attachment.imageView = frame.draw_img->view();

    VkRenderingInfo render_info{};
    render_info.sType = VK_STRUCTURE_TYPE_RENDERING_INFO;
    render_info.layerCount = 1;
    render_info.renderArea = VkRect2D{VkOffset2D{0, 0}, frame.draw_img->extent()};
    render_info.colorAttachmentCount = 1;
    render_info.pColorAttachments = &color_attachment;

    frame.draw_img->transition_layout(cmd, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);
    vkCmdBeginRendering(cmd, &render_info);

    ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), cmd);

    vkCmdEndRendering(cmd);
}
} // namespace ving
