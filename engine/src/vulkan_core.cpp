#include "vulkan_core.hpp"
#include "SDL3/SDL_vulkan.h"
#include "utility/vulkan_utils.hpp"
#include <algorithm>
#include <stdexcept>
#include <vector>
#include <vulkan/vulkan_core.h>

namespace ving
{
VulkanCore::VulkanCore(const VulkanInstance &instance, const Window &window) : m_instance{instance.instance}
{
    std::vector<const char *> device_extensions{};

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

    uint32_t present_queue_family;
    VkBool32 supported;
    for (uint32_t i = 0; i < queue_family_properties.size(); ++i)
    {
        vkGetPhysicalDeviceSurfaceSupportKHR(m_physical_device, i, window.vulkan_surface(), &supported);
        if (supported)
        {
            present_queue_family = i;
            break;
        }
    }
    if (!supported)
    {
        throw std::runtime_error("Failed to find suitable present queue");
    }

    std::vector<uint32_t> queue_families{m_graphics_queue_family, transfer_queue_family, present_queue_family};
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
}
VulkanCore::~VulkanCore()
{
    vkDestroyDevice(m_device, nullptr);
}
} // namespace ving
