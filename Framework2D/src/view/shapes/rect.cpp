#include "view/shapes/rect.h"

#include <imgui.h>

namespace USTC_CG
{
// Draw the rectangle using ImGui
void Rect::draw(const Config& config) const
{
    ImDrawList* draw_list = ImGui::GetWindowDrawList();

    draw_list->AddRect(
        ImVec2(
            config.bias[0] + start_point_x_, config.bias[1] + start_point_y_),
        ImVec2(config.bias[0] + end_point_x_, config.bias[1] + end_point_y_),
        IM_COL32(
            config.line_color[0],
            config.line_color[1],
            config.line_color[2],
            config.line_color[3]),
        0.f,  // No rounding of corners
        ImDrawFlags_None,
        config.line_thickness);
}

void Rect::update(float x, float y)
{
    end_point_x_ = x;
    end_point_y_ = y;
}

}  // namespace USTC_CG
