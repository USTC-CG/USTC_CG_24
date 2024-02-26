#pragma once

#include <memory>
#include <vector>

#include "shapes/shape.h"
#include "view/component.h"

namespace USTC_CG
{
class Canvas : public Component
{
   public:
    // Use the constructor of the parent class
    using Component::Component;

    void draw() override;

    // Set shape types
    void set_default();
    void set_line();
    void set_rect();

    void clear_shape_list();

    void set_attributes(const ImVec2& min, const ImVec2& size);
    void show_background(bool flag);

   private:
    // Draw functions
    void draw_background();
    void draw_shapes();

    // Mouse interactions
    void mouse_click_event();
    void mouse_drag_event();
    void mouse_release_event();

   private:
    // The mouse's relative position in the canvas
    const ImVec2 mouse_pos_in_canvas();

    // Canvas attributes
    ImVec2 canvas_min;   // Top-left corner of canvas rectangle
    ImVec2 canvas_max;   // Bottom-right corner of canvas rectangle
    ImVec2 canvas_size;  // Size of canvas rectangle
    bool draw_status = false;

    ImVec2 canvas_minimal_size = ImVec2(50.f, 50.f);
    ImU32 background_color = IM_COL32(50, 50, 50, 255);
    ImU32 border_color = IM_COL32(255, 255, 255, 255);
    bool flag_show_background = true;

    // Mouse status
    bool is_hovered, is_active;
    // Current shape
    Shape::Type shape_type;
    ImVec2 start_point, end_point;
    std::shared_ptr<Shape> p_shape;
    // Previous shapes
    std::vector<std::shared_ptr<Shape>> shape_list;
};
}  // namespace USTC_CG