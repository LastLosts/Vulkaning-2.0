#pragma once

#include <vulkan/vulkan_core.h>

namespace ving
{
struct VulkanInstance
{
    VulkanInstance();
    ~VulkanInstance();

    VulkanInstance(const VulkanInstance &) = delete;
    VulkanInstance &operator=(const VulkanInstance &) = delete;
    VulkanInstance(VulkanInstance &&) = delete;
    VulkanInstance &operator=(VulkanInstance &&) = delete;

    VkInstance instance;
};
} // namespace ving
