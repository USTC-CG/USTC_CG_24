#include "view/shapes/line.h"

namespace USTC_CG
{
Line::Line()
{
}

Line::~Line()
{
}

void Line::draw(const Settings& settings)
{
    ImDrawList* draw_list = ImGui::GetWindowDrawList();

    draw_list->AddLine(
        ImVec2(
            settings.bias.x + start_point.x, settings.bias.y + start_point.y),
        ImVec2(settings.bias.x + end_point.x, settings.bias.y + end_point.y),
        settings.line_color,
        settings.line_thickness);
}
}  // namespace USTC_CG