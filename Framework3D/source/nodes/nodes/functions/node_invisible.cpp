#include "NODES_FILES_DIR.h"
#include "Nodes/node.hpp"
#include "Nodes/node_declare.hpp"
#include "Nodes/node_register.h"
#include "RCore/Backend.hpp"
#include "func_node_base.h"
#include "nvrhi/utils.h"

namespace USTC_CG::node_invisible {
static void node_declare(NodeDeclarationBuilder& b)
{
}

static void node_exec(ExeParams params)
{
    std::cout << "The invisible node is being executed!" << std::endl;
}

static void node_register()
{
    static NodeTypeInfo ntype;

    strcpy(ntype.ui_name, "invisible");
    strcpy_s(ntype.id_name, "node_invisible");

    func_node_type_base(&ntype);
    ntype.node_execute = node_exec;
    ntype.declare = node_declare;
    nodeRegisterType(&ntype);
}

NOD_REGISTER_NODE(node_register)
}  // namespace USTC_CG::node_invisible
