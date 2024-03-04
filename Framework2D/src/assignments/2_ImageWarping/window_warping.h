#pragma once

#include <memory>

#include "view/window.h"
#include "comp_warping.h"

namespace USTC_CG
{
class ImageWarping : public Window
{
   public:
    explicit ImageWarping(const std::string& window_name);
    ~ImageWarping();

    void draw();

   private:
    void draw_toolbar();
    void draw_image();
    void draw_open_image_file_dialog();
    void draw_save_image_file_dialog();

    std::shared_ptr<CompWarping> p_image_ = nullptr;

    bool flag_show_main_view_ = true;
    bool flag_open_file_dialog_ = false;
    bool flag_save_file_dialog_ = false;
};
}  // namespace USTC_CG