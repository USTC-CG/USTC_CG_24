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


    void update(const HdRenderPassStateSharedPtr& renderPassState) const;

    mutable GfRect2i _dataWindow;
    mutable GfMatrix4d _inverseProjMatrix;
    mutable GfMatrix4d _projMatrix;
    mutable GfMatrix4d _inverseViewMatrix;
    mutable GfMatrix4d _viewMatrix;
};

USTC_CG_NAMESPACE_CLOSE_SCOPE
