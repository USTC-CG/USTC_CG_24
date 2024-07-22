// #undef _MSC_VER

#include "GCore/Components/MeshOperand.h"

#include "GCore/GOP.h"

USTC_CG_NAMESPACE_OPEN_SCOPE
MeshComponent::~MeshComponent()
{
    GlobalUsdStage::global_usd_stage->RemovePrim(scratch_buffer_path);
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

GeometryComponentHandle MeshComponent::copy(Geometry* operand) const
{
    auto ret = std::make_shared<MeshComponent>(operand);

    // This is fast because the VtArray has the copy on write mechanism
    ret->mesh = this->mesh;
    return ret;
}

void MeshComponent::set_mesh_geom(const pxr::UsdGeomMesh& usdgeom)
{
    for (pxr::UsdAttribute attr : usdgeom.GetPrim().GetAttributes()) {
        // Copy attribute value
        pxr::VtValue value;
        if (attr.Get(&value)) {
            mesh.GetPrim()
                .CreateAttribute(attr.GetName(), attr.GetTypeName())
                .Set(value);
        }
    }
}

USTC_CG_NAMESPACE_CLOSE_SCOPE
