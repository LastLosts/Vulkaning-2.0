#pragma once

#include "window.hpp"
#include <memory>
#include <vulkan/vulkan.h>

namespace ving
{
class VulkanCore
{
  public:
    VulkanCore(const Window &window);

  private:
    VkInstance m_instance;
    VkPhysicalDevice m_physical_device;
    VkDevice m_device;
    VkCommandPool m_graphics_command_pool;
};
} // namespace ving
