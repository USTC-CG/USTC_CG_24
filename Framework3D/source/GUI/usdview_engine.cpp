// My resharper is not working well with the _MSC_VER_ macro.
#include "GUI/usdview_engine.h"

#include "Nodes/GlobalUsdStage.h"
#include "free_camera.h"
#include "imgui.h"
#include "pxr/base/gf/camera.h"
#include "pxr/imaging/glf/drawTarget.h"
#include "pxr/pxr.h"
#include "pxr/usd/usd/primRange.h"
#include "pxr/usd/usd/stage.h"
#include "pxr/usdImaging/usdImagingGL/engine.h"

USTC_CG_NAMESPACE_OPEN_SCOPE
class NodeTree;
using namespace pxr;

class UsdviewEngineImpl {
   public:
    enum class CamType { First, Third };
    struct Status {
        CamType cam_type = CamType::First;  // 0 for 1st personal, 1 for 3rd personal
        unsigned renderer_id = 0;
    } engine_status;

    float timecode = 0;
    float frame_per_second;
    const float time_code_max = 250;
    bool playing = false;

    UsdviewEngineImpl(pxr::UsdStageRefPtr stage)
    {
        GarchGLApiLoad();
        glGenFramebuffers(1, &fbo);

        renderer_ = std::make_unique<UsdImagingGLEngine>();
        renderer_->SetEnablePresentation(true);
        free_camera_ = std::make_unique<FirstPersonCamera>();

        auto plugins = renderer_->GetRendererPlugins();
        renderer_->SetRendererPlugin(plugins[engine_status.renderer_id]);
        free_camera_->SetProjection(GfCamera::Projection::Perspective);
        free_camera_->SetClippingRange(pxr::GfRange1f{ 0.1f, 1000.f });
    }

    void DrawMenuBar();
    void OnFrame(float delta_time, NodeTree* node_tree);
    void OnFrame(float delta_time, NodeTree* node_tree, NodeTreeExecutor* executor);
    void refresh_platform_texture();
    void refresh_viewport(int x, int y);
    void OnResize(int x, int y);
    void time_controller(float delta_time);
    void set_current_time_code(float time_code);

   private:
    unsigned fbo = 0;
    unsigned tex = 0;
    std::unique_ptr<FreeCamera> free_camera_;
    bool is_hovered_ = false;
    std::unique_ptr<UsdImagingGLEngine> renderer_;
    UsdImagingGLRenderParams _renderParams;
    GfVec2i renderBufferSize_;
    bool is_active_;
    bool CameraCallback(float delta_time);
};

void UsdviewEngineImpl::DrawMenuBar()
{
    ImGui::BeginMenuBar();
    if (ImGui::BeginMenu("Free Camera")) {
        if (ImGui::BeginMenu("Camera Type")) {
            if (ImGui::MenuItem(
                    "First Personal", 0, this->engine_status.cam_type == CamType::First)) {
                if (engine_status.cam_type != CamType::First) {
                    free_camera_ = std::make_unique<FirstPersonCamera>();
                    engine_status.cam_type = CamType::First;
                }
            }
            if (ImGui::MenuItem(
                    "Third Personal", 0, this->engine_status.cam_type == CamType::Third)) {
                if (engine_status.cam_type != CamType::Third) {
                    free_camera_ = std::make_unique<ThirdPersonCamera>();
                    engine_status.cam_type = CamType::Third;
                }
            }
            ImGui::EndMenu();
        }
        ImGui::EndMenu();
    }
    if (ImGui::BeginMenu("Renderer")) {
        if (ImGui::BeginMenu("Select Renderer")) {
            auto available_renderers = renderer_->GetRendererPlugins();
            for (unsigned i = 0; i < available_renderers.size(); ++i) {
                if (ImGui::MenuItem(
                        available_renderers[i].GetText(),
                        0,
                        this->engine_status.renderer_id == i)) {
                    if (this->engine_status.renderer_id != i) {
                        renderer_->SetRendererPlugin(available_renderers[i]);

                        // Perform a fake resize event
                        refresh_viewport(renderBufferSize_[0], renderBufferSize_[1]);
                        this->engine_status.renderer_id = i;
                    }
                }
            }
            ImGui::EndMenu();
        }
        ImGui::EndMenu();
    }

    ImGui::EndMenuBar();
}

