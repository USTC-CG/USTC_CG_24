#pragma once
#include <string>

#include "GCore/Components.h"
#include "GCore/GOP.h"
#include "pxr/usd/usdGeom/xform.h"

USTC_CG_NAMESPACE_OPEN_SCOPE
struct USTC_CG_API MeshComponent : public GOperandComponent {
    explicit MeshComponent(GOperandBase* attached_operand)
        : GOperandComponent(attached_operand)
    {
    }

    std::string to_string() const override;

    GOperandComponentHandle copy(GOperandBase* operand) const override;

    [[nodiscard]] pxr::VtArray<pxr::GfVec3f>& get_vertices()
    {
        return vertices;
    }

    [[nodiscard]] pxr::VtArray<int>& get_face_vertex_counts()
    {
        return faceVertexCounts;
    }

    [[nodiscard]] pxr::VtArray<int>& get_face_vertex_indices()
    {
        return faceVertexIndices;
    }

    [[nodiscard]] pxr::VtArray<float>& get_control_points()
    {
        return controlPoints;
    }

    [[nodiscard]] pxr::VtArray<pxr::GfVec3f>& get_normals()
    {
        return normals;
    }

    [[nodiscard]] pxr::VtArray<pxr::GfVec2f>& get_texcoords_array()
    {
        return texcoordsArray;
    }

    [[nodiscard]] pxr::VtArray<pxr::GfVec3f>& get_display_color()
    {
        return displayColor;
    }

   private:
    pxr::VtArray<pxr::GfVec3f> vertices;
    pxr::VtArray<int> faceVertexCounts;
    pxr::VtArray<int> faceVertexIndices;
    pxr::VtArray<float>
        controlPoints;  // point group for combining animation and simulation

    pxr::VtArray<pxr::GfVec3f> normals;
    pxr::VtArray<pxr::GfVec2f> texcoordsArray;
    pxr::VtArray<pxr::GfVec3f> displayColor;
};

USTC_CG_NAMESPACE_CLOSE_SCOPE
