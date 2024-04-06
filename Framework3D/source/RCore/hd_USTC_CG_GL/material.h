#pragma once
#include "USTC_CG.h"
#include "pxr/imaging/hd/material.h"
USTC_CG_NAMESPACE_OPEN_SCOPE
using namespace pxr;

class Hd_USTC_CG_Material : public HdMaterial {
public:
    explicit Hd_USTC_CG_Material(SdfPath const& id)
        : HdMaterial(id)
    {
    }

    void Sync(
        HdSceneDelegate* sceneDelegate,
        HdRenderParam* renderParam,
        HdDirtyBits* dirtyBits) override;
    HdDirtyBits GetInitialDirtyBitsMask() const override;

};

USTC_CG_NAMESPACE_CLOSE_SCOPE