#include "view/shapes/Freehand.h"

#include <imgui.h>

namespace USTC_CG
{

void Freehand::draw(const Config& config) const
{
    ImDrawList* draw_list = ImGui::GetWindowDrawList();
    for (int i = 0; i < points.size() - 1; i++)
    {
        draw_list->AddLine(
            ImVec2(config.bias[0] + points[i].x, config.bias[1] + points[i].y),
            ImVec2(
                config.bias[0] + points[i + 1].x,
                config.bias[1] + points[i + 1].y),
            IM_COL32(
                config.line_color[0],
                config.line_color[1],
                config.line_color[2],
                config.line_color[3]),
            config.line_thickness);
    }
}

void Freehand::update(float x, float y)
{
    add_point(x, y);
    config.index++;  // update the index
    draw(config);
}

void Freehand::set_index(int index)
{
    config.index = index;
}  // namespace USTC_CG

void Freehand::add_point(float x, float y)
{
    points.push_back(point(x, y));
}

int Freehand::get_index()
{
    return config.index;
}  // namespace USTC_CG
}  // namespace USTC_CG