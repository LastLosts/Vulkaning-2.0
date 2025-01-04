#include "shader_resources.hpp"
#include <stdexcept>

namespace ving
{
ShaderResources::ShaderResources(VkDevice device, std::span<ShaderBindingSet> sets, VkShaderStageFlags stage)
    : m_device{device}
{
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
        vkDestroyDescriptorSetLayout(m_device, layout, nullptr);
    }
    vkDestroyDescriptorPool(m_device, m_pool, nullptr);
}
} // namespace ving
