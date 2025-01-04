#pragma once

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
    ShaderResources(VkDevice device, std::span<ShaderBindingSet> sets, VkShaderStageFlags stage);
    ~ShaderResources();

    [[nodiscard]] std::vector<VkDescriptorSetLayout> layouts() const noexcept { return m_layouts; }

  private:
    VkDevice m_device;

    VkDescriptorPool m_pool;
    std::vector<VkDescriptorSetLayout> m_layouts;
    std::vector<VkDescriptorSet> m_sets;
};
} // namespace ving
