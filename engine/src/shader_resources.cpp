#include "shader_resources.hpp"
#include <cassert>
#include <stdexcept>
#include <vulkan/vulkan_core.h>

namespace ving
{
ShaderResources::ShaderResources() : m_device{VK_NULL_HANDLE}, m_pool{VK_NULL_HANDLE}
{
}
ShaderResources::ShaderResources(VkDevice device, std::span<ShaderBindingSet> sets, VkShaderStageFlags stage)
    : m_device{device}
{
    m_shader_sets.reserve(sets.size());

    for (auto &&set : sets)
    {
        m_shader_sets.push_back(set);
    }

    m_layouts.reserve(sets.size());

    std::vector<VkDescriptorPoolSize> sizes{};

    for (auto &&set : sets)
    {
        std::vector<VkDescriptorSetLayoutBinding> bindings{};
        bindings.reserve(set.bindings.size());

        for (auto &&binding : set.bindings)
        {
            sizes.push_back(VkDescriptorPoolSize{binding.type, 1});
            bindings.push_back(VkDescriptorSetLayoutBinding{binding.binding, binding.type, 1, stage});
        }

        VkDescriptorSetLayoutCreateInfo layout_info{};
        layout_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        layout_info.bindingCount = bindings.size();
        layout_info.pBindings = bindings.data();

        VkDescriptorSetLayout layout{};

        if (vkCreateDescriptorSetLayout(m_device, &layout_info, nullptr, &layout) != VK_SUCCESS)
            throw std::runtime_error("Failed to create descriptor set layout");

        assert(layout != VK_NULL_HANDLE);

        m_layouts.push_back(layout);
    }

    VkDescriptorPoolCreateInfo pool_info{};
    pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    pool_info.maxSets = sets.size();
    pool_info.poolSizeCount = sizes.size();
    pool_info.pPoolSizes = sizes.data();

    if (vkCreateDescriptorPool(m_device, &pool_info, nullptr, &m_pool) != VK_SUCCESS)
        throw std::runtime_error("Failed to create descriptor pool");

    VkDescriptorSetAllocateInfo alloc_info{};
    alloc_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    alloc_info.descriptorSetCount = m_layouts.size();
    alloc_info.pSetLayouts = m_layouts.data();
    alloc_info.descriptorPool = m_pool;

    m_sets.resize(m_layouts.size());
    if (vkAllocateDescriptorSets(m_device, &alloc_info, m_sets.data()) != VK_SUCCESS)
        throw std::runtime_error("Failed to allocate descriptor sets");
}
ShaderResources::~ShaderResources()
{
    for (auto &&layout : m_layouts)
    {
        if (layout != VK_NULL_HANDLE)
            vkDestroyDescriptorSetLayout(m_device, layout, nullptr);
    }
    if (m_pool != VK_NULL_HANDLE)
        vkDestroyDescriptorPool(m_device, m_pool, nullptr);
}
void ShaderResources::write_image(uint32_t set, uint32_t binding, const Texture2D &image, VkImageLayout expected_layout)
{
    VkDescriptorImageInfo image_info{};
    image_info.imageView = image.view();
    image_info.imageLayout = expected_layout;

    VkWriteDescriptorSet write{};
    write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    write.dstSet = m_sets[set];
    write.dstBinding = binding;
    write.descriptorCount = 1;
    write.descriptorType = m_shader_sets[set].bindings[binding].type;
    write.pImageInfo = &image_info;

    vkUpdateDescriptorSets(m_device, 1, &write, 0, nullptr);
}
void ShaderResources::write_buffer(uint32_t set, uint32_t binding, const GPUBuffer &buffer)
{
    VkDescriptorBufferInfo buffer_info{};
    buffer_info.buffer = buffer.buffer();
    buffer_info.offset = 0;
    buffer_info.range = buffer.size();

    VkWriteDescriptorSet write{};
    write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    write.dstSet = m_sets[set];
    write.dstBinding = binding;
    write.descriptorCount = 1;
    write.descriptorType = m_shader_sets[set].bindings[binding].type;
    write.pBufferInfo = &buffer_info;

    vkUpdateDescriptorSets(m_device, 1, &write, 0, nullptr);
}
} // namespace ving
