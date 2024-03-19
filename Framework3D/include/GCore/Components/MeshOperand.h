#pragma once
#include <string>

#include "GCore/Components.h"
#include "GCore/GOP.h"
#include "pxr/usd/usdGeom/xform.h"

USTC_CG_NAMESPACE_OPEN_SCOPE
struct USTC_CG_API MeshComponent : public GOperandComponent {
    explicit MeshComponent(GOperandBase* attached_operand) : GOperandComponent(attached_operand)
    {
    }

    std::string to_string() const override;

    pxr::VtArray<pxr::GfVec3f> vertices;
    pxr::VtArray<int> faceVertexCounts;
    pxr::VtArray<int> faceVertexIndices;

    pxr::VtArray<pxr::GfVec3f> normals;
    pxr::VtArray<pxr::GfVec2f> texcoordsArray;
    pxr::VtArray<pxr::GfVec3f> displayColor;

    GOperandComponentHandle copy(GOperandBase* operand) const override;
};

USTC_CG_NAMESPACE_CLOSE_SCOPE
