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

    _projMatrix = this->ComputeProjectionMatrix();
    _inverseProjMatrix = _projMatrix.GetInverse();

    _inverseViewMatrix = GetTransform();
    _viewMatrix = _inverseViewMatrix.GetInverse();
}

GfRay Hd_USTC_CG_Camera::generateRay(
    GfVec2f pixel_center,
    const std::function<float()>& uniform_float) const
{
    const unsigned int minX = _dataWindow.GetMinX();
    unsigned int minY = _dataWindow.GetMinY();
    const unsigned int maxX = _dataWindow.GetMaxX() + 1;
    unsigned int maxY = _dataWindow.GetMaxY() + 1;

    float x = pixel_center[0];
    float y = pixel_center[1];
    GfVec2f jitter(0.0f, 0.0f);
    if (HdEmbreeConfig::GetInstance().jitterCamera)
    {
        jitter = GfVec2f(uniform_float(), uniform_float());
    }

    // Un-transform the pixel's NDC coordinates through the
    // projection matrix to get the trace of the camera ray in
    // the near plane.
    const float w(_dataWindow.GetWidth());
    const float h(_dataWindow.GetHeight());

    const GfVec3f ndc(
        2 * ((x + jitter[0] - minX) / w) - 1,
        2 * ((y + jitter[1] - minY) / h) - 1,
        -1);
    const GfVec3f nearPlaneTrace = _inverseProjMatrix.Transform(ndc);

    GfVec3f origin;
    GfVec3f dir;

    const bool isOrthographic = round(_projMatrix[3][3]) == 1;
    if (isOrthographic)
    {
        // During orthographic projection: trace parallel rays
        // from the near plane trace.
        origin = nearPlaneTrace;
        dir = GfVec3f(0, 0, -1);
    }
    else
    {
        // Otherwise, assume this is a perspective projection;
        // project from the camera origin through the
        // near plane trace.
        origin = GfVec3f(0, 0, 0);
        dir = nearPlaneTrace;
    }
    // Transform camera rays to world space.
    origin = _inverseViewMatrix.Transform(origin);
    dir = _inverseViewMatrix.TransformDir(dir).GetNormalized();
    return { origin, dir };
}

static GfRect2i _GetDataWindow(
    const HdRenderPassStateSharedPtr& renderPassState)
{
    const CameraUtilFraming& framing = renderPassState->GetFraming();
    if (framing.IsValid())
    {
        return framing.dataWindow;
    }
    else
    {
        const GfVec4f vp = renderPassState->GetViewport();
        return GfRect2i(GfVec2i(0), int(vp[2]), int(vp[3]));
    }
}

void Hd_USTC_CG_Camera::update(
    const HdRenderPassStateSharedPtr& renderPassState) const
{
    _projMatrix = renderPassState->GetProjectionMatrix();
    _inverseProjMatrix = _projMatrix.GetInverse();
    _viewMatrix = renderPassState->GetWorldToViewMatrix();
    _inverseViewMatrix = _viewMatrix.GetInverse();
    _dataWindow = _GetDataWindow(renderPassState);
}

void Hd_USTC_CG_Camera::attachFilm(Hd_USTC_CG_RenderBuffer* new_film) const
{
    film = new_film;
}


USTC_CG_NAMESPACE_CLOSE_SCOPE
