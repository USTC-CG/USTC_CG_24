#include "Nodes/node.hpp"
#include "Nodes/node_declare.hpp"
#include "Nodes/node_register.h"
#include "func_node_base.h"

namespace USTC_CG::node_storage {
static void node_declare_storage_in(NodeDeclarationBuilder& b)
{
    b.add_input<decl::String>("Name").default_val("Storage");
    b.add_input<decl::Any>("storage");
}

static void node_exec_storage_in(ExeParams params)
{
}

static void node_declare_storage_out(NodeDeclarationBuilder& b)
{
    b.add_input<decl::String>("Name").default_val("Storage");
    b.add_output<decl::Any>("storage");
}

static void node_exec_storage_out(ExeParams params)
{
}

static void node_register()
{
    static NodeTypeInfo storage_in_ntype;

    strcpy(storage_in_ntype.ui_name, "Storage In");
    strcpy(storage_in_ntype.id_name, "func_storage_in");

    func_node_type_base(&storage_in_ntype);
    storage_in_ntype.node_execute = node_exec_storage_in;
    storage_in_ntype.declare = node_declare_storage_in;
    storage_in_ntype.ALWAYS_REQUIRED = true;
    nodeRegisterType(&storage_in_ntype);

    static NodeTypeInfo storage_out_ntype;

    strcpy(storage_out_ntype.ui_name, "Storage Out");
    strcpy(storage_out_ntype.id_name, "func_storage_out");

    func_node_type_base(&storage_out_ntype);
    storage_out_ntype.node_execute = node_exec_storage_out;
    storage_out_ntype.declare = node_declare_storage_out;
    nodeRegisterType(&storage_out_ntype);
}

NOD_REGISTER_NODE(node_register)
}  // namespace USTC_CG::node_storage
