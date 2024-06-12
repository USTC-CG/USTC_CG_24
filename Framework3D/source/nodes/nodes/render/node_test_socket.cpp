#include "NODES_FILES_DIR.h"
#include "Nodes/node.hpp"
#include "Nodes/node_declare.hpp"
#include "Nodes/node_register.h"
#include "RCore/Backend.hpp"
#include "nvrhi/utils.h"
#include "render_node_base.h"
#include "resource_allocator_instance.hpp"

namespace USTC_CG::node_test_socket {
static void node_declare(NodeDeclarationBuilder& b)
{
    b.add_input<decl::Texture>("tt");
    b.add_output<decl::Texture>("tt");
}

static void node_exec(ExeParams params)
{
}

static void node_register()
{
    static NodeTypeInfo ntype;

    strcpy(ntype.ui_name, "test_socket");
    strcpy(ntype.id_name, "node_test_socket");

    render_node_type_base(&ntype);
    ntype.node_execute = node_exec;
    ntype.declare = node_declare;
    nodeRegisterType(&ntype);
}

NOD_REGISTER_NODE(node_register)
}  // namespace USTC_CG::node_test_socket
