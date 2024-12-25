#include "vulkan_utils.hpp"
#include <stdexcept>
#include <vulkan/vulkan_core.h>

namespace ving
{
VkInstance create_vulkan_instance(std::span<const char *> required_extensions, std::span<const char *> required_layers)
{
    VkInstance instance{};

    VkApplicationInfo app_info{};
    app_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    app_info.apiVersion = VK_API_VERSION_1_3;
    app_info.pEngineName = "Vulkaning Engine";
    app_info.engineVersion = VK_MAKE_VERSION(0, 0, 1);

    VkInstanceCreateInfo create_info{};
    create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    create_info.pApplicationInfo = &app_info;
    create_info.enabledLayerCount = required_layers.size();
    create_info.ppEnabledLayerNames = required_layers.data();
    create_info.enabledExtensionCount = required_extensions.size();
    create_info.ppEnabledExtensionNames = required_extensions.data();

    if (vkCreateInstance(&create_info, nullptr, &instance) == VK_SUCCESS)
        return instance;
    else
        throw std::runtime_error("Failed to create vulkan instance");
}
VkPhysicalDevice pick_vulkan_physical_device(std::span<VkPhysicalDevice> available_devices)
{
    VkPhysicalDevice physical_device{};

    for (auto &&device : available_devices)
    {
        VkPhysicalDeviceVulkan13Features features_vulkan13{};
        features_vulkan13.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES;

        VkPhysicalDeviceFeatures2 features2{};
        features2.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
        features2.pNext = &features_vulkan13;

        vkGetPhysicalDeviceFeatures2(device, &features2);

        if (features_vulkan13.dynamicRendering)
            physical_device = device;
    }

    if (physical_device == VK_NULL_HANDLE)
    {
        throw std::runtime_error("Could not find suitable vulkan device");
    }

    return physical_device;
}
VkDevice create_vulkan_device(VkPhysicalDevice physical_device, std::span<VkDeviceQueueCreateInfo> device_queue_infos,
                              std::span<const char *> device_extensions)
{
    VkDevice device{};

    VkDeviceCreateInfo create_info{};
    create_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    create_info.queueCreateInfoCount = device_queue_infos.size();
    create_info.pQueueCreateInfos = device_queue_infos.data();
    create_info.enabledExtensionCount = device_extensions.size();
    create_info.ppEnabledExtensionNames = device_extensions.data();

    vkCreateDevice(physical_device, &create_info, nullptr, &device);

    return device;
}
} // namespace ving
