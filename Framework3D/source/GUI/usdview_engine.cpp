// My resharper is not working well with the _MSC_VER_ macro.
#include "GUI/usdview_engine.h"

#include <cmath>

#include "GCore/GlobalUsdStage.h"
#include "free_camera.h"
#include "imgui.h"
#include "pxr/base/gf/camera.h"
#include "pxr/imaging/glf/drawTarget.h"
#include "pxr/pxr.h"
#include "pxr/usd/usd/primRange.h"
#include "pxr/usd/usd/stage.h"
#include "pxr/usdImaging/usdImagingGL/engine.h"

USTC_CG_NAMESPACE_OPEN_SCOPE
using namespace pxr;

class UsdviewEngineImpl {
   public:
    enum class CamType { First, Third };
    struct Status {
        CamType cam_type = CamType::First;  // 0 for 1st personal, 1 for 3rd personal
    } engine_status;
    UsdviewEngineImpl(pxr::UsdStageRefPtr stage)
    {
        renderer_ = std::make_unique<UsdImagingGLEngine>();
        renderer_->SetEnablePresentation(true);
        free_camera_ = std::make_unique<FirstPersonCamera>();

        auto plugins = renderer_->GetRendererPlugins();
        renderer_->SetRendererPlugin(plugins[0]);

        GfCamera::Projection proj = GfCamera::Projection::Perspective;
        free_camera_->SetProjection(proj);

        free_camera_->SetClippingRange(pxr::GfRange1f{ 0.1f, 1000.f });
    }

    void DrawMenuBar();
    void OnFrame(float delta_time);
    void OnResize(int x, int y);

   private:
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
    ImGui::EndMenuBar();
}

void UsdviewEngineImpl::OnFrame(float delta_time)
{
    DrawMenuBar();
    // Update the camera when mouse is in the subwindow
    CameraCallback(delta_time);

    auto frustum = free_camera_->GetFrustum();

    GfMatrix4d projectionMatrix = frustum.ComputeProjectionMatrix();
    GfMatrix4d viewMatrix = frustum.ComputeViewMatrix();

    renderer_->SetCameraState(viewMatrix, projectionMatrix);
    renderer_->SetRendererAov(HdAovTokens->color);

    _renderParams.enableLighting = true;
    _renderParams.enableSceneMaterials = true;
    _renderParams.showRender = true;
    _renderParams.frame = UsdTimeCode::Default();
    _renderParams.drawMode = UsdImagingGLDrawMode::DRAW_WIREFRAME_ON_SURFACE;

    _renderParams.clearColor = GfVec4f(0.2f, 0.2f, 0.2f, 1.f);

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

    auto texture = renderer_->GetAovTexture(HdAovTokens->color)->GetRawResource();
    ImGui::BeginChild("Render View Port", ImGui::GetContentRegionAvail(),0,ImGuiWindowFlags_NoMove);
    ImGui::Image(ImTextureID(texture), ImGui::GetContentRegionAvail());
    is_active_ = ImGui::IsWindowFocused();
    ImGui::EndChild();

    is_hovered_ = ImGui::IsItemHovered();
}

void UsdviewEngineImpl::OnResize(int x, int y)
{
    renderBufferSize_[0] = x;
    renderBufferSize_[1] = y;
    renderer_->SetRenderBufferSize(renderBufferSize_);
    renderer_->SetRenderViewport(
        GfVec4d{ 0.0, 0.0, double(renderBufferSize_[0]), double(renderBufferSize_[1]) });

    GfCamera::Projection proj = GfCamera::Projection::Perspective;
    free_camera_->SetProjection(proj);

    free_camera_->m_ViewportSize = renderBufferSize_;
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

void UsdviewEngine::render()
{
    auto delta_time = ImGui::GetIO().DeltaTime;

    ImGui::SetNextWindowSize({ 800, 600 });
    ImGui::Begin(
        "UsdView Engine",
        nullptr,
        ImGuiWindowFlags_MenuBar);
    auto size = ImGui::GetContentRegionAvail();

    impl_->OnResize(size.x, size.y);

    impl_->OnFrame(delta_time);

    ImGui::End();
}

USTC_CG_NAMESPACE_CLOSE_SCOPE
