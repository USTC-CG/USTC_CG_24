#include "Nodes/node.hpp"
#include "Nodes/node_declare.hpp"
#include "Nodes/node_register.h"
#include "RCore/Backend.hpp"
#include "render_node_base.h"

namespace USTC_CG::node_scene_present {
static void node_declare(NodeDeclarationBuilder& b)
{
    b.add_input<decl::Texture>("Present");
}

static void node_exec(ExeParams params)
{
    // Do nothing. Wait for external statements to fetch
}

static void node_register()
{
    static NodeTypeInfo ntype;

    strcpy(ntype.ui_name, "Present");
    strcpy_s(ntype.id_name, "render_present");

    render_node_type_base(&ntype);
    ntype.node_execute = node_exec;
    ntype.ALWAYS_REQUIRED = true;
    ntype.declare = node_declare;
    nodeRegisterType(&ntype);
}

NOD_REGISTER_NODE(node_register)
}  // namespace USTC_CG::node_scene_present
