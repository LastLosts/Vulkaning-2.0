#pragma once

#include "glm/ext/matrix_float4x4.hpp"

namespace ving
{
void imgui_text_matrix(const glm::mat4 &m);
void imgui_text_vec(const glm::vec3 &v);
} // namespace ving
