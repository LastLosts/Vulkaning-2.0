#pragma once

#include <span>
#include <vector>
#include <vulkan/vulkan_core.h>

namespace ving
{
VkInstance create_vulkan_instance(std::span<const char *> required_extensions, std::span<const char *> required_layers);
VkPhysicalDevice pick_vulkan_physical_device(std::span<VkPhysicalDevice> available_devices);
VkDevice create_vulkan_device(VkPhysicalDevice physical_device, std::span<VkDeviceQueueCreateInfo> device_queue_infos,
                              std::span<const char *> device_extensions);
VkCommandPool create_vulkan_command_pool(VkDevice device, uint32_t queue_family, VkCommandPoolCreateFlags flags);
std::vector<VkCommandBuffer> allocate_vulkan_command_buffers(VkDevice device, VkCommandPool command_pool,
                                                             uint32_t count);
VkSemaphore create_vulkan_semaphore(VkDevice device);
VkFence create_vulkan_fence(VkDevice device, bool initial_state);

VkSwapchainKHR create_vulkan_swapchain(VkPhysicalDevice physical_device, VkDevice device, VkSurfaceKHR surface,
                                       VkExtent2D extent, uint32_t queue_family_count, uint32_t image_count);

VkImageView create_vulkan_image_view(VkDevice device, VkImage image, VkFormat format);

void transition_image(VkCommandBuffer cmd, VkImage image, VkImageLayout current_layout, VkImageLayout new_layout);
} // namespace ving
