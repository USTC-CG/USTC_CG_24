#pragma once
#include <pxr/usd/usdGeom/mesh.h>

#include <string>

#include "GCore/Components.h"
#include "GCore/GOP.h"
#include "Nodes/GlobalUsdStage.h"
#include "pxr/usd/usdGeom/primvarsAPI.h"
#include "pxr/usd/usdGeom/xform.h"

USTC_CG_NAMESPACE_OPEN_SCOPE
struct USTC_CG_API MeshComponent : public GeometryComponent {
    explicit MeshComponent(Geometry* attached_operand)
        : GeometryComponent(attached_operand)
    {

        mesh = pxr::UsdGeomMesh::Define(
            GlobalUsdStage::global_usd_stage, scratch_buffer_path);
    }

    ~MeshComponent() override;

    std::string to_string() const override;

    GeometryComponentHandle copy(Geometry* operand) const override;
    [[nodiscard]] pxr::VtArray<pxr::GfVec3f> get_vertices() const
    {
        pxr::VtArray<pxr::GfVec3f> vertices;
        mesh.GetPointsAttr().Get(&vertices);
        return vertices;
    }

    [[nodiscard]] pxr::VtArray<int> get_face_vertex_counts() const
    {
        pxr::VtArray<int> faceVertexCounts;
        mesh.GetFaceVertexCountsAttr().Get(&faceVertexCounts);
        return faceVertexCounts;
    }

    [[nodiscard]] pxr::VtArray<int> get_face_vertex_indices() const
    {
        pxr::VtArray<int> faceVertexIndices;
        mesh.GetFaceVertexIndicesAttr().Get(&faceVertexIndices);
        return faceVertexIndices;
    }

    [[nodiscard]] pxr::VtArray<pxr::GfVec3f> get_normals() const
    {
        pxr::VtArray<pxr::GfVec3f> normals;
        mesh.GetNormalsAttr().Get(&normals);
        return normals;
    }

    [[nodiscard]] pxr::VtArray<pxr::GfVec2f> get_texcoords_array() const
    {
        auto PrimVarAPI = pxr::UsdGeomPrimvarsAPI(mesh);
        auto primvar = PrimVarAPI.CreatePrimvar(
            pxr::TfToken("UVMap"), pxr::SdfValueTypeNames->TexCoord2fArray);
        pxr::VtArray<pxr::GfVec2f> texcoordsArray;
        primvar.Get(&texcoordsArray);
        return texcoordsArray;
    }

    [[nodiscard]] pxr::VtArray<pxr::GfVec3f> get_display_color() const
    {
        pxr::VtArray<pxr::GfVec3f> displayColor;
        mesh.GetDisplayColorAttr().Get(&displayColor);
        return displayColor;
    }

    void set_vertices(const pxr::VtArray<pxr::GfVec3f>& vertices)
    {
        mesh.CreatePointsAttr().Set(vertices);
    }

    void set_face_vertex_counts(const pxr::VtArray<int>& face_vertex_counts)
    {
        mesh.CreateFaceVertexCountsAttr().Set(face_vertex_counts);
    }

    void set_face_vertex_indices(const pxr::VtArray<int>& face_vertex_indices)
    {
        mesh.CreateFaceVertexIndicesAttr().Set(face_vertex_indices);
    }

    void set_normals(const pxr::VtArray<pxr::GfVec3f>& normals)
    {
        mesh.CreateNormalsAttr().Set(normals);
    }
    void set_texcoords_array(const pxr::VtArray<pxr::GfVec2f>& texcoords_array)
    {
        auto PrimVarAPI = pxr::UsdGeomPrimvarsAPI(mesh);
        auto primvar = PrimVarAPI.CreatePrimvar(
            pxr::TfToken("UVMap"), pxr::SdfValueTypeNames->TexCoord2fArray);
        primvar.Set(texcoords_array);
    }

    void set_display_color(const pxr::VtArray<pxr::GfVec3f>& display_color)
    {
        mesh.CreateDisplayColorAttr().Set(display_color);
    }

    void set_mesh_geom(const pxr::UsdGeomMesh& usdgeom);

   private:
    pxr::UsdGeomMesh mesh;

};

USTC_CG_NAMESPACE_CLOSE_SCOPE
