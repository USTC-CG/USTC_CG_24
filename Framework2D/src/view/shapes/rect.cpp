#include "view/shapes/rect.h"

namespace USTC_CG
{
Rect::Rect()
{
}

Rect::~Rect()
{
}

void Rect::draw(const Settings& settings)
{
    ImDrawList* draw_list = ImGui::GetWindowDrawList();

    draw_list->AddRect(
        ImVec2(
            settings.bias.x + start_point.x, settings.bias.y + start_point.y),
        ImVec2(settings.bias.x + end_point.x, settings.bias.y + end_point.y),
        settings.line_color,
        0.f,
        ImDrawFlags_None,
        settings.line_thickness);
}
}  // namespace USTC_CG