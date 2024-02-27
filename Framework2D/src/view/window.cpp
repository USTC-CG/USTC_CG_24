#include "view/window.h"

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include <format>

namespace USTC_CG
{
Window::Window(const std::string& window_name) : name_(window_name)
{
    if (!init_glfw())
    {  // Initialize GLFW and check for failure
        throw std::runtime_error("Failed to initialize GLFW!");
    }

    window_ =
        glfwCreateWindow(width_, height_, name_.c_str(), nullptr, nullptr);
    if (window_ == nullptr)
    {
        glfwTerminate();  // Ensure GLFW is cleaned up before throwing
        throw std::runtime_error("Failed to create GLFW window!");
    }

    if (!init_gui())
    {  // Initialize the GUI and check for failure
        glfwDestroyWindow(window_);
        glfwTerminate();
        throw std::runtime_error("Failed to initialize GUI!");
    }
}

Window::~Window()
{
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window_);
    glfwTerminate();
}

bool Window::init()
{
    // Placeholder for additional initialization if needed.
    return true;
}

void Window::run()
{
    glfwShowWindow(window_);

    while (!glfwWindowShouldClose(window_))
    {
        if (!glfwGetWindowAttrib(window_, GLFW_VISIBLE) ||
            glfwGetWindowAttrib(window_, GLFW_ICONIFIED))
            glfwWaitEvents();
        else
        {
            glfwPollEvents();
            render();
        }
    }
}

void Window::draw()
{
    // Placeholder for custom draw logic, should be overridden in derived
    // classes.
    ImGui::ShowDemoWindow();
}

bool Window::init_glfw()
{
    glfwSetErrorCallback(
        [](int error, const char* desc)
        { fprintf(stderr, "GLFW Error %d: %s\n", error, desc); });

    if (!glfwInit())
    {
        return false;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    return true;
}

bool Window::init_gui()
{
    glfwMakeContextCurrent(window_);
    glfwSwapInterval(1);  // Enable vsync

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        return false;
    }

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    ImGuiIO& io = ImGui::GetIO();
    (void)io; 
    // - fontsize
    float xscale, yscale;
    glfwGetWindowContentScale(window_, &xscale, &yscale);
    io.FontGlobalScale = xscale;
    // - style
    ImGui::StyleColorsDark();

    ImGui_ImplGlfw_InitForOpenGL(window_, true);
    ImGui_ImplOpenGL3_Init("#version 130");

    return true;
}

void Window::render()
{
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    draw();

    ImGui::Render();

    glfwGetFramebufferSize(window_, &width_, &height_);
    glViewport(0, 0, width_, height_);
    glClearColor(0.35f, 0.45f, 0.50f, 1.00f);
    glClear(GL_COLOR_BUFFER_BIT);

    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    glfwSwapBuffers(window_);
}

}  // namespace USTC_CG