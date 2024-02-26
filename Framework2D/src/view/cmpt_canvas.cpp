#include "view/cmpt_canvas.h"

#include <cmath>
#include <iostream>

#include "imgui.h"
#include "view/shapes/line.h"
#include "view/shapes/rect.h"

namespace USTC_CG
{
void Canvas::draw()
{
    draw_background();

    if (is_hovered && !draw_status &&
        ImGui::IsMouseClicked(ImGuiMouseButton_Left))
        mouse_click_event();
    if (draw_status)
    {
        mouse_drag_event();
        if (!ImGui::IsMouseDown(ImGuiMouseButton_Left))
            mouse_release_event();
    }

    draw_shapes();
}

void Canvas::set_attributes(const ImVec2& min, const ImVec2& size)
{
    canvas_min = min;
    canvas_size = size;
    canvas_minimal_size = size;
    canvas_max = 
        ImVec2(canvas_min.x + canvas_size.x, canvas_min.y + canvas_size.y);
}

void Canvas::show_background(bool flag)
{
    flag_show_background = flag;
}

void Canvas::set_default()
{
    shape_type = Shape::kDefault;
}

void Canvas::set_line()
{
    shape_type = Shape::kLine;
}

void Canvas::set_rect()
{
    shape_type = Shape::kRect;
}

void Canvas::clear_shape_list()
{
    shape_list.clear();
}

void Canvas::draw_background()
{
    ImDrawList* draw_list = ImGui::GetWindowDrawList();
    if (flag_show_background)
    {
        // Draw background recrangle
        draw_list->AddRectFilled(canvas_min, canvas_max, background_color);
        // Draw background border
        draw_list->AddRect(canvas_min, canvas_max, border_color);
    }
    // Add an invisible button for interactions
    // The invisible button should capture the left mouse button actions
    ImGui::SetCursorScreenPos(canvas_min);
    ImGui::InvisibleButton(
        label.c_str(), canvas_size, ImGuiButtonFlags_MouseButtonLeft);
    // Record the current status of the invisible button
    is_hovered = ImGui::IsItemHovered();
    is_active = ImGui::IsItemActive();
}

void Canvas::draw_shapes()
{
    Shape::Settings s = { .bias = canvas_min };
    ImDrawList* draw_list = ImGui::GetWindowDrawList();

    // ClipRect can hide the drawing content outside of the rectangular area
    draw_list->PushClipRect(canvas_min, canvas_max, true);
    for (const auto& shape : shape_list)
    {
        shape->draw(s);
    }
    if (p_shape)
    {
        p_shape->draw(s);
    }
    draw_list->PopClipRect();
}

void Canvas::mouse_click_event()
{
    switch (shape_type)
    {
        case Shape::kDefault:
        {
            break;
        }
        case Shape::kLine:
        {
            // Similar as p_shape = new Line()
            p_shape = std::make_shared<Line>();
            break;
        }
        case Shape::kRect:
        {
            p_shape = std::make_shared<Rect>();
            break;
        }
        default: break;
    }
    if (p_shape)
    {
        draw_status = true;
        start_point = end_point = mouse_pos_in_canvas();
        p_shape->set_start(start_point);
        p_shape->set_end(end_point);
    }
}

void Canvas::mouse_drag_event()
{
    if (p_shape)
    {
        end_point = mouse_pos_in_canvas();
        p_shape->set_end(end_point);
    }
}

void Canvas::mouse_release_event()
{
    draw_status = false;
    if (p_shape)
    {
        shape_list.push_back(p_shape);
        p_shape.reset();
    }
}

const ImVec2 Canvas::mouse_pos_in_canvas()
{
    ImGuiIO& io = ImGui::GetIO();
    const ImVec2 mouse_pos_in_canvas(
        io.MousePos.x - canvas_min.x, io.MousePos.y - canvas_min.y);
    return mouse_pos_in_canvas;
}
}  // namespace USTC_CG