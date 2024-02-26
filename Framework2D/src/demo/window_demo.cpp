#include "window_demo.h"

#include <ImGuiFileDialog.h>

namespace USTC_CG
{
Demo::Demo(const std::string& window_name) : Window(window_name)
{
    p_canvas = std::make_shared<Canvas>("canvas");
    p_canvas->set_line();
}
Demo::~Demo()
{
}
void Demo::draw()
{
    draw_toolbar();
    if (flag_open_file_dialog)
        draw_open_image_file_dialog();
    // Fill the whole window
    const ImGuiViewport* viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(viewport->WorkPos);
    ImGui::SetNextWindowSize(viewport->WorkSize);
    if (ImGui::Begin(
            "Demo", &flag_show_main_view, ImGuiWindowFlags_NoDecoration))
    {
        if (p_image)
            draw_image();
        if (flag_enable_canvas)
            draw_canvas();
        ImGui::End();
    }
}
void Demo::draw_toolbar()
{
    if (ImGui::BeginMainMenuBar())
    {
        if (ImGui::BeginMenu("File"))
        {
            if (ImGui::MenuItem("Open Image File.."))
            {
                flag_open_file_dialog = true;
            }
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Edit"))
        {
            ImGui::Checkbox("Enable Canvas", &flag_enable_canvas);
            ImGui::EndMenu();
        }

        ImGui::EndMainMenuBar();
    }
}
void Demo::draw_image()
{
    const auto& canvas_min = ImGui::GetCursorScreenPos();
    const auto& canvas_size = ImGui::GetContentRegionAvail();
    const auto& image_size = p_image->get_image_size();
    // Center the image in the window
    ImVec2 pos = ImVec2(
        canvas_min.x + canvas_size.x / 2 - image_size.x / 2,
        canvas_min.y + canvas_size.y / 2 - image_size.y / 2);
    p_image->set_position(pos);
    p_image->draw();
}
void Demo::draw_open_image_file_dialog()
{
    IGFD::FileDialogConfig config; config.path = ".";
    ImGuiFileDialog::Instance()->OpenDialog(
        "ChooseImageOpenFileDlg", "Choose Image File", ".png,.jpg", config);
    if (ImGuiFileDialog::Instance()->Display("ChooseImageOpenFileDlg"))
    {
        if (ImGuiFileDialog::Instance()->IsOk())
        {
            std::string filePathName =
                ImGuiFileDialog::Instance()->GetFilePathName();
            std::string label = filePathName;
            p_image = std::make_shared<Image>(label, filePathName);
            p_canvas->clear_shape_list();
        }
        ImGuiFileDialog::Instance()->Close();
        flag_open_file_dialog = false;
    }
}
void Demo::draw_canvas()
{
    const auto& canvas_min = ImGui::GetCursorScreenPos();
    const auto& canvas_size = ImGui::GetContentRegionAvail();

    if (p_image)
    {
        // Resize the canvas to fit the image
        const auto& image_size = p_image->get_image_size();
        ImVec2 pos = ImVec2(
            canvas_min.x + canvas_size.x / 2 - image_size.x / 2,
            canvas_min.y + canvas_size.y / 2 - image_size.y / 2);
        p_canvas->set_attributes(pos, image_size);
        p_canvas->show_background(false);
    }
    else
    {
        // Fill the window
        const auto& canvas_size = ImGui::GetContentRegionAvail();
        p_canvas->set_attributes(canvas_min, canvas_size);
        p_canvas->show_background(true);
    }

    p_canvas->draw();
}
}  // namespace USTC_CG