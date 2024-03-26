#include "Nodes/GlobalUsdStage.h"
#include "GCore/Components/MeshOperand.h"
#include "Nodes/node.hpp"
#include "Nodes/node_declare.hpp"
#include "Nodes/node_register.h"
#include "geom_node_base.h"

namespace USTC_CG::node_mesh_decompose {
static void node_declare(NodeDeclarationBuilder& b)
{
    b.add_input<decl::Geometry>("Mesh");

    b.add_output<decl::Float3Buffer>("Vertices");
    b.add_output<decl::Int1Buffer>("FaceVertexCounts");
    b.add_output<decl::Int1Buffer>("FaceVertexIndices");
    b.add_output<decl::Float3Buffer>("Normals");
    b.add_output<decl::Float2Buffer>("Texcoords");
}

static void node_exec(ExeParams params)
{
    GOperandBase geometry = params.get_input<GOperandBase>("Mesh");
    auto mesh_component = geometry.get_component<MeshComponent>();

    if (mesh_component) {
        auto vertices = mesh_component->vertices;
        auto faceVertexCounts = mesh_component->faceVertexCounts;
        auto faceVertexIndices = mesh_component->faceVertexIndices;
        auto normals = mesh_component->normals;
        auto texcoordsArray = mesh_component->texcoordsArray;

        params.set_output("Vertices", vertices);
        params.set_output("FaceVertexCounts", faceVertexCounts);
        params.set_output("FaceVertexIndices", faceVertexIndices);
        params.set_output("Normals", normals);
        params.set_output("Texcoords", texcoordsArray);
    }
    else {
        // TODO: Throw something
    }
}

static void node_register()
{
    static NodeTypeInfo ntype;

    strcpy(ntype.ui_name, "Mesh Decompose");
    strcpy_s(ntype.id_name, "geom_mesh_decompose");

    geo_node_type_base(&ntype);
    ntype.node_execute = node_exec;
    ntype.declare = node_declare;
    nodeRegisterType(&ntype);
}

NOD_REGISTER_NODE(node_register)
}  // namespace USTC_CG::node_mesh_decompose
