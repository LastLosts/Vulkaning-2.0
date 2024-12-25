#pragma once

#include <span>
#include <vulkan/vulkan_core.h>

namespace ving {
VkInstance create_vulkan_instance(std::span<const char *> required_extensions,
                           std::span<const char *> required_layers);
}
