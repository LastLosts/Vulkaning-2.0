#include "texture_2d.hpp"
#include "utility/vulkan_utils.hpp"
#include <vulkan/vulkan_core.h>

namespace ving
{
Texture2D::Texture2D(const VulkanCore &core, VkExtent2D extent, VkImageUsageFlags image_usage,
                     VmaMemoryUsage memory_usage, VkFormat format, VkImageLayout initial_layout)
    : m_device{core.device()}, m_allocator{core.allocator()}, m_layout{initial_layout}
{
    m_extent = extent;
    m_format = format;

    VmaAllocationCreateInfo alloc_info{};
    alloc_info.usage = memory_usage;
    alloc_info.requiredFlags = VkMemoryPropertyFlags{VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT};

    VkImageCreateInfo info{};
    info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    info.imageType = VK_IMAGE_TYPE_2D;
    info.format = format;
    info.extent = VkExtent3D{extent.width, extent.height, 1};
    info.mipLevels = 1;
    info.arrayLayers = 1;
    info.samples = VK_SAMPLE_COUNT_1_BIT;
    info.tiling = VK_IMAGE_TILING_OPTIMAL;
    info.usage = image_usage;

    vmaCreateImage(core.allocator(), &info, &alloc_info, &m_image, &m_allocation, nullptr);

    m_view = create_vulkan_image_view(core.device(), m_image, format);
}
Texture2D::~Texture2D()
{
    vkDestroyImageView(m_device, m_view, nullptr);
    vmaDestroyImage(m_allocator, m_image, m_allocation);
}
void Texture2D::transition_layout(VkCommandBuffer cmd, VkImageLayout new_layout)
{
    transition_image(cmd, m_image, m_layout, new_layout);
    m_layout = new_layout;
}

} // namespace ving
