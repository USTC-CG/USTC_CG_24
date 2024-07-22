// #undef _MSC_VER

#include "GCore/Components/MeshOperand.h"

#include "GCore/GOP.h"

USTC_CG_NAMESPACE_OPEN_SCOPE
MeshComponent::~MeshComponent()
{
}

std::string MeshComponent::to_string() const
{
    std::ostringstream out;
    // Loop over the faces and vertices and print the data
    out << "Topology component. "
        << "Vertices count " << get_vertices().size()
        << ". Face vertices count " << get_face_vertex_counts().size()
        << ". Face vertex indices " << get_face_vertex_indices().size() << ".";
    return out.str();
}

void copy_prim(const pxr::UsdPrim& from, const pxr::UsdPrim& to)
{
    for (pxr::UsdAttribute attr : from.GetPrim().GetAttributes()) {
        // Copy attribute value
        pxr::VtValue value;
        if (attr.Get(&value)) {
            to.GetPrim()
                .CreateAttribute(attr.GetName(), attr.GetTypeName())
                .Set(value);
        }
    }
}

GeometryComponentHandle MeshComponent::copy(Geometry* operand) const
{
    auto ret = std::make_shared<MeshComponent>(operand);
    copy_prim(this->mesh.GetPrim(), ret->mesh.GetPrim());
    return ret;
}

void MeshComponent::set_mesh_geom(const pxr::UsdGeomMesh& usdgeom)
{
    copy_prim(usdgeom.GetPrim(), mesh.GetPrim());

    pxr::UsdGeomImageable(mesh).MakeInvisible();
}

USTC_CG_NAMESPACE_CLOSE_SCOPE
