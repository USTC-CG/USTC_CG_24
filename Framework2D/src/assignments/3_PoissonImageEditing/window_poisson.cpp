#include "window_poisson.h"

#include <ImGuiFileDialog.h>

#include <iostream>

namespace USTC_CG
{
WindowPoisson::WindowPoisson(const std::string& window_name)
    : Window(window_name)
{
}

WindowPoisson::~WindowPoisson()
{
}

void WindowPoisson::draw()
{
    draw_toolbar();
    if (flag_open_target_file_dialog_)
        draw_open_target_image_file_dialog();
    if (flag_open_source_file_dialog_ && p_target_)
        draw_open_source_image_file_dialog();
    if (flag_save_file_dialog_ && p_target_)
        draw_save_image_file_dialog();

    if (p_target_)
        draw_target();
    if (p_source_)
        draw_source();
}

void WindowPoisson::draw_toolbar()
{
    if (ImGui::BeginMainMenuBar())
    {
        if (ImGui::BeginMenu("File"))
        {
            if (ImGui::MenuItem("Open Target.."))
            {
                flag_open_target_file_dialog_ = true;
            }
            add_tooltips("Open the target image file.");
            if (ImGui::MenuItem("Open Source..") && p_target_)
            {
                flag_open_source_file_dialog_ = true;
            }
            add_tooltips(
                "Open the source image file. This is available only when "
                "the target image is loaded.");
            if (ImGui::MenuItem("Save As..") && p_target_)
            {
                flag_save_file_dialog_ = true;
            }
            ImGui::EndMenu();
        }

        ImGui::Separator();

        if (ImGui::MenuItem("Restore") && p_target_)
        {
            p_target_->restore();
        }
        add_tooltips("Replace the target image with back up data.");

        ImGui::Separator();

        static bool selectable = false;
        ImGui::Checkbox("Select", &selectable);
        add_tooltips(
            "On: Enable region selection in the source image. Drag left mouse "
            "to select rectangle (default) in the source.");
        if (p_source_)
            p_source_->enable_selecting(selectable);
        static bool realtime = false;
        ImGui::Checkbox("Realtime", &realtime);
        add_tooltips(
            "On: Enable realtime cloning in the target image, which means that "
            "you can drag the mouse and the cloning would update along the "
            "mouse.");
        if (p_target_)
            p_target_->set_realtime(realtime);

        ImGui::Separator();

        if (ImGui::MenuItem("Paste") && p_target_ && p_source_)
        {
            p_target_->set_paste();
        }
        add_tooltips(
            "Press this button and then click in the target image, to "
            "clone the selected region to the target image.");
        // HW3_TODO: You may add more items in the menu for the different types
        // of Poisson editing.

        ImGui::EndMainMenuBar();
    }
}

void WindowPoisson::draw_target()
{
    const auto& image_size = p_target_->get_image_size();
    ImGui::SetNextWindowSize(ImVec2(image_size.x + 60, image_size.y + 60));
    if (ImGui::Begin("Target Image", &flag_show_target_view_))
    {
        // Place the image in the center of the window
        const auto& min = ImGui::GetCursorScreenPos();
        const auto& size = ImGui::GetContentRegionAvail();
        ImVec2 pos = ImVec2(
            min.x + size.x / 2 - image_size.x / 2,
            min.y + size.y / 2 - image_size.y / 2);
        p_target_->set_position(pos);
        p_target_->draw();
    }
    ImGui::End();
}

void WindowPoisson::draw_source()
{
    const auto& image_size = p_source_->get_image_size();
    ImGui::SetNextWindowSize(ImVec2(image_size.x + 60, image_size.y + 60));
    if (ImGui::Begin("Source Image", &flag_show_source_view_))
    {
        // Place the image in the center of the window
        const auto& min = ImGui::GetCursorScreenPos();
        const auto& size = ImGui::GetContentRegionAvail();
        ImVec2 pos = ImVec2(
            min.x + size.x / 2 - image_size.x / 2,
            min.y + size.y / 2 - image_size.y / 2);
        p_source_->set_position(pos);
        p_source_->draw();
    }
    ImGui::End();
}

void WindowPoisson::draw_open_target_image_file_dialog()
{
    IGFD::FileDialogConfig config;
    config.path = DATA_PATH;
    config.flags = ImGuiFileDialogFlags_Modal;
    ImGuiFileDialog::Instance()->OpenDialog(
        "ChooseTargetOpenFileDlg", "Choose Image File", ".jpg,.png", config);
    ImVec2 main_size = ImGui::GetMainViewport()->WorkSize;
    ImVec2 dlg_size(main_size.x / 2, main_size.y / 2);
    if (ImGuiFileDialog::Instance()->Display(
            "ChooseTargetOpenFileDlg", ImGuiWindowFlags_NoCollapse, dlg_size))
    {
        if (ImGuiFileDialog::Instance()->IsOk())
        {
            std::string filePathName =
                ImGuiFileDialog::Instance()->GetFilePathName();
            std::string label = filePathName;
            p_target_ = std::make_shared<CompTargetImage>(label, filePathName);
            if (p_source_)
                p_target_->set_source(p_source_);
        }
        ImGuiFileDialog::Instance()->Close();
        flag_open_target_file_dialog_ = false;
    }
}

void WindowPoisson::draw_open_source_image_file_dialog()
{
    IGFD::FileDialogConfig config;
    config.path = DATA_PATH;
    config.flags = ImGuiFileDialogFlags_Modal;
    ImGuiFileDialog::Instance()->OpenDialog(
        "ChooseSourceOpenFileDlg", "Choose Image File", ".jpg,.png", config);
    ImVec2 main_size = ImGui::GetMainViewport()->WorkSize;
    ImVec2 dlg_size(main_size.x / 2, main_size.y / 2);
    if (ImGuiFileDialog::Instance()->Display(
            "ChooseSourceOpenFileDlg", ImGuiWindowFlags_NoCollapse, dlg_size))
    {
        if (ImGuiFileDialog::Instance()->IsOk())
        {
            std::string filePathName =
                ImGuiFileDialog::Instance()->GetFilePathName();
            std::string label = filePathName;
            p_source_ = std::make_shared<CompSourceImage>(label, filePathName);
            // Bind the source image to the target
            if (p_source_)
                p_target_->set_source(p_source_);
        }
        ImGuiFileDialog::Instance()->Close();
        flag_open_source_file_dialog_ = false;
    }
}

void WindowPoisson::draw_save_image_file_dialog()
{
    IGFD::FileDialogConfig config;
    config.path = DATA_PATH;
    config.flags = ImGuiFileDialogFlags_Modal;
    ImGuiFileDialog::Instance()->OpenDialog(
        "ChooseImageSaveFileDlg", "Save Image As...", ".jpg", config);
    ImVec2 main_size = ImGui::GetMainViewport()->WorkSize;
    ImVec2 dlg_size(main_size.x / 2, main_size.y / 2);
    if (ImGuiFileDialog::Instance()->Display(
            "ChooseImageSaveFileDlg", ImGuiWindowFlags_NoCollapse, dlg_size))
    {
        if (ImGuiFileDialog::Instance()->IsOk())
        {
            std::string filePathName =
                ImGuiFileDialog::Instance()->GetFilePathName();
            std::string label = filePathName;
            if (p_target_)
                p_target_->save_to_disk(filePathName);
        }
        ImGuiFileDialog::Instance()->Close();
        flag_save_file_dialog_ = false;
    }
}
void WindowPoisson::add_tooltips(std::string desc)
{
    if (ImGui::BeginItemTooltip())
    {
        ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
        ImGui::TextUnformatted(desc.c_str());
        ImGui::PopTextWrapPos();
        ImGui::EndTooltip();
    }
}
}  // namespace USTC_CG