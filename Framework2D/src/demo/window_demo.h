#pragma once

#include <memory>

#include "view/comp_canvas.h"
#include "view/comp_image.h"
#include "view/window.h"

namespace USTC_CG
{
class Demo : public Window
{
   public:
    explicit Demo(const std::string& window_name);
    ~Demo();
    void draw();

    void draw_toolbar();
    void draw_image();
    void draw_open_image_file_dialog();
    void draw_canvas();

   private:
    std::shared_ptr<Canvas> p_canvas_ = nullptr;
    std::shared_ptr<Image> p_image_ = nullptr;

    bool flag_show_main_view_ = true;
    bool flag_open_file_dialog_ = false;
    bool flag_enable_canvas_ = false;
};
}  // namespace USTC_CG