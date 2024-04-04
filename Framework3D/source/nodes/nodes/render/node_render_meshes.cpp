#include "Nodes/node.hpp"
#include "Nodes/node_declare.hpp"
#include "Nodes/node_register.h"
#include "render_node_base.h"

namespace USTC_CG::node_scene_meshes {
static void node_declare(NodeDeclarationBuilder& b)
{
    b.add_output<decl::Meshes>("Meshes");
}

static void node_exec(ExeParams params)
{
    // Do nothing. The output is filled in renderer.
}

static void node_register()
{
    static NodeTypeInfo ntype;

    strcpy(ntype.ui_name, "Scene Meshes");
    strcpy_s(ntype.id_name, "render_scene_meshes");

    render_node_type_base(&ntype);
    ntype.node_execute = node_exec;
    ntype.declare = node_declare;
    nodeRegisterType(&ntype);
}

NOD_REGISTER_NODE(node_register)
}  // namespace USTC_CG::node_scene_meshes
