#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <memory>
#include <string>
#include <vector>

#include "imgui.h"
#include "view/component.h"

namespace USTC_CG
{
class Image : public Component
{
   public:
    explicit Image(const std::string &label, const std::string &filename);
    ~Image();

    void draw() override;
    // Set the top-left corner of the image
    void set_position(const ImVec2 &pos);

    const ImVec2 get_image_size();

   private:
    void draw_image();
    // Load pixel texture data to opengl
    void load_gltexture();

   protected:
    std::string filename;
    unsigned char *image_data = nullptr;
    GLuint tex_id;

    ImVec2 position = ImVec2(0.f, 0.f);
    int image_width, image_height;
};
}  // namespace USTC_CG