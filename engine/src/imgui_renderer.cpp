#include "imgui_renderer.hpp"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_vulkan.h"
#include "imgui.h"
#include "window.hpp"
#include <iostream>

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
        std::cout << "Failed to create descriptor pool for imgui. Do proper error handling\n";

    ImGui::CreateContext();
    ImGui_ImplGlfw_InitForVulkan(window.window(), true);

    ImGui_ImplVulkan_InitInfo init_info{};
    init_info.Instance = core.vulkan_instance();
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
    init_info.PipelineRenderingCreateInfo.depthAttachmentFormat = RenderFrames::depth_image_format;

    ImGui_ImplVulkan_Init(&init_info);
    ImGui_ImplVulkan_CreateFontsTexture();
}
ImGuiRenderer::~ImGuiRenderer()
{
    vkDeviceWaitIdle(m_device);
    ImGui_ImplVulkan_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    vkDestroyDescriptorPool(m_device, m_pool, nullptr);
}

void ImGuiRenderer::render(const FrameInfo &frame, std::function<void()> &&function) const
{
    ImGui_ImplVulkan_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    function();

    ImGui::EndFrame();
    ImGui::Render();

    VkCommandBuffer cmd = frame.cmd;
    ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), cmd);
}
} // namespace ving
