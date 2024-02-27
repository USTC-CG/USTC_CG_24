#include "view/comp_image.h"
#define STB_IMAGE_IMPLEMENTATION
#include "iostream"
#include "stb_image.h"

namespace USTC_CG
{
Image::Image(const std::string& label, const std::string& filename)
    : filename_(filename),
      Component(label)
{
    glGenTextures(1, &tex_id_);
    image_data_ =
        stbi_load(filename.c_str(), &image_width_, &image_height_, NULL, 4);
    if (image_data_ == nullptr)
        std::cout << "Failed to load image from file " << filename << std::endl;
    else
        load_gltexture();
}

Image::~Image()
{
    if (image_data_)
        stbi_image_free(image_data_);
    glDeleteTextures(1, &tex_id_);
}

void Image::draw()
{
    draw_image();
}

void Image::set_position(const ImVec2& pos)
{
    position_ = pos;
}

ImVec2 Image::get_image_size() const
{
    return ImVec2((float)image_width_, (float)image_height_);
}

void Image::load_gltexture()
{
    glBindTexture(GL_TEXTURE_2D, tex_id_);

    // Setup filtering parameters for display
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Upload pixels into texture
    glTexImage2D(
        GL_TEXTURE_2D,
        0,
        GL_RGBA,
        image_width_,
        image_height_,
        0,
        GL_RGBA,
        GL_UNSIGNED_BYTE,
        image_data_);
}

void Image::draw_image()
{
    auto draw_list = ImGui::GetWindowDrawList();
    if (image_data_)
    {
        ImVec2 p_min = position_;
        ImVec2 p_max = ImVec2(p_min.x + image_width_, p_min.y + image_height_);
        draw_list->AddImage((void*)(intptr_t)tex_id_, p_min, p_max);
    }
}
}  // namespace USTC_CG