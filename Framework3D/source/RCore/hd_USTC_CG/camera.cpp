#include "camera.h"

#include "config.h"
USTC_CG_NAMESPACE_OPEN_SCOPE
using namespace pxr;
void Hd_USTC_CG_Camera::Sync(
    HdSceneDelegate* sceneDelegate,
    HdRenderParam* renderParam,
    HdDirtyBits* dirtyBits)
{
    HdCamera::Sync(sceneDelegate, renderParam, dirtyBits);

    projMatrix = this->ComputeProjectionMatrix();
    inverseProjMatrix = projMatrix.GetInverse();

    inverseViewMatrix = GetTransform();
    viewMatrix = inverseViewMatrix.GetInverse();
}

GfRay Hd_USTC_CG_Camera::generateRay(
    GfVec2f pixel_center,
    const std::function<float()>& uniform_float) const
{
    const unsigned int minX = dataWindow.GetMinX();
    unsigned int minY = dataWindow.GetMinY();
    const unsigned int maxX = dataWindow.GetMaxX() + 1;
    unsigned int maxY = dataWindow.GetMaxY() + 1;

    float x = pixel_center[0];
    float y = pixel_center[1];
    GfVec2f jitter(0.0f, 0.0f);
    if (Hd_USTC_CG_Config::GetInstance().jitterCamera) {
        jitter = GfVec2f(uniform_float() - 0.5, uniform_float() - 0.5);
    }

    // Un-transform the pixel's NDC coordinates through the
    // projection matrix to get the trace of the camera ray in
    // the near plane.
    const float w(dataWindow.GetWidth());
    const float h(dataWindow.GetHeight());

    const GfVec3f ndc(
        2 * ((x + jitter[0] - minX) / w) - 1, 2 * ((y + jitter[1] - minY) / h) - 1, -1);
    const GfVec3f nearPlaneTrace = inverseProjMatrix.Transform(ndc);

    GfVec3f origin;
    GfVec3f dir;

    const bool isOrthographic = round(projMatrix[3][3]) == 1;
    if (isOrthographic) {
        // During orthographic projection: trace parallel rays
        // from the near plane trace.
        origin = nearPlaneTrace;
        dir = GfVec3f(0, 0, -1);
    }
    else {
        // Otherwise, assume this is a perspective projection;
        // project from the camera origin through the
        // near plane trace.
        origin = GfVec3f(0, 0, 0);
        dir = nearPlaneTrace;
    }
    // Transform camera rays to world space.
    origin = inverseViewMatrix.Transform(origin);
    dir = inverseViewMatrix.TransformDir(dir).GetNormalized();
    return { origin, dir };
}

static GfRect2i _GetDataWindow(const HdRenderPassStateSharedPtr& renderPassState)
{
    const CameraUtilFraming& framing = renderPassState->GetFraming();
    if (framing.IsValid()) {
        return framing.dataWindow;
    }
    else {
        const GfVec4f vp = renderPassState->GetViewport();
        return GfRect2i(GfVec2i(0), int(vp[2]), int(vp[3]));
    }
}

void Hd_USTC_CG_Camera::update(const HdRenderPassStateSharedPtr& renderPassState) const
{
    projMatrix = renderPassState->GetProjectionMatrix();
    inverseProjMatrix = projMatrix.GetInverse();
    viewMatrix = renderPassState->GetWorldToViewMatrix();
    inverseViewMatrix = viewMatrix.GetInverse();
    dataWindow = _GetDataWindow(renderPassState);
}

void Hd_USTC_CG_Camera::attachFilm(Hd_USTC_CG_RenderBuffer* new_film) const
{
    film = new_film;
}

USTC_CG_NAMESPACE_CLOSE_SCOPE
