#include <vulkan/vulkan_core.h>
#define VMA_IMPLEMENTATION
#include "vulkan_core.hpp"

#include "utility/vulkan_utils.hpp"
#include <algorithm>
#include <stdexcept>
#include <vector>

namespace ving
{
VulkanCore::VulkanCore(const VulkanInstance &instance, const Window &window) : m_instance{instance.instance}
{
    std::vector<const char *> device_extensions{
        VK_KHR_SWAPCHAIN_EXTENSION_NAME,
        VK_KHR_DYNAMIC_RENDERING_EXTENSION_NAME,
    };

    uint32_t device_count{};
    vkEnumeratePhysicalDevices(m_instance, &device_count, nullptr);
    std::vector<VkPhysicalDevice> available_devices{device_count};
    vkEnumeratePhysicalDevices(m_instance, &device_count, available_devices.data());

    m_physical_device = pick_vulkan_physical_device(available_devices);

    VkPhysicalDeviceMemoryProperties memory_properties{};
    vkGetPhysicalDeviceMemoryProperties(m_physical_device, &memory_properties);

    uint32_t family_count{};
    vkGetPhysicalDeviceQueueFamilyProperties(m_physical_device, &family_count, nullptr);
    std::vector<VkQueueFamilyProperties> queue_family_properties{family_count};
    vkGetPhysicalDeviceQueueFamilyProperties(m_physical_device, &family_count, queue_family_properties.data());

    auto graphics_it =
        std::find_if(queue_family_properties.begin(), queue_family_properties.end(),
                     [](VkQueueFamilyProperties properties) { return properties.queueFlags | VK_QUEUE_GRAPHICS_BIT; });
    if (graphics_it != queue_family_properties.end())
        m_graphics_queue_family = std::distance(queue_family_properties.begin(), graphics_it);
    else
        throw std::runtime_error("Failed to find transfer queue");

    uint32_t transfer_queue_family{};
    auto transfer_it =
        std::find_if(queue_family_properties.begin(), queue_family_properties.end(),
                     [](VkQueueFamilyProperties properties) { return properties.queueFlags | VK_QUEUE_TRANSFER_BIT; });
    if (transfer_it != queue_family_properties.end())
        transfer_queue_family = std::distance(queue_family_properties.begin(), transfer_it);
    else
        throw std::runtime_error("Failed to find transfer queue");

    float queue_priority = 1.0f;

    VkBool32 supported;
    for (uint32_t i = 0; i < queue_family_properties.size(); ++i)
    {
        vkGetPhysicalDeviceSurfaceSupportKHR(m_physical_device, i, window.vulkan_surface(), &supported);
        if (supported)
        {
            m_present_queue_family = i;
            break;
        }
    }
    if (!supported)
    {
        throw std::runtime_error("Failed to find present queue");
    }

    std::vector<uint32_t> queue_families{m_graphics_queue_family, transfer_queue_family, m_present_queue_family};
    queue_families.erase(std::unique(queue_families.begin(), queue_families.end()), queue_families.end());

    std::vector<VkDeviceQueueCreateInfo> queue_create_infos{};

    for (auto &&queue_family : queue_families)
    {
        VkDeviceQueueCreateInfo info{};
        info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        info.queueCount = 1;
        info.pQueuePriorities = &queue_priority;
        info.queueFamilyIndex = queue_family;

        queue_create_infos.push_back(info);
    }

    m_device = create_vulkan_device(m_physical_device, queue_create_infos, device_extensions);

    VmaAllocatorCreateInfo allocator_info{};
    allocator_info.instance = m_instance;
    allocator_info.physicalDevice = m_physical_device;
    allocator_info.device = m_device;
    allocator_info.flags = VMA_ALLOCATOR_CREATE_BUFFER_DEVICE_ADDRESS_BIT;

    vmaCreateAllocator(&allocator_info, &m_allocator);

    vkGetDeviceQueue(m_device, transfer_queue_family, 0, &m_transfer_queue);
    m_immediate_transfer_pool =
        create_vulkan_command_pool(m_device, transfer_queue_family, VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT);
    std::vector<VkCommandBuffer> buffers = allocate_vulkan_command_buffers(m_device, m_immediate_transfer_pool, 1);
    m_immediate_transfer_commands = buffers[0];
    m_immediate_transfer_fence = create_vulkan_fence(m_device, false);
}
VulkanCore::~VulkanCore()
{
    vkDestroyCommandPool(m_device, m_immediate_transfer_pool, nullptr);
    vkDestroyFence(m_device, m_immediate_transfer_fence, nullptr);

    vmaDestroyAllocator(m_allocator);
    vkDestroyDevice(m_device, nullptr);
}
void VulkanCore::immediate_transfer(std::function<void(VkCommandBuffer cmd)> &&func) const
{
    VkCommandBuffer cmd = m_immediate_transfer_commands;

    vkResetFences(m_device, 1, &m_immediate_transfer_fence);
    vkResetCommandBuffer(cmd, 0);

    VkCommandBufferBeginInfo begin_info{};
    begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    begin_info.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

    vkBeginCommandBuffer(cmd, &begin_info);
    func(cmd);
    vkEndCommandBuffer(cmd);

    VkCommandBufferSubmitInfo cmd_info{};
    cmd_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_SUBMIT_INFO;
    cmd_info.commandBuffer = cmd;

    VkSubmitInfo2 submit{};
    submit.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO_2;
    submit.commandBufferInfoCount = 1;
    submit.pCommandBufferInfos = &cmd_info;

    vkQueueSubmit2(m_transfer_queue, 1, &submit, m_immediate_transfer_fence);

    vkWaitForFences(m_device, 1, &m_immediate_transfer_fence, VK_TRUE, 1000000000);
}
} // namespace ving
