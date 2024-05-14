#pragma once
#include <string>

#include "GCore/Components.h"
#include "GCore/GOP.h"
#include "pxr/usd/usdGeom/xform.h"
#include "pxr/base/gf/matrix4f.h"
#include "pxr/usd/usdSkel/topology.h"
#include "pxr/usd/usdSkel/skeleton.h"

USTC_CG_NAMESPACE_OPEN_SCOPE
struct USTC_CG_API SkelComponent : public GOperandComponent {
    explicit SkelComponent(GOperandBase* attached_operand) : GOperandComponent(attached_operand)
    {
    }

    std::string to_string() const override;

    pxr::VtTokenArray jointOrder;
    pxr::UsdSkelTopology topology;
    pxr::VtArray<pxr::GfMatrix4f> localTransforms;
    pxr::VtArray<pxr::GfMatrix4d> bindTransforms;
    pxr::VtArray<float> jointWeight;
    pxr::VtArray<int> jointIndices;

    GOperandComponentHandle copy(GOperandBase* operand) const override;
};

USTC_CG_NAMESPACE_CLOSE_SCOPE
