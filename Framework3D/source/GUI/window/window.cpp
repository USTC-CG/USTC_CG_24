
// #include "glad/glad.h"

#include "GUI/window/window.h"

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include <format>
#include <stdexcept>

#include "imgui_internal.h"

namespace USTC_CG {
Window::Window(const std::string& window_name) : name_(window_name)
{
    if (!init_glfw()) {
        // Initialize GLFW and check for failure
        throw std::runtime_error("Failed to initialize GLFW!");
    }

    window_ = glfwCreateWindow(width_, height_, name_.c_str(), nullptr, nullptr);
    if (window_ == nullptr) {
        glfwTerminate();  // Ensure GLFW is cleaned up before throwing
        throw std::runtime_error("Failed to create GLFW window!");
    }

    if (!init_gui()) {
        // Initialize the GUI and check for failure
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

    while (!glfwWindowShouldClose(window_)) {
        if (!glfwGetWindowAttrib(window_, GLFW_VISIBLE) ||
            glfwGetWindowAttrib(window_, GLFW_ICONIFIED))
            glfwWaitEvents();
        else {
            glfwPollEvents();
            render();
        }
    }
}

void Window::BuildUI()
{
    ImGui::ShowDemoWindow();
}

bool Window::init_glfw()
{
    glfwSetErrorCallback(
        [](int error, const char* desc) { fprintf(stderr, "GLFW Error %d: %s\n", error, desc); });

    if (!glfwInit()) {
        return false;
    }

#ifdef __APPLE__
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    return true;
}

bool Window::init_gui()
{
    glfwMakeContextCurrent(window_);
    glfwSwapInterval(1);  // Enable vsync

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    // io.ConfigFlags |=
    //     ImGuiConfigFlags_ViewportsEnable; // Enable Multi-Viewport / Platform
    //  - fontsize
    float xscale, yscale;
    glfwGetWindowContentScale(window_, &xscale, &yscale);
    // - style
    ImGui::StyleColorsDark();

    io.DisplayFramebufferScale.x = xscale;
    io.DisplayFramebufferScale.x = yscale;

    ImGui_ImplGlfw_InitForOpenGL(window_, true);
#ifdef __APPLE__
    ImGui_ImplOpenGL3_Init("#version 410");
#else
    io.FontGlobalScale = xscale;
    ImGui_ImplOpenGL3_Init("#version 130");
#endif
    return true;
}

void Window::render()
{
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    ImGuiWindowFlags window_flags = ImGuiWindowFlags_None;
    const ImGuiViewport* viewport = ImGui::GetMainViewport();

    ImGui::SetNextWindowPos(viewport->WorkPos);
    ImGui::SetNextWindowSize(viewport->WorkSize);
    ImGui::SetNextWindowViewport(viewport->ID);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
    window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse |
                    ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
    window_flags |= ImGuiWindowFlags_NoNavFocus | ImGuiWindowFlags_NoBackground;

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));

    ImGui::Begin(("DockSpace" + std::to_string(0)).c_str(), 0, window_flags);
    ImGui::PopStyleVar(3);
    ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");

    ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_PassthruCentralNode);

    Render();
    BuildUI();

    ImGui::End();

    ImGui::Render();

    glfwGetFramebufferSize(window_, &width_, &height_);
    glViewport(0, 0, width_, height_);
    glClearColor(0.35f, 0.45f, 0.50f, 1.00f);
    glClear(GL_COLOR_BUFFER_BIT);

    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    glfwSwapBuffers(window_);
}
}  // namespace USTC_CG
