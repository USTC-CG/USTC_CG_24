#pragma once

#include "imgui.h"

namespace USTC_CG
{
class Shape
{
   public:
    enum Type
    {
        kDefault = 0,
        kLine = 1,
        kRect = 2,
        kEllipse = 3,
        kPolygon = 4,
    };

    // Settings for ImGUI draw call
    struct Settings
    {
        // Bias to translate the positions in canvas to the screen
        ImVec2 bias = ImVec2(0.f, 0.f);
        ImU32 line_color = IM_COL32(255, 0, 0, 255);
        float line_thickness = 2.0f;
    };

   public:
    Shape();
    virtual ~Shape();

    virtual void draw(const Settings& settings) = 0;
    void set_start(const ImVec2& s);
    void set_end(const ImVec2& e);

   protected:
    ImVec2 start_point, end_point;
};
}  // namespace USTC_CG