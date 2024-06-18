#pragma once
#include "USTC_CG.h"

#include "renderBuffer.h"
#include "pxr/pxr.h"
#include "pxr/base/gf/ray.h"
#include "pxr/base/gf/rect2i.h"
#include "pxr/imaging/hd/camera.h"
#include "pxr/imaging/hdx/renderSetupTask.h"
USTC_CG_NAMESPACE_OPEN_SCOPE
using namespace pxr;
class Hd_USTC_CG_Camera : public HdCamera
{
public:
    explicit Hd_USTC_CG_Camera(const SdfPath& id)
        : HdCamera(id)
    {
    }

    void Sync(
        HdSceneDelegate* sceneDelegate,
        HdRenderParam* renderParam,
        HdDirtyBits* dirtyBits) override;
    virtual GfRay generateRay(
        GfVec2f pixel_center,
        const std::function<float()>& function) const;

    void update(const HdRenderPassStateSharedPtr& renderPassState) const;

    void attachFilm(Hd_USTC_CG_RenderBuffer* new_film) const;

    mutable Hd_USTC_CG_RenderBuffer* film;
    mutable GfRect2i dataWindow;
private:
    mutable GfMatrix4d inverseProjMatrix;
    mutable GfMatrix4d projMatrix;
    mutable GfMatrix4d inverseViewMatrix;
    mutable GfMatrix4d viewMatrix;
};

USTC_CG_NAMESPACE_CLOSE_SCOPE
