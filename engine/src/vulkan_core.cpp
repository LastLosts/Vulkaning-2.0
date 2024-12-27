#include "vulkan_core.hpp"
#include "utility/vulkan_utils.hpp"
#include <algorithm>
#include <stdexcept>
#include <vector>

namespace ving
{
VulkanCore::VulkanCore(const Window &window)
{
    std::vector<const char *> instance_layers{
        "VK_LAYER_KHRONOS_validation",
    };
    std::vector<const char *> instance_extensions = window.vulkan_extensions();
    /*std::vector<const char *> instance_extensions{};*/
    /*instance_extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);*/

    std::vector<const char *> device_extensions{};

    m_instance = create_vulkan_instance(instance_extensions, instance_layers);
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

    uint32_t graphics_queue_family{};
    auto graphics_it =
        std::find_if(queue_family_properties.begin(), queue_family_properties.end(),
                     [](VkQueueFamilyProperties properties) { return properties.queueFlags | VK_QUEUE_GRAPHICS_BIT; });
    if (graphics_it != queue_family_properties.end())
        graphics_queue_family = std::distance(queue_family_properties.begin(), graphics_it);
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

    std::vector<uint32_t> queue_families{graphics_queue_family, transfer_queue_family};
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
} // namespace ving
