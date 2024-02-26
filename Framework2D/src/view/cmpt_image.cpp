#include "view/cmpt_image.h"
#define STB_IMAGE_IMPLEMENTATION
#include "iostream"
#include "stb_image.h"

namespace USTC_CG
{
Image::Image(const std::string& label, const std::string& filename)
    : filename(filename),
      Component(label)
{
    glGenTextures(1, &tex_id);
    image_data =
        stbi_load(filename.c_str(), &image_width, &image_height, NULL, 4);
    if (image_data == nullptr)
        std::cout << "Failed to load image from file " << filename << std::endl;
    else
        load_gltexture();
}

Image::~Image()
{
    if (image_data)
      stbi_image_free(image_data);
    glDeleteTextures(1, &tex_id);
}

void Image::draw()
{
    draw_image();
}

void Image::set_position(const ImVec2& pos)
{
    position = pos;
}

const ImVec2 Image::get_image_size()
{
    return ImVec2((float)image_width, (float)image_height);
}

void Image::load_gltexture()
{
    glBindTexture(GL_TEXTURE_2D, tex_id);

    // Setup filtering parameters for display
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Upload pixels into texture
    glTexImage2D(
        GL_TEXTURE_2D,
        0,
        GL_RGBA,
        image_width,
        image_height,
        0,
        GL_RGBA,
        GL_UNSIGNED_BYTE,
        image_data);
}
void Image::draw_image()
{
    auto draw_list = ImGui::GetWindowDrawList();
    if (image_data)
    {
      ImVec2 p_min = position;
      ImVec2 p_max = ImVec2(p_min.x + image_width, p_min.y + image_height);
      draw_list->AddImage((void*)(intptr_t)tex_id, p_min, p_max);
    }
}
}  // namespace USTC_CG