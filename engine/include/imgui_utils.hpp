#pragma once

#include "math/mat4.hpp"
#include "math/vec3.hpp"

namespace ving
{
struct Model;

void imgui_drag_model_transform(ving::Model &model);
void imgui_text_matrix(const mat4 &m);
void imgui_text_vec(const vec3 &v, const char *text = "");
void imgui_text_vec(const vec4 &v, const char *text = "");
} // namespace ving
