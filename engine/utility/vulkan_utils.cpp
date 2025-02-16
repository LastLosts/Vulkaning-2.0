#include "vulkan_utils.hpp"
#include <cassert>
#include <fstream>
#include <iostream>

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

    VkResult r = vkCreateInstance(&create_info, nullptr, &instance);

    if (r != VK_SUCCESS)
    {
        std::cout << "Failed to create vulkan instance :  " << r << '\n';
        return VK_NULL_HANDLE;
    }

    return instance;
}
VkPhysicalDevice pick_vulkan_physical_device(std::span<VkPhysicalDevice> available_devices)
{
    VkPhysicalDevice physical_device{};

    for (auto &&device : available_devices)
    {
        VkPhysicalDeviceVulkan12Features features12{};
        features12.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES;

        VkPhysicalDeviceVulkan13Features features_vulkan13{};
        features_vulkan13.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES;
        features_vulkan13.pNext = &features12;

        VkPhysicalDeviceFeatures2 features2{};
        features2.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
        features2.pNext = &features_vulkan13;

        vkGetPhysicalDeviceFeatures2(device, &features2);

        if (features_vulkan13.dynamicRendering && features12.bufferDeviceAddress)
            physical_device = device;
    }

    if (physical_device == VK_NULL_HANDLE)
    {
        std::cout << "Could not find suitable vulkan device\n";
        return VK_NULL_HANDLE;
    }

    return physical_device;
}
VkDevice create_vulkan_device(VkPhysicalDevice physical_device, std::span<VkDeviceQueueCreateInfo> device_queue_infos,
                              std::span<const char *> device_extensions)
{
    VkDevice device{};

    VkPhysicalDeviceVulkan12Features features12{};
    features12.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES;
    features12.bufferDeviceAddress = VK_TRUE;

    VkPhysicalDeviceVulkan13Features features13{};
    features13.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES;
    features13.dynamicRendering = VK_TRUE;
    features13.synchronization2 = VK_TRUE;
    features13.pNext = &features12;

    VkPhysicalDeviceFeatures2 features2{};
    features2.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
    features2.pNext = &features13;

    /*vkGetPhysicalDeviceFeatures2(physical_device, &features2);*/

    VkDeviceCreateInfo create_info{};
    create_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    create_info.queueCreateInfoCount = device_queue_infos.size();
    create_info.pQueueCreateInfos = device_queue_infos.data();
    create_info.enabledExtensionCount = device_extensions.size();
    create_info.ppEnabledExtensionNames = device_extensions.data();
    create_info.pNext = &features2;

    if (vkCreateDevice(physical_device, &create_info, nullptr, &device) != VK_SUCCESS)
    {
        std::cout << "Failed to create vulkan device\n";
    }

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
    {
        std::cout << "Failed to create vulkan command pool\n";
        return VK_NULL_HANDLE;
    }

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
    {
        std::cout << "Failed to allocate vulkan command buffers";
        return {};
    }

    return buffers;
}
VkSemaphore create_vulkan_semaphore(VkDevice device)
{
    VkSemaphore semaphore{};

    VkSemaphoreCreateInfo info{};
    info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    if (vkCreateSemaphore(device, &info, nullptr, &semaphore) != VK_SUCCESS)
    {
        std::cout << "Failed to create vulkan semaphore";
        return VK_NULL_HANDLE;
    }
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
    {
        std::cout << "Failed to create vulkan fence\n";
        return VK_NULL_HANDLE;
    }

    return fence;
}

