#pragma once

#include "texture_2d.hpp"
#include <span>
#include <string>
#include <vector>
#include <vulkan/vulkan_core.h>

namespace ving
{
struct ShaderBinding
{
    uint32_t binding;
    VkDescriptorType type;
};
struct ShaderBindingSet
{
    std::string name;
    std::vector<ShaderBinding> bindings;
};
class ShaderResources
{
  public:
    ShaderResources();
    ShaderResources(VkDevice device, std::span<ShaderBindingSet> sets, VkShaderStageFlags stage);
    ~ShaderResources();

    ShaderResources(const ShaderResources &) = delete;
    ShaderResources(ShaderResources &&) = delete;
    ShaderResources &operator=(const ShaderResources &) = delete;
    // Stupid temporary objects
    ShaderResources &operator=(ShaderResources &&other)
    {
        std::swap(m_device, other.m_device);
        std::swap(m_pool, other.m_pool);
        m_layouts = std::move(other.m_layouts);
        m_sets = std::move(other.m_sets);
        m_shader_sets = std::move(other.m_shader_sets);
        return *this;
    }

    [[nodiscard]] const VkDescriptorSetLayout *layouts() const noexcept { return m_layouts.data(); }
    [[nodiscard]] size_t layouts_size() const noexcept { return m_layouts.size(); }

    [[nodiscard]] const VkDescriptorSet *sets() const noexcept { return m_sets.data(); }
    [[nodiscard]] size_t sets_size() const noexcept { return m_sets.size(); }

    void write_image(uint32_t set, uint32_t binding, const Texture2D &image, VkImageLayout expected_layout);

  private:
    VkDevice m_device;

    VkDescriptorPool m_pool;
    std::vector<VkDescriptorSetLayout> m_layouts;
    std::vector<VkDescriptorSet> m_sets;
    std::vector<ShaderBindingSet> m_shader_sets;
};
} // namespace ving
