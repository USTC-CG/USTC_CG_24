#include "Nodes/node.hpp"
#include "Nodes/node_declare.hpp"
#include "Nodes/node_register.h"
#include "geom_node_base.h"

namespace USTC_CG::node_triangulate {
static void node_declare(NodeDeclarationBuilder& b)
{
    b.add_input<decl::Geometry>("G-IN");
    b.add_output<decl::Geometry>("G-out");
}

static void node_exec(ExeParams params)
{
    // Left empty.
    throw std::runtime_error("Not implemented!");
}


static void node_register()
{
    static NodeTypeInfo ntype;

    strcpy(ntype.ui_name, "Triangulate");
    strcpy_s(ntype.id_name, "geom_Triangulate");

    geo_node_type_base(&ntype);
    ntype.node_execute = node_exec;
    ntype.declare = node_declare;
    nodeRegisterType(&ntype);
}

NOD_REGISTER_NODE(node_register)
}  // namespace USTC_CG::node_triangulate