// TODO: Most of the stuff is hardcoded
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

    assert(!(surface_capabilites.maxImageCount != 0 && image_count < surface_capabilites.maxImageCount));

    if (image_count < surface_capabilites.minImageCount)
    {
        std::cout << "Warning: prefered image count of " << image_count
                  << " is less then vulkan surface capabilities min image count " << surface_capabilites.minImageCount
                  << ", changing image count to surface capabilites min image count" << std::endl;
        image_count = surface_capabilites.minImageCount;
    }

    VkPresentModeKHR present_mode = VK_PRESENT_MODE_FIFO_KHR;
    /*VkPresentModeKHR present_mode = VK_PRESENT_MODE_IMMEDIATE_KHR;*/

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

    if (vkCreateSwapchainKHR(device, &info, nullptr, &swapchain) != VK_SUCCESS)
    {
        std::cout << "Failed to create swapchain\n";
        return VK_NULL_HANDLE;
    }

    return swapchain;
}
VkImage create_vulkan_image(VkDevice device, VkFormat format, VkExtent3D extent, VkImageUsageFlags usage,
                            VkImageType type)
{
    VkImage image{};

    VkImageCreateInfo info{};
    info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    info.imageType = type;
    info.format = format;
    info.extent = extent;
    info.mipLevels = 1;
    info.arrayLayers = 1;
    info.samples = VK_SAMPLE_COUNT_1_BIT;
    info.tiling = VK_IMAGE_TILING_OPTIMAL;
    info.usage = usage;

    if (vkCreateImage(device, &info, nullptr, &image) != VK_SUCCESS)
    {
        std::cout << "Failed to create vulkan image\n";
        return VK_NULL_HANDLE;
    }

    return image;
}
VkImageView create_vulkan_image_view(VkDevice device, VkImage image, VkFormat format)
{
    VkImageView view{};

    VkImageSubresourceRange subresource_range{};
    subresource_range.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    if (format == VK_FORMAT_D32_SFLOAT)
    {
        subresource_range.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
    }
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

    if (vkCreateImageView(device, &info, nullptr, &view) != VK_SUCCESS)
    {
        std::cout << "Failed to create image view\n";
        return VK_NULL_HANDLE;
    }

    return view;
}
// TODO: when file not loads error code -1, when shader module doesn't create error code -2
bool load_vulkan_shader_module(VkDevice device, const char *file_path, VkShaderModule &out_shader)
{
    std::ifstream file(file_path, std::ios::ate | std::ios::binary);

    if (!file.is_open())
    {
        std::cerr << "Failed to open shader file: " << file_path << '\n';
        return false;
    }

    size_t file_size = static_cast<size_t>(file.tellg());

    std::vector<uint32_t> buffer(file_size / sizeof(uint32_t));
    file.seekg(0);

    file.read(reinterpret_cast<char *>(buffer.data()), file_size);

    file.close();

    VkShaderModuleCreateInfo info{};
    info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    info.codeSize = buffer.size() * sizeof(uint32_t);
    info.pCode = buffer.data();

    if (vkCreateShaderModule(device, &info, nullptr, &out_shader) != VK_SUCCESS)
    {
        std::cerr << "Failed to create vulkan shader module\n";
        return false;
    }
    return true;
}
void transition_image(VkCommandBuffer cmd, VkImage image, VkImageLayout current_layout, VkImageLayout new_layout)
{
    VkImageSubresourceRange subresource_range{};
    subresource_range.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    if (new_layout == VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL)
        subresource_range.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
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
void copy_image_to_image(VkCommandBuffer cmd, VkImage source, VkImage destination, VkExtent2D src_size,
                         VkExtent2D dst_size)
{
    VkImageBlit2 blit{};
    blit.sType = VK_STRUCTURE_TYPE_IMAGE_BLIT_2;

    blit.srcOffsets[1].x = src_size.width;
    blit.srcOffsets[1].y = src_size.height;
    blit.srcOffsets[1].z = 1;

    blit.dstOffsets[1].x = dst_size.width;
    blit.dstOffsets[1].y = dst_size.height;
    blit.dstOffsets[1].z = 1;

    blit.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    blit.srcSubresource.baseArrayLayer = 0;
    blit.srcSubresource.layerCount = 1;
    blit.srcSubresource.mipLevel = 0;

    blit.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    blit.dstSubresource.baseArrayLayer = 0;
    blit.dstSubresource.layerCount = 1;
    blit.dstSubresource.mipLevel = 0;

    VkBlitImageInfo2 info{};
    info.sType = VK_STRUCTURE_TYPE_BLIT_IMAGE_INFO_2;
    info.srcImage = source;
    info.srcImageLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
    info.dstImage = destination;
    info.dstImageLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
    info.filter = VK_FILTER_LINEAR;
    info.regionCount = 1;
    info.pRegions = &blit;

    vkCmdBlitImage2(cmd, &info);
}
} // namespace ving
