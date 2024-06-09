#include "NODES_FILES_DIR.h"
#include "Nodes/node.hpp"
#include "Nodes/node_declare.hpp"
#include "Nodes/node_register.h"
#include "RCore/Backend.hpp"
#include "nvrhi/utils.h"
#include "render_node_base.h"
#include "resource_allocator_instance.hpp"

namespace USTC_CG::node_dynamic_socket {
static void node_declare(NodeDeclarationBuilder& b)
{
}

static void node_exec(ExeParams params)
{
}

static void node_register()
{
    static NodeTypeInfo ntype;

    strcpy(ntype.ui_name, "dynamic_socket");
    strcpy(ntype.id_name, "node_dynamic_socket");

    render_node_type_base(&ntype);
    ntype.node_execute = node_exec;
    ntype.declare = node_declare;
    nodeRegisterType(&ntype);
}

NOD_REGISTER_NODE(node_register)
}  // namespace USTC_CG::node_dynamic_socket
