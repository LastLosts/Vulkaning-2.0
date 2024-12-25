#include "vulkan_core.hpp"
#include "utility/vulkan_create.hpp"

namespace ving {
VulkanCore::VulkanCore() {
  m_instance = create_vulkan_instance({}, {});
}
} // namespace ving
