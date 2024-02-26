#pragma once

#include <string>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

namespace USTC_CG
{
class Window
{
   public:
    Window(const std::string& window_name);
    virtual ~Window();

    bool init();
    void run();

   protected:
    virtual void draw();


   private:
    void init_glfw();
    void init_gui();
    void exit_gui();
    void render();

    std::string name = "";
    GLFWwindow *window = nullptr;
    int width = 1280, height = 720;
};
}  // namespace USTC_CG