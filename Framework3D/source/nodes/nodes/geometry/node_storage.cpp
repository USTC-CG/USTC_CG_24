#include "Nodes/node.hpp"
#include "Nodes/node_declare.hpp"
#include "Nodes/node_register.h"
#include "geom_node_base.h"

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

// Through one execution, how much time is advected? Unit is seconds.
static void node_declare_time_gain(NodeDeclarationBuilder& b)
{
    b.add_input<decl::Float>("time").default_val(0.0333333333f).min(0).max(0.2);
}

static void node_exec_time_gain(ExeParams params)
{
    // This is for external read. Do nothing.
}

// Through one execution, how much time is advected? Unit is seconds.
static void node_declare_time_code(NodeDeclarationBuilder& b)
{
    b.add_output<decl::Float>("time");
}

static void node_exec_time_code(ExeParams params)
{
    // This is for external write. Do nothing.
}

static void node_register()
{
    static NodeTypeInfo storage_in_ntype;

    strcpy(storage_in_ntype.ui_name, "Storage In");
    strcpy_s(storage_in_ntype.id_name, "geom_storage_in");

    geo_node_type_base(&storage_in_ntype);
    storage_in_ntype.node_execute = node_exec_storage_in;
    storage_in_ntype.declare = node_declare_storage_in;
    storage_in_ntype.ALWAYS_REQUIRED = true;
    nodeRegisterType(&storage_in_ntype);

    static NodeTypeInfo storage_out_ntype;

    strcpy(storage_out_ntype.ui_name, "Storage Out");
    strcpy_s(storage_out_ntype.id_name, "geom_storage_out");

    geo_node_type_base(&storage_out_ntype);
    storage_out_ntype.node_execute = node_exec_storage_out;
    storage_out_ntype.declare = node_declare_storage_out;
    nodeRegisterType(&storage_out_ntype);

    static NodeTypeInfo time_gain_ntype;

    strcpy(time_gain_ntype.ui_name, "Time Gain");
    strcpy_s(time_gain_ntype.id_name, "geom_time_gain");

    geo_node_type_base(&time_gain_ntype);
    time_gain_ntype.node_execute = node_exec_time_gain;
    time_gain_ntype.declare = node_declare_time_gain;
    time_gain_ntype.ALWAYS_REQUIRED = true;
    nodeRegisterType(&time_gain_ntype);

    static NodeTypeInfo time_code_ntype;

    strcpy(time_code_ntype.ui_name, "Time Code");
    strcpy_s(time_code_ntype.id_name, "geom_time_code");

    geo_node_type_base(&time_code_ntype);
    time_code_ntype.node_execute = node_exec_time_code;
    time_code_ntype.declare = node_declare_time_code;
    time_code_ntype.ALWAYS_REQUIRED = true;
    nodeRegisterType(&time_code_ntype);
}

NOD_REGISTER_NODE(node_register)
}  // namespace USTC_CG::node_storage
