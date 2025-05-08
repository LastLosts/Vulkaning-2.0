#pragma once

#include "mesh.hpp"

#include <string_view>

namespace ving
{
Mesh load_mesh(const VulkanCore &core, std::string_view filepath);
} // namespace ving
