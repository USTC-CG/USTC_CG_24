#include "Nodes/node.hpp"
#include "Nodes/node_declare.hpp"
#include "Nodes/node_register.h"
#include "camera.h"
#include "geometries/mesh.h"
#include "light.h"
#include "material.h"
#include "pxr/imaging/hd/tokens.h"
#include "render_node_base.h"
#include "rich_type_buffer.hpp"

namespace USTC_CG::node_debug_info {
static void node_declare(NodeDeclarationBuilder& b)
{
    b.add_input<decl::Lights>("Lights");
    b.add_input<decl::Camera>("Camera");
    b.add_input<decl::Meshes>("Meshes");
    b.add_input<decl::Materials>("Materials");
}

static void node_exec(ExeParams params)
{
    // Left empty.
    auto lights = params.get_input<LightArray>("Lights");
    auto cameras = params.get_input<CameraArray>("Camera");
    auto meshes = params.get_input<MeshArray>("Meshes");
    MaterialMap materials = params.get_input<MaterialMap>("Materials");

    for (auto&& camera : cameras) {
        std::cout << camera->GetTransform() << std::endl;
    }

    for (auto&& light : lights) {
        std::cout << light->Get(HdTokens->transform).Cast<GfMatrix4d>() << std::endl;
    }

    for (auto&& mesh : meshes) {
        std::cout << mesh->GetId() << std::endl;
        auto names = mesh->GetBuiltinPrimvarNames();
        for (auto&& name : names) {
            std::cout << name.GetString() << std::endl;
        }
        logging(
            "Mesh contained material: " + materials[mesh->GetMaterialId()]->GetId().GetString(),
            Info);
    }
}

static void node_register()
{
    static NodeTypeInfo ntype;

    strcpy(ntype.ui_name, "Debug Info");
    strcpy_s(ntype.id_name, "render_debug_info");

    render_node_type_base(&ntype);
    ntype.node_execute = node_exec;
    ntype.ALWAYS_REQUIRED = true;
    ntype.declare = node_declare;
    nodeRegisterType(&ntype);
}

NOD_REGISTER_NODE(node_register)
}  // namespace USTC_CG::node_debug_info
