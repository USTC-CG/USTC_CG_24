#pragma once

#include <glad/glad.h>  // Include GLAD before GLFW.
#include <GLFW/glfw3.h>

#include <memory>
#include <string>
#include <vector>

#include "imgui.h"
#include "view/image.h"
#include "view/component.h"

namespace USTC_CG
{

// Represents an image component that can be rendered within a GUI.
class ImageEditor : public Component
{
   public:
    // Constructs an Image component with a given label and image file.
    explicit ImageEditor(const std::string& label, const std::string& filename);
    virtual ~ImageEditor();  // Destructor to manage resources.

    // Renders the image component.
    void draw() override;

    // Sets the top-left corner position of the image in the GUI.
    void set_position(const ImVec2& pos);

    // Retrieves the size (width, height) of the loaded image.
    ImVec2 get_image_size() const;

    void update();

    void save_to_disk(const std::string& filename);

   private:
    // Draws the loaded image.
    void draw_image();

    // Loads the image file into OpenGL texture memory.
    void load_gltexture();

   protected:
    std::string filename_;                 // Path to the image file.
    std::shared_ptr<Image> data_;          // Raw pixel data of the image.
    GLuint tex_id_ = 0;                    // OpenGL texture identifier.

    ImVec2 position_ = ImVec2(0.0f, 0.0f);  // Position of the image in the GUI.
    int image_width_ = 0, image_height_ = 0;  // Dimensions of the loaded image.
};

}  // namespace USTC_CG
