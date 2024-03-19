// My resharper is not working well with the _MSC_VER_ macro.
#include "GUI/usdview_engine.h"

#include <pxr\base\gf\quatd.h>

#include <cmath>

#include "GCore/GlobalUsdStage.h"
#include "free_camera.h"
#include "imgui.h"
#include "pxr/base/gf/camera.h"
#include "pxr/imaging/glf/drawTarget.h"
#include "pxr/pxr.h"
#include "pxr/usd/usd/primRange.h"
#include "pxr/usd/usd/stage.h"
#include "pxr/usd/usdGeom/camera.h"
#include "pxr/usdImaging/usdImagingGL/engine.h"

USTC_CG_NAMESPACE_OPEN_SCOPE
using namespace pxr;

class UsdviewEngineImpl {
   public:
    UsdviewEngineImpl(pxr::UsdStageRefPtr stage)
    {
        renderer_ = std::make_unique<UsdImagingGLEngine>();
        renderer_->SetEnablePresentation(true);

        auto plugins = renderer_->GetRendererPlugins();
        renderer_->SetRendererPlugin(plugins[0]);

        GfCamera::Projection proj = GfCamera::Projection::Perspective;
        free_camera_.SetProjection(proj);

        GfMatrix4d t{ 1.0f };
        auto position = GfVec3d(0, 0, radius_);
        t.SetLookAt(position, GfVec3d(0, 0, 0), GfVec3d{ 0, 1, 0 });
        free_camera_.SetTransform(t.GetInverse());

        azimuth_ = 0.f;
        elevation_ = 0.f;
    }

    void OnFrame(float delta_time);
    void OnResize(int x, int y);

   private:
    ThirdPersonCamera free_camera_;
    float radius_ = 10.0;
    ImVec2 top_left_;
    float elevation_, azimuth_;

    bool is_hovered_ = false;
    bool is_rotating_ = false;

    std::unique_ptr<UsdImagingGLEngine> renderer_;

    UsdImagingGLRenderParams _renderParams;
    GfVec2i renderBufferSize_;

    bool CameraCallback(float delta_time);
    void AdjustCameraFov(float fovAdjustment);
    // void UpdateCameraRotation(float deltaX, float deltaY);
    void UpdateCameraRotation(
        float lastMouseX,
        float lastMouseY,
        float currentMouseX,
        float currentMouseY);
};

void UsdviewEngineImpl::OnFrame(float delta_time)
{
    // Update the camera when mouse is in the subwindow
    if (is_hovered_) {
        CameraCallback(delta_time);
    }

    auto frustum = free_camera_.GetFrustum();

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

    for (int i = 0; i < free_camera_.GetClippingPlanes().size(); ++i) {
        _renderParams.clipPlanes[i] = free_camera_.GetClippingPlanes()[i];
    }

    GlfSimpleLightVector lights(1);
    auto cam_pos = frustum.GetPosition();
    lights[0].SetPosition(GfVec4f{ float(cam_pos[0]), float(cam_pos[1]), float(cam_pos[2]), 1.0f });
    lights[0].SetAmbient(GfVec4f(0, 0, 0, 0));
    lights[0].SetDiffuse(GfVec4f(1.0f) * 3);
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

    top_left_ = ImGui::GetCursorScreenPos();

    ImGui::Image(ImTextureID(texture), ImGui::GetContentRegionAvail());

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
    free_camera_.SetProjection(proj);

    free_camera_.m_ViewportSize = renderBufferSize_;
}