void UsdviewEngineImpl::OnFrame(float delta_time, NodeTree* node_tree, NodeTreeExecutor* executor)
{
    DrawMenuBar();
    // Update the camera when mouse is in the subwindow
    CameraCallback(delta_time);

    auto frustum = free_camera_->GetFrustum();

    GfMatrix4d projectionMatrix = frustum.ComputeProjectionMatrix();
    GfMatrix4d viewMatrix = frustum.ComputeViewMatrix();

    renderer_->SetCameraState(viewMatrix, projectionMatrix);
    renderer_->SetRendererAov(HdAovTokens->color);
    renderer_->SetRendererSetting(TfToken("RenderNodeTree"), VtValue((void*)node_tree));
    renderer_->SetRendererSetting(TfToken("RenderNodeTreeExecutor"), VtValue((void*)executor));

    _renderParams.enableLighting = true;
    _renderParams.enableSceneMaterials = true;
    _renderParams.showRender = true;
    _renderParams.frame = UsdTimeCode::Default();
    _renderParams.drawMode = UsdImagingGLDrawMode::DRAW_WIREFRAME_ON_SURFACE;
    _renderParams.colorCorrectionMode = TfToken("sRGB");

    _renderParams.clearColor = GfVec4f(0.4f, 0.4f, 0.4f, 1.f);
    _renderParams.frame = UsdTimeCode(timecode);

    for (int i = 0; i < free_camera_->GetClippingPlanes().size(); ++i) {
        _renderParams.clipPlanes[i] = free_camera_->GetClippingPlanes()[i];
    }

    GlfSimpleLightVector lights(1);
    auto cam_pos = frustum.GetPosition();
    lights[0].SetPosition(GfVec4f{ float(cam_pos[0]), float(cam_pos[1]), float(cam_pos[2]), 1.0f });
    lights[0].SetAmbient(GfVec4f(0, 0, 0, 0));
    lights[0].SetDiffuse(GfVec4f(1.0f) * 1.9);
    GlfSimpleMaterial material;
    float kA = 0.0f;
    float kS = 0.0f;
    float shiness = 0.f;
    material.SetDiffuse(GfVec4f(kA, kA, kA, 1.0f));
    material.SetSpecular(GfVec4f(kS, kS, kS, 1.0f));
    material.SetShininess(shiness);
    GfVec4f sceneAmbient = { 0.01, 0.01, 0.01, 1.0 };
    renderer_->SetLightingState(lights, material, sceneAmbient);

    UsdPrim root = GlobalUsdStage::global_usd_stage->GetPseudoRoot();
    renderer_->Render(root, _renderParams);

    renderer_->SetPresentationOutput(pxr::TfToken("OpenGL"), pxr::VtValue(fbo));
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    auto imgui_frame_size = ImVec2(renderBufferSize_[0], renderBufferSize_[1]);

    ImGui::BeginChild("ViewPort", imgui_frame_size, 0, ImGuiWindowFlags_NoMove);
    ImGui::Image(ImTextureID(tex), imgui_frame_size, ImVec2(0.0f, 1.0f), ImVec2(1.0f, 0.0f));
    is_active_ = ImGui::IsWindowFocused();
    is_hovered_ = ImGui::IsItemHovered();
    ImGui::EndChild();
}

void UsdviewEngineImpl::refresh_platform_texture()
{
    if (tex) {
        glDeleteTextures(1, &tex);
    }
    glGenTextures(1, &tex);

    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    glBindTexture(GL_TEXTURE_2D, tex);
    glTexImage2D(
        GL_TEXTURE_2D,
        0,
        GL_RGBA8,
        renderBufferSize_[0],
        renderBufferSize_[1],
        0,
        GL_RGBA,
        GL_UNSIGNED_BYTE,
        NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, tex, 0);
    glBindTexture(GL_TEXTURE_2D, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void UsdviewEngineImpl::refresh_viewport(int x, int y)
{
    renderBufferSize_[0] = x;
    renderBufferSize_[1] = y;

    renderer_->SetRenderBufferSize(renderBufferSize_);
    renderer_->SetRenderViewport(
        GfVec4d{ 0.0, 0.0, double(renderBufferSize_[0]), double(renderBufferSize_[1]) });
    free_camera_->m_ViewportSize = renderBufferSize_;

    refresh_platform_texture();
}

void UsdviewEngineImpl::OnResize(int x, int y)
{
    if (renderBufferSize_[0] != x || renderBufferSize_[1] != y) {
        refresh_viewport(x, y);
    }
}

void UsdviewEngineImpl::time_controller(float delta_time)
{
    if (is_active_ && ImGui::IsKeyPressed(ImGuiKey_Space)) {
        playing = !playing;
    }
    if (playing) {
        timecode += delta_time * GlobalUsdStage::timeCodesPerSecond;
        if (timecode > time_code_max) {
            timecode = 0;
        }
    }

    ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
    if (ImGui::SliderFloat("Time##timecode", &timecode, 0, time_code_max)) {
    }
}

void UsdviewEngineImpl::set_current_time_code(float time_code)
{
    timecode = time_code;
}

bool UsdviewEngineImpl::CameraCallback(float delta_time)
{
    ImGuiIO& io = ImGui::GetIO();
    if (is_active_) {
        free_camera_->KeyboardUpdate();
    }

    if (is_hovered_) {
        for (int i = 0; i < 5; ++i) {
            if (io.MouseClicked[i]) {
                free_camera_->MouseButtonUpdate(i);
            }
        }
        float fovAdjustment = io.MouseWheel * 5.0f;
        if (fovAdjustment != 0) {
            free_camera_->MouseScrollUpdate(fovAdjustment);
        }
    }
    for (int i = 0; i < 5; ++i) {
        if (io.MouseReleased[i]) {
            free_camera_->MouseButtonUpdate(i);
        }
    }
    free_camera_->MousePosUpdate(io.MousePos.x, io.MousePos.y);

    free_camera_->Animate(delta_time);

    return false;
}

UsdviewEngine::UsdviewEngine(pxr::UsdStageRefPtr root_stage)
{
    impl_ = std::make_unique<UsdviewEngineImpl>(root_stage);
}

UsdviewEngine::~UsdviewEngine()
{
}

void UsdviewEngine::render(NodeTree* render_node_tree, NodeTreeExecutor* get_executor)
{
    auto delta_time = ImGui::GetIO().DeltaTime;

    // ImGui::SetNextWindowSize({ 800, 600 });
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));

    if (ImGui::Begin(
            "UsdView Engine", nullptr, ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoCollapse)) {
        ImGui::PopStyleVar(1);

        auto size = ImGui::GetContentRegionAvail();
        size.y -= 28;

        impl_->OnResize(size.x, size.y);

        impl_->OnFrame(delta_time, render_node_tree, get_executor);
        impl_->time_controller(delta_time);
    }
    else {
        ImGui::PopStyleVar(1);
    }
    ImGui::End();
}

float UsdviewEngine::current_time_code()
{
    return impl_->timecode;
}

void UsdviewEngine::set_current_time_code(float time_code)
{
    impl_->set_current_time_code(time_code);
}

USTC_CG_NAMESPACE_CLOSE_SCOPE
