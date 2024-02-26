#include "window_minidraw.h"

#include <iostream>

namespace USTC_CG
{
MiniDraw::MiniDraw(const std::string& window_name) : Window(window_name)
{
    p_canvas = std::make_shared<Canvas>("Cmpt.Canvas");
}

MiniDraw::~MiniDraw()
{
}

void MiniDraw::draw()
{
    draw_canvas();
}

void MiniDraw::draw_canvas()
{
    // Set a full screen canvas view
    const ImGuiViewport* viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(viewport->WorkPos);
    ImGui::SetNextWindowSize(viewport->WorkSize);
    if (ImGui::Begin("Canvas", &flag_show_canvas_view, ImGuiWindowFlags_NoDecoration))
    {
        // Buttons for shape types
        if (ImGui::Button("Line"))
        {
            std::cout << "Set shape to Line" << std::endl;
            p_canvas->set_line();
        }
        ImGui::SameLine();
        if (ImGui::Button("Rect"))
        {
            std::cout << "Set shape to Rect" << std::endl;
            p_canvas->set_rect();
        }
        // HW1_TODO: More primitives
        
        // Canvas component
        ImGui::Text("Drag left mouse to add shapes.");
        // Set the canvas to fill the rest of the window
        const auto& canvas_min = ImGui::GetCursorScreenPos();
        const auto& canvas_size = ImGui::GetContentRegionAvail();
        p_canvas->set_attributes(canvas_min, canvas_size);
        p_canvas->draw();
    }
    ImGui::End();
}
}  // namespace USTC_CG