bool UsdviewEngineImpl::CameraCallback(float delta_time)
{
    ImGuiIO& io = ImGui::GetIO();
    for (int i = 0; i < 5; ++i) {
        if (io.MouseClicked[i] || io.MouseReleased[i]) {
            free_camera_.MouseButtonUpdate(i);
        }
    }
    free_camera_.MousePosUpdate(io.MousePos.x, io.MousePos.y);

    float fovAdjustment = io.MouseWheel * 5.0f;
    if (fovAdjustment != 0) {
        free_camera_.MouseScrollUpdate(fovAdjustment);
    }

    free_camera_.Animate(delta_time);

    // if (fovAdjustment != 0.0f) {
    //     AdjustCameraFov(fovAdjustment);
    // }
    // static bool status = false;
    // static ImVec2 pos = io.MousePos;
    // if (!status && ImGui::IsMouseClicked(ImGuiMouseButton_Left)) {
    //     pos = io.MousePos;
    //     status = true;
    // }
    // if (status) {
    //     float delta_x = io.MousePos.x - pos.x;
    //     float delta_y = io.MousePos.y - pos.y;
    //     UpdateCameraRotation(
    //         pos.x - top_left_.x,
    //         pos.y - top_left_.y,
    //         io.MousePos.x - top_left_.x,
    //         io.MousePos.y - top_left_.y);
    //     pos = io.MousePos;
    //     if (ImGui::IsMouseReleased(ImGuiMouseButton_Left))
    //         status = false;
    // }
    return false;
}

void UsdviewEngineImpl::AdjustCameraFov(float fovAdjustment)
{
    float currentFocalLength = free_camera_.GetFocalLength();

    double horizontalAperture = 36.0;

    double currentFovRadians = 2.0 * atan(horizontalAperture / (2.0 * currentFocalLength));

    double currentFovDegrees = currentFovRadians * 180.0 / M_PI;

    double newFovDegrees = std::max(1.0, std::min(179.0, currentFovDegrees + fovAdjustment));

    double newFovRadians = newFovDegrees * M_PI / 180.0;
    double newFocalLength = horizontalAperture / (2.0 * tan(newFovRadians / 2.0));

    free_camera_.SetFocalLength(newFocalLength);
}

inline GfVec3d MapToTrackball(
    float mouseX,
    float mouseY,
    float trackballRadius,
    float screenWidth,
    float screenHeight)
{
    float x = (2.0f * mouseX - screenWidth) / screenWidth;
    float y = (screenHeight - 2.0f * mouseY) / screenHeight;
    float z2 = trackballRadius * trackballRadius - x * x - y * y;
    float z = z2 > 0 ? sqrt(z2) : 0;

    return GfVec3d(x, y, z).GetNormalized();
}

void UsdviewEngineImpl::UpdateCameraRotation(
    float lastMouseX,
    float lastMouseY,
    float currentMouseX,
    float currentMouseY)
{
    // float screenWidth = width_;
    // float screenHeight = height_;
    // float trackballRadius = 1.0f;
    // float sensitivity = 2.0f;

    // GfVec3d lastPos =
    //     MapToTrackball(lastMouseX, lastMouseY, trackballRadius, screenWidth, screenHeight);
    // GfVec3d currentPos =
    //     MapToTrackball(currentMouseX, currentMouseY, trackballRadius, screenWidth, screenHeight);

    // GfVec3d axis = GfCross(currentPos, lastPos).GetNormalized();
    // if (axis.GetLength() < 1e-7) {
    //     return;
    // }

    // float dot = std::max(-1.0, std::min(1.0, GfDot(lastPos, currentPos)));
    // float angle = acos(dot) * sensitivity;

    // GfQuatd rotationQuat = GfRotation(axis, GfRadiansToDegrees(angle)).GetQuat();

    // GfMatrix4d cameraMatrix = free_camera_.GetTransform();
    // GfVec3d cameraPosition = cameraMatrix.ExtractTranslation();
    // GfVec3d lookAtPoint(0, 0, 0);
    // GfVec3d upVector(0, 1, 0);

    // GfMatrix4d rotationMatrix(GfRotation(rotationQuat.GetInverse()), GfVec3d(0, 0, 0));
    // GfVec3d newPosition = rotationMatrix.TransformAffine(cameraPosition);

    // free_camera_.SetTransform(
    //     GfMatrix4d().SetLookAt(newPosition, lookAtPoint, upVector).GetInverse());
}

inline ImGuiWindowFlags GetWindowFlags()
{
    return ImGuiWindowFlags_NoDecoration;
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

    ImGui::Begin("UsdView Engine", nullptr, GetWindowFlags());
    auto size = ImGui::GetContentRegionAvail();

    impl_->OnResize(size.x, size.y);

    impl_->OnFrame(delta_time);

    ImGui::End();
}

USTC_CG_NAMESPACE_CLOSE_SCOPE
