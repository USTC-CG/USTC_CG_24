#pragma once

#include "USTC_CG.h"
#include "pxr/base/gf/vec3d.h"
#include "pxr/base/gf/ray.h"
#include "pxr/imaging/hd/sceneDelegate.h"

USTC_CG_NAMESPACE_OPEN_SCOPE

struct PickEvent {
    pxr::GfVec3d point;
    pxr::GfVec3d normal;
    pxr::SdfPath path;
    pxr::SdfPath instancer;
    pxr::HdInstancerContext outInstancerContext;
    int outHitInstanceIndex;
    pxr::GfRay pick_ray;
};

USTC_CG_NAMESPACE_CLOSE_SCOPE