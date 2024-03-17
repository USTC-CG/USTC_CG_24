#include <imgui.h>

#include <cmath>

#include "view/shapes/ellipse.h"

namespace USTC_CG
{
// Draw the Ellipse using ImGui
void Ellipse::draw(const Config& config) const
{
    ImDrawList* draw_list = ImGui::GetWindowDrawList();

    draw_list->AddEllipse(
        ImVec2(
            static_cast<float>(
                config.bias[0] + (start_point_x_ + end_point_x_) * 0.5),
            static_cast<float>(
                config.bias[1] + (start_point_y_ + end_point_y_) * 0.5)),
        static_cast<float>(fabs(end_point_x_ - start_point_x_) * 0.5),
        static_cast<float>(fabs(end_point_y_ - start_point_y_) * 0.5),
        IM_COL32(
            config.Ellipse_color[0],
            config.Ellipse_color[1],
            config.Ellipse_color[2],
            config.Ellipse_color[3]),
        0,
        0,
        config.Ellipse_thickness);
}

void Ellipse::update(float x, float y)
{
    end_point_x_ = x;
    end_point_y_ = y;
}
}  // namespace USTC_CG