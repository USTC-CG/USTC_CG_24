#include "Nodes/node.hpp"
#include "Nodes/node_declare.hpp"
#include "Nodes/node_register.h"
#include "func_node_base.h"

namespace USTC_CG::node_iteration {
static void node_declare_iteration_end(NodeDeclarationBuilder& b)
{
    b.add_input<decl::String>("Name").default_val("Iteration");
    b.add_input<decl::Any>("iteration");
}

static void node_exec_iteration_end(ExeParams params)
{
}

static void node_declare_iteration_begin(NodeDeclarationBuilder& b)
{
    b.add_input<decl::String>("Name").default_val("Iteration");
    b.add_output<decl::Any>("iteration");
}

static void node_exec_iteration_begin(ExeParams params)
{
}

static void node_register()
{
    static NodeTypeInfo iteration_end_ntype;

    strcpy(iteration_end_ntype.ui_name, "Iteration End");
    strcpy(iteration_end_ntype.id_name, "func_iteration_end");

    func_node_type_base(&iteration_end_ntype);
    iteration_end_ntype.node_execute = node_exec_iteration_end;
    iteration_end_ntype.declare = node_declare_iteration_end;
    iteration_end_ntype.ALWAYS_REQUIRED = true;
    nodeRegisterType(&iteration_end_ntype);

    static NodeTypeInfo iteration_begin_ntype;

    strcpy(iteration_begin_ntype.ui_name, "Iteration Begin");
    strcpy(iteration_begin_ntype.id_name, "func_iteration_begin");

    func_node_type_base(&iteration_begin_ntype);
    iteration_begin_ntype.node_execute = node_exec_iteration_begin;
    iteration_begin_ntype.declare = node_declare_iteration_begin;
    nodeRegisterType(&iteration_begin_ntype);
}

NOD_REGISTER_NODE(node_register)
}  // namespace USTC_CG::node_iteration
