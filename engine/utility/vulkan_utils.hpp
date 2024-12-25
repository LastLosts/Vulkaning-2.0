#pragma once

#include <span>
#include <vulkan/vulkan_core.h>

namespace ving
{
VkInstance create_vulkan_instance(std::span<const char *> required_extensions, std::span<const char *> required_layers);
VkPhysicalDevice pick_vulkan_physical_device(std::span<VkPhysicalDevice> available_devices);
VkDevice create_vulkan_device(VkPhysicalDevice physical_device, std::span<VkDeviceQueueCreateInfo> device_queue_infos,
                              std::span<const char *> device_extensions);
} // namespace ving
