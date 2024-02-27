#include "view/shapes/line.h"

#include <imgui.h>

namespace USTC_CG
{
// Draw the line using ImGui
void Line::draw(const Config& config) const
{
    ImDrawList* draw_list = ImGui::GetWindowDrawList();

    draw_list->AddLine(
        ImVec2(
            config.bias[0] + start_point_x_, config.bias[1] + start_point_y_),
        ImVec2(config.bias[0] + end_point_x_, config.bias[1] + end_point_y_),
        IM_COL32(
            config.line_color[0],
            config.line_color[1],
            config.line_color[2],
            config.line_color[3]),
        config.line_thickness);
}

void Line::update(float x, float y)
{
    end_point_x_ = x;
    end_point_y_ = y;
}
}  // namespace USTC_CG