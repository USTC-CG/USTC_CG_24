#include "Nodes/node.hpp"
#include "Nodes/node_declare.hpp"
#include "Nodes/node_register.h"
#include "func_node_base.h"
namespace USTC_CG::node_value {
static void node_declare_int(NodeDeclarationBuilder& b)
{
    b.add_input<decl::Int>("value").min(0).max(10).default_val(1);
    b.add_output<decl::Int>("value");
}

static void node_exec_int(ExeParams params)
{
    auto val = params.get_input<int>("value");
    params.set_output("value", val);
}

static void node_declare_float(NodeDeclarationBuilder& b)
{
    b.add_input<decl::Float>("value").min(0).max(10).default_val(1);
    b.add_output<decl::Float>("value");
}

static void node_exec_float(ExeParams params)
{
    auto val = params.get_input<float>("value");
    params.set_output("value", val);
}

static void node_register()
{
    static NodeTypeInfo ntype_value_int;
    strcpy(ntype_value_int.ui_name, "Int Value");
    strcpy_s(ntype_value_int.id_name, "func_value_int");
    func_node_type_base(&ntype_value_int);
    ntype_value_int.node_execute = node_exec_int;
    ntype_value_int.declare = node_declare_int;
    nodeRegisterType(&ntype_value_int);

    static NodeTypeInfo ntype_value_float;
    strcpy(ntype_value_float.ui_name, "Float Value");
    strcpy_s(ntype_value_float.id_name, "func_value_float");
    func_node_type_base(&ntype_value_float);
    ntype_value_float.node_execute = node_exec_float;
    ntype_value_float.declare = node_declare_float;
    nodeRegisterType(&ntype_value_float);
}

NOD_REGISTER_NODE(node_register)
}  // namespace USTC_CG::node_value
