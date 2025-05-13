#pragma once

#include <span>
#include <vector>
#include <vulkan/vulkan_core.h>

namespace ving
{
// Create vulkan stuff
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
                                       VkExtent2D extent, uint32_t queue_family_count, uint32_t preferred_image_count,
                                       VkPresentModeKHR prefered_present_mode = VK_PRESENT_MODE_FIFO_KHR);
VkImage create_vulkan_image(VkDevice device, VkFormat format, VkExtent3D extent, VkImageUsageFlags usage,
                            VkImageType type = VK_IMAGE_TYPE_2D);
VkImageView create_vulkan_image_view(VkDevice device, VkImage image, VkFormat format);
bool load_vulkan_shader_module(VkDevice device, const char *file_path, VkShaderModule &out_shader);

// Images
void transition_image(VkCommandBuffer cmd, VkImage image, VkImageLayout current_layout, VkImageLayout new_layout);
void copy_image_to_image(VkCommandBuffer cmd, VkImage source, VkImage destination, VkExtent2D src_size,
                         VkExtent2D dst_size);
} // namespace ving
