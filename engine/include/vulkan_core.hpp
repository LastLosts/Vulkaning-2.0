#pragma once

#include <vulkan/vulkan.h>

namespace ving
{
class VulkanCore
{
  public:
    VulkanCore();

  private:
    VkInstance m_instance;
    VkPhysicalDevice m_physical_device;
    VkDevice m_device;
    VkCommandPool m_graphics_command_pool;
};
} // namespace ving
