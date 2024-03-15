#include "comp_source_image.h"

#include <algorithm>
#include <cmath>

namespace USTC_CG
{
using uchar = unsigned char;

CompSourceImage::CompSourceImage(
    const std::string& label,
    const std::string& filename)
    : ImageEditor(label, filename)
{
    if (data_)
        selected_region_ =
            std::make_shared<Image>(data_->width(), data_->height(), 1);
}

void CompSourceImage::draw()
{
    // Draw the image
    ImageEditor::draw();
    // Draw selected region
    if (flag_enable_selecting_region_)
        select_region();
}

void CompSourceImage::enable_selecting(bool flag)
{
    flag_enable_selecting_region_ = flag;
}

void CompSourceImage::select_region()
{
    /// Invisible button over the canvas to capture mouse interactions.
    ImGui::SetCursorScreenPos(position_);
    ImGui::InvisibleButton(
        label_.c_str(),
        ImVec2(
            static_cast<float>(image_width_),
            static_cast<float>(image_height_)),
        ImGuiButtonFlags_MouseButtonLeft);
    // Record the current status of the invisible button
    bool is_hovered_ = ImGui::IsItemHovered();
    // HW3_TODO(optional): You can add more shapes for region selection. You can
    // also consider using the implementation in HW1. (We use rectangle for
    // example)
    ImGuiIO& io = ImGui::GetIO();
    if (is_hovered_ && ImGui::IsMouseClicked(ImGuiMouseButton_Left))
    {
        draw_status_ = true;
        start_ = end_ = ImVec2(
            std::clamp<float>(
                io.MousePos.x - position_.x, 0, (float)image_width_),
            std::clamp<float>(
                io.MousePos.y - position_.y, 0, (float)image_height_));
    }
    if (draw_status_)
    {
        end_ = ImVec2(
            std::clamp<float>(
                io.MousePos.x - position_.x, 0, (float)image_width_),
            std::clamp<float>(
                io.MousePos.y - position_.y, 0, (float)image_height_));
        if (!ImGui::IsMouseDown(ImGuiMouseButton_Left))
        {
            draw_status_ = false;
            // Update the selected region.
            // HW3_TODO(optional): For other types of closed shapes, the most
            // important part in region selection is to find the interior pixels
            // of the region.
            // We give an example of rectangle here.
            //
            // For polygon or freehand regions, you should inplement the
            // "scanning line" algorithm, which is a well-known algorithm in CG.
            for (int i = 0; i < selected_region_->width(); ++i)
                for (int j = 0; j < selected_region_->height(); ++j)
                    selected_region_->set_pixel(i, j, { 0 });
            switch (region_type_)
            {
                case USTC_CG::CompSourceImage::kDefault: break;
                case USTC_CG::CompSourceImage::kRect:
                {
                    for (int i = static_cast<int>(start_.x);
                         i < static_cast<int>(end_.x);
                         ++i)
                    {
                        for (int j = static_cast<int>(start_.y);
                             j < static_cast<int>(end_.y);
                             ++j)
                        {
                            selected_region_->set_pixel(i, j, { 255 });
                        }
                    }
                    break;
                }
                default: break;
            }
        }
    }

    // Visualization
    auto draw_list = ImGui::GetWindowDrawList();
    ImVec2 s(start_.x + position_.x, start_.y + position_.y);
    ImVec2 e(end_.x + position_.x, end_.y + position_.y);

    switch (region_type_)
    {
        case USTC_CG::CompSourceImage::kDefault: break;
        case USTC_CG::CompSourceImage::kRect:
        {
            if (e.x > s.x && e.y > s.y)
                draw_list->AddRect(s, e, IM_COL32(255, 0, 0, 255), 2.0f);
            break;
        }
        default: break;
    }
}
std::shared_ptr<Image> CompSourceImage::get_region()
{
    return selected_region_;
}
std::shared_ptr<Image> CompSourceImage::get_data()
{
    return data_;
}
ImVec2 CompSourceImage::get_position() const
{
    return start_;
}
}  // namespace USTC_CG