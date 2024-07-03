#include "Nodes/node.hpp"
#include "Nodes/node_declare.hpp"
#include "Nodes/node_register.h"
#include "RCore/Backend.hpp"
#include "render_node_base.h"

namespace USTC_CG::node_scene_present {
static void node_declare_present_color(NodeDeclarationBuilder& b)
{
    b.add_input<decl::Texture>("Color");
}

static void node_declare_present_depth(NodeDeclarationBuilder& b)
{
    b.add_input<decl::Texture>("Depth");
}

static void node_exec(ExeParams params)
{
    // Do nothing. Wait for external statements to fetch
}

static void node_register()
{
    static NodeTypeInfo ntype;

    strcpy(ntype.ui_name, "Present Color");
    strcpy(ntype.id_name, "render_present");

    render_node_type_base(&ntype);
    ntype.node_execute = node_exec;
    ntype.ALWAYS_REQUIRED = true;
    ntype.declare = node_declare_present_color;
    nodeRegisterType(&ntype);

    static NodeTypeInfo ntype_depth;

    strcpy(ntype_depth.ui_name, "Present Depth");
    strcpy(ntype_depth.id_name, "render_present_depth");

    render_node_type_base(&ntype_depth);
    ntype_depth.node_execute = node_exec;
    ntype_depth.ALWAYS_REQUIRED = true;
    ntype_depth.declare = node_declare_present_depth;
    nodeRegisterType(&ntype_depth);
}

NOD_REGISTER_NODE(node_register)
}  // namespace USTC_CG::node_scene_present
