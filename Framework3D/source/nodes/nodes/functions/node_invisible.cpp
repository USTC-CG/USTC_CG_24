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
    b.add_input<decl::Int>("int");
    b.add_output<decl::Float>("float");
}

static void node_exec(ExeParams params)
{
    auto i = params.get_input<int>("int");
    params.set_output("float", float(i));
    std::cout << "The invisible node is being executed!" << std::endl;
}

static void node_register()
{
    static NodeTypeInfo ntype;

    strcpy(ntype.ui_name, "invisible");
    strcpy_s(ntype.id_name, "int_to_float");

    func_node_type_base(&ntype);
    ntype.node_execute = node_exec;
    ntype.declare = node_declare;
    ntype.INVISIBLE = true;
    nodeRegisterType(&ntype);
}

NOD_REGISTER_NODE(node_register)
}  // namespace USTC_CG::node_invisible
