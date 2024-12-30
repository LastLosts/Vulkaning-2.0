#include "vulkan_utils.hpp"
#include <cassert>
#include <iostream>
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

    if (vkCreateInstance(&create_info, nullptr, &instance) != VK_SUCCESS)
        throw std::runtime_error("Failed to create vulkan instance");

    return instance;
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

    VkPhysicalDeviceVulkan13Features features13{};
    features13.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES;

    VkPhysicalDeviceFeatures2 features2{};
    features2.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
    features2.pNext = &features13;

    vkGetPhysicalDeviceFeatures2(physical_device, &features2);

    VkDeviceCreateInfo create_info{};
    create_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    create_info.queueCreateInfoCount = device_queue_infos.size();
    create_info.pQueueCreateInfos = device_queue_infos.data();
    create_info.enabledExtensionCount = device_extensions.size();
    create_info.ppEnabledExtensionNames = device_extensions.data();
    create_info.pNext = &features2;

    if (vkCreateDevice(physical_device, &create_info, nullptr, &device) != VK_SUCCESS)
        throw std::runtime_error("Failed to create vulkan device");

    return device;
}
VkCommandPool create_vulkan_command_pool(VkDevice device, uint32_t queue_family, VkCommandPoolCreateFlags flags)
{
    VkCommandPool command_pool{};

    VkCommandPoolCreateInfo info{};
    info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    info.queueFamilyIndex = queue_family;
    info.flags = flags;

    if (vkCreateCommandPool(device, &info, nullptr, &command_pool) != VK_SUCCESS)
        throw std::runtime_error("Failed to create vulkan command pool");

    return command_pool;
}
std::vector<VkCommandBuffer> allocate_vulkan_command_buffers(VkDevice device, VkCommandPool command_pool,
                                                             uint32_t count)
{
    std::vector<VkCommandBuffer> buffers{};
    buffers.resize(count);

    VkCommandBufferAllocateInfo info{};
    info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    info.commandPool = command_pool;
    info.commandBufferCount = count;

    if (vkAllocateCommandBuffers(device, &info, buffers.data()) != VK_SUCCESS)
        throw std::runtime_error("Failed to allocate vulkan command buffers");

    return buffers;
}
VkSemaphore create_vulkan_semaphore(VkDevice device)
{
    VkSemaphore semaphore{};

    VkSemaphoreCreateInfo info{};
    info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    if (vkCreateSemaphore(device, &info, nullptr, &semaphore) != VK_SUCCESS)
        throw std::runtime_error("Failed to create vulkan semaphore");
    return semaphore;
}
VkFence create_vulkan_fence(VkDevice device, bool initial_state)
{
    VkFence fence{};

    VkFenceCreateInfo info{};
    info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;

    if (initial_state)
        info.flags = VK_FENCE_CREATE_SIGNALED_BIT;

    if (vkCreateFence(device, &info, nullptr, &fence) != VK_SUCCESS)
        throw std::runtime_error("Failed to create vulkan fence");

    return fence;
}
VkSwapchainKHR create_vulkan_swapchain(VkPhysicalDevice physical_device, VkDevice device, VkSurfaceKHR surface,
                                       VkExtent2D extent, uint32_t queue_family_count, uint32_t image_count)
{
    VkSwapchainKHR swapchain{};

    uint32_t formats_count;
    vkGetPhysicalDeviceSurfaceFormatsKHR(physical_device, surface, &formats_count, nullptr);
    std::vector<VkSurfaceFormatKHR> formats{};
    formats.resize(formats_count);
    vkGetPhysicalDeviceSurfaceFormatsKHR(physical_device, surface, &formats_count, formats.data());
    assert(!formats.empty());

    VkSurfaceCapabilitiesKHR surface_capabilites{};
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physical_device, surface, &surface_capabilites);

    VkPresentModeKHR present_mode = VK_PRESENT_MODE_FIFO_KHR;

    VkSwapchainCreateInfoKHR info{};
    info.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    info.surface = surface;
    info.minImageCount = image_count;
    info.imageFormat = VK_FORMAT_B8G8R8A8_UNORM;
    info.imageColorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;
    info.imageExtent = extent;
    info.imageArrayLayers = 1;
    info.imageUsage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    info.queueFamilyIndexCount = queue_family_count;
    info.preTransform = surface_capabilites.currentTransform;
    info.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    info.presentMode = present_mode;
    info.imageSharingMode = (queue_family_count > 1) ? VK_SHARING_MODE_CONCURRENT : VK_SHARING_MODE_EXCLUSIVE;

    vkCreateSwapchainKHR(device, &info, nullptr, &swapchain);

    return swapchain;
}
VkImageView create_vulkan_image_view(VkDevice device, VkImage image, VkFormat format)
{
    VkImageView view{};

    VkImageSubresourceRange subresource_range{};
    subresource_range.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    subresource_range.baseMipLevel = 0;
    subresource_range.levelCount = 1;
    subresource_range.baseArrayLayer = 0;
    subresource_range.layerCount = 1;

    VkImageViewCreateInfo info{};
    info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    info.viewType = VK_IMAGE_VIEW_TYPE_2D;
    info.format = format;
    info.subresourceRange = subresource_range;
    info.image = image;

    vkCreateImageView(device, &info, nullptr, &view);

    return view;
}
void transition_image(VkCommandBuffer cmd, VkImage image, VkImageLayout current_layout, VkImageLayout new_layout)
{
    VkImageSubresourceRange subresource_range{};
    subresource_range.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    subresource_range.baseMipLevel = 0;
    subresource_range.levelCount = VK_REMAINING_MIP_LEVELS;
    subresource_range.baseArrayLayer = 0;
    subresource_range.layerCount = VK_REMAINING_ARRAY_LAYERS;

    VkImageMemoryBarrier2 image_barrier{};
    image_barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2;
    image_barrier.srcStageMask = VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT;
    image_barrier.srcAccessMask = VK_ACCESS_2_MEMORY_WRITE_BIT;
    image_barrier.dstStageMask = VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT;
    image_barrier.dstAccessMask = VK_ACCESS_2_MEMORY_READ_BIT | VK_ACCESS_2_MEMORY_WRITE_BIT;
    image_barrier.oldLayout = current_layout;
    image_barrier.newLayout = new_layout;
    image_barrier.subresourceRange = subresource_range;
    image_barrier.image = image;

    VkDependencyInfo dependency_info{};
    dependency_info.sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO;
    dependency_info.imageMemoryBarrierCount = 1;
    dependency_info.pImageMemoryBarriers = &image_barrier;

    vkCmdPipelineBarrier2(cmd, &dependency_info);
}
} // namespace ving
