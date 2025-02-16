#include "vulkan_instance.hpp"

#include "GLFW/glfw3.h"
#include "utility/vulkan_utils.hpp"
#include <iostream>
#include <vector>

namespace ving
{
VulkanInstance::VulkanInstance()
{
    if (!glfwInit())
        std::cout << "Failed to initialize glfw\n";

    std::vector<const char *> instance_layers{
        "VK_LAYER_KHRONOS_validation",
    };

    uint32_t count;
    const char *const *window_extensions = glfwGetRequiredInstanceExtensions(&count);
    std::vector<const char *> instance_extensions{};
    instance_extensions.reserve(count);

    for (size_t i = 0; i < count; ++i)
    {
        instance_extensions.push_back(window_extensions[i]);
    }

    instance = create_vulkan_instance(instance_extensions, instance_layers);
}
VulkanInstance::~VulkanInstance()
{
    vkDestroyInstance(instance, nullptr);
}

} // namespace ving
