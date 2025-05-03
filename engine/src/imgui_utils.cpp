#include "imgui_utils.hpp"

#include "imgui.h"

namespace ving
{
void imgui_text_matrix(const glm::mat4 &m)
{
    ImGui::Text("%f %f %f %f\n%f %f %f %f\n%f %f %f %f\n%f %f %f %f", m[0][0], m[1][0], m[2][0], m[3][0], m[0][1],
                m[1][1], m[2][1], m[3][1], m[0][2], m[1][2], m[2][2], m[3][2], m[0][3], m[1][3], m[2][3], m[3][3]);
}
void imgui_text_vec(const glm::vec3 &v)
{
    ImGui::Text("%f %f %f", v.x, v.y, v.z);
}

} // namespace ving
