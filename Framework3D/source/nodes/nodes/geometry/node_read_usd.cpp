#include <pxr/usd/usd/primRange.h>
#include <pxr/usd/usdGeom/mesh.h>
#include <pxr/usd/usdGeom/primvarsAPI.h>
#include <pxr/usd/usdShade/material.h>
#include <pxr/usd/usdShade/materialBindingAPI.h>

#include "GCore/GlobalUsdStage.h"
#include "GCore/Components/MaterialComponent.h"
#include "GCore/Components/MeshOperand.h"
#include "Nodes/node.hpp"
#include "Nodes/node_declare.hpp"
#include "Nodes/node_register.h"
#include "geom_node_base.h"

namespace USTC_CG::node_read_usd {
static void node_declare(NodeDeclarationBuilder& b)
{
    b.add_input<decl::String>("File Name").default_val("Default");
    b.add_input<decl::String>("Prim Path").default_val("geometry");
    b.add_input<decl::Int>("Time Code").default_val(0).min(0).max(240);
    b.add_output<decl::Geometry>("Geometry");
}

static void node_exec(ExeParams params)
{
    auto file_name = params.get_input<std::string>("File Name");
    auto prim_path = params.get_input<std::string>("Prim Path");

    GOperandBase geometry;
    std::shared_ptr<MeshComponent> mesh = std::make_shared<MeshComponent>(&geometry);
    geometry.attach_component(mesh);

    auto t = params.get_input<int>("Time Code");
    pxr::UsdTimeCode time = pxr::UsdTimeCode(t);
    if (t == 0) {
        time = pxr::UsdTimeCode::Default();
    }

    auto stage = pxr::UsdStage::Open(file_name);

    if (stage) {
        // Here 'c_str' call is necessary since prim_path
        auto sdf_path = pxr::SdfPath(prim_path.c_str());
        pxr::UsdGeomMesh usdgeom(stage->GetPrimAtPath(sdf_path));

        if (usdgeom) {
            // Fill in the vertices and faces here
            usdgeom.CreatePointsAttr().Get(&mesh->vertices, time);
            usdgeom.CreateFaceVertexCountsAttr().Get(&mesh->faceVertexCounts, time);
            usdgeom.CreateFaceVertexIndicesAttr().Get(&mesh->faceVertexIndices, time);

            usdgeom.CreateNormalsAttr().Get(&mesh->normals, time);

            auto PrimVarAPI = pxr::UsdGeomPrimvarsAPI(usdgeom);
            pxr::UsdGeomPrimvar primvar = PrimVarAPI.GetPrimvar(pxr::TfToken("UVMap"));
            primvar.Get(&mesh->texcoordsArray, time);
        }

        // TODO: add material reading
    }
    else {
        // TODO: throw something
    }
    params.set_output("Geometry", std::move(geometry));
}

static void node_register()
{
    static NodeTypeInfo ntype;

    strcpy(ntype.ui_name, "Read USD");
    strcpy_s(ntype.id_name, "geom_read_usd");

    geo_node_type_base(&ntype);
    ntype.node_execute = node_exec;
    ntype.declare = node_declare;
    nodeRegisterType(&ntype);
}

NOD_REGISTER_NODE(node_register)
}  // namespace USTC_CG::node_read_usd
