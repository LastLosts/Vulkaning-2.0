#pragma once

#include <vulkan/vulkan.h>

namespace ving {
class VulkanCore {
public:
  VulkanCore();

private:
  VkInstance m_instance;
};
} // namespace ving
