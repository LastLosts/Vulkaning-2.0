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
        throw std::runtime_error("Failed to find suitable present queue");
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
}
VulkanCore::~VulkanCore()
{
    vmaDestroyAllocator(m_allocator);
    vkDestroyDevice(m_device, nullptr);
}
} // namespace ving
