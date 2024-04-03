#include "Nodes/node.hpp"
#include "Nodes/node_declare.hpp"
#include "Nodes/node_register.h"
#include "stage_node_base.h"

namespace USTC_CG::node_read_usd {
static void node_declare(NodeDeclarationBuilder& b)
{
}

static void node_exec(ExeParams params)
{
}

static void node_register()
{
    static NodeTypeInfo ntype;

    strcpy(ntype.ui_name, "Scene Lights");
    strcpy_s(ntype.id_name, "stage_read_usd");

    stage_node_type_base(&ntype);
    ntype.node_execute = node_exec;
    ntype.declare = node_declare;
    nodeRegisterType(&ntype);
}

NOD_REGISTER_NODE(node_register)
}  // namespace USTC_CG::node_read_usd
