#include "view/window.h"

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include <format>

namespace USTC_CG
{
Window::Window(const std::string& window_name) : name(window_name)
{
    init_glfw();
    window = glfwCreateWindow(width, height, name.c_str(), nullptr, nullptr);
    if (window == nullptr)
        throw std::runtime_error("Failed to create window!");
    init_gui();
}

Window::~Window()
{
    ImGui_ImplGlfw_Shutdown();
    exit_gui();

    glfwDestroyWindow(window);
    glfwTerminate();
}

bool Window::init()
{
    return true;
}

void Window::run()
{
    glfwShowWindow(window);

    while (!glfwWindowShouldClose(window))
    {
        if (!glfwGetWindowAttrib(window, GLFW_VISIBLE) ||
            glfwGetWindowAttrib(window, GLFW_ICONIFIED))
            glfwWaitEvents();
        else
            glfwPollEvents();
        render();
    }
}

void Window::draw()
{
    static bool show_demo_window = true;
    ImGui::ShowDemoWindow(&show_demo_window);
}

void Window::init_glfw()
{
    glfwSetErrorCallback(
        [](int error, const char* desc)
        { fprintf(stderr, "GLFW Error %d: %s\n", error, desc); });
    if (!glfwInit())
        throw std::runtime_error("Failed to initialize GLFW!");
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
}

void Window::init_gui()
{
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
        throw std::runtime_error("Failed to load GL!");

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    ImGuiIO& io = ImGui::GetIO();
    // - fontsize
    float xscale, yscale;
    glfwGetWindowContentScale(window, &xscale, &yscale);
    io.FontGlobalScale = xscale;
    // - style
    ImGui::StyleColorsDark();

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 130");
}

void Window::exit_gui()
{
    glfwMakeContextCurrent(window);

    ImGui_ImplOpenGL3_Shutdown();

    ImGui::DestroyContext();
}

void Window::render()
{
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
    draw();
    ImGui::Render();

    glfwGetFramebufferSize(window, &width, &height);
    glViewport(0, 0, width, height);
    glClearColor(0, 0, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT);
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    glfwSwapBuffers(window);
}

}  // namespace USTC_CG