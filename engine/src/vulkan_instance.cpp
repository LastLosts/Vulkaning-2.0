#include "vulkan_instance.hpp"
#include "SDL3/SDL_init.h"
#include "SDL3/SDL_vulkan.h"
#include "utility/vulkan_utils.hpp"
#include <stdexcept>
#include <vector>

namespace ving
{
VulkanInstance::VulkanInstance()
{
    if (!SDL_Init(SDL_INIT_VIDEO))
        throw std::runtime_error("Failed to initialize SDL");

    std::vector<const char *> instance_layers{
        "VK_LAYER_KHRONOS_validation",
    };

    uint32_t count;
    const char *const *window_extensions = SDL_Vulkan_GetInstanceExtensions(&count);
    std::vector<const char *> instance_extensions{};
    instance_extensions.reserve(count);

    for (size_t i = 0; i < count; ++i)
    {
        instance_extensions.push_back(window_extensions[i]);
    }

    /*instance_extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);*/

    instance = create_vulkan_instance(instance_extensions, instance_layers);
}
VulkanInstance::~VulkanInstance()
{
    vkDestroyInstance(instance, nullptr);
}

} // namespace ving
