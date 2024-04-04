#pragma once

#include <GLFW/glfw3.h>

#include <string>

#include "USTC_CG.h"
#include "Utils/Logging/Logging.h"

namespace pxr {
    class Hgi;
}

namespace USTC_CG {
// Represents a window in a GUI application, providing basic functionalities
// such as initialization and rendering.
class USTC_CG_API Window {
public:
    // Constructor that sets the window's title.
    explicit Window(const std::string& window_name);

    virtual ~Window();

    // Initializes the window and its dependencies (GLFW, GLAD, ImGui, etc.).
    bool init();

    // Enters the main rendering loop.
    void run();

protected:
    // Virtual draw function to be implemented by derived classes for custom
    // rendering.
    virtual void BuildUI();

    virtual void Render()
    {
        USTC_CG::logging("Empty render function for window.", Info);
    }


protected:
    // Initializes GLFW library.
    bool init_glfw();

    // Initializes the GUI library (e.g., Dear ImGui).
    bool init_gui();

    // Handles the rendering of each frame.
    void render();

    std::string name_;             // Name (title) of the window.
    GLFWwindow* window_ = nullptr; // Pointer to the GLFW window.
    int width_ = 1920;             // Width of the window.
    int height_ = 1080;            // Height of the window.

    pxr::Hgi* hgi_;
private:
};
} // namespace USTC_CG
