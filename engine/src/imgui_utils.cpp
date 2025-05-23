#include "imgui_utils.hpp"

#include "imgui.h"
#include "math/mat4.hpp"

namespace ving
{
void imgui_text_matrix(const mat4 &m)
{
    ImGui::Text("%f %f %f %f\n%f %f %f %f\n%f %f %f %f\n%f %f %f %f", m[0][0], m[1][0], m[2][0], m[3][0], m[0][1],
                m[1][1], m[2][1], m[3][1], m[0][2], m[1][2], m[2][2], m[3][2], m[0][3], m[1][3], m[2][3], m[3][3]);
}
void imgui_text_vec(const vec3 &v, const char *text)
{
    ImGui::Text("%s: %f %f %f", text, v.x, v.y, v.z);
}
void imgui_text_vec(const vec4 &v, const char *text)
{
    ImGui::Text("%s: %f %f %f %f", text, v.x, v.y, v.z, v.w);
}

} // namespace ving
