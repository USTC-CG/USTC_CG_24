#include "Nodes/node.hpp"
#include "Nodes/node_declare.hpp"
#include "Nodes/node_register.h"
#include "geom_node_base.h"

namespace USTC_CG::node_time_code {
// Through one execution, how much time is advected? Unit is seconds.
static void node_declare_time_gain(NodeDeclarationBuilder& b)
{
    b.add_input<decl::Float>("time")
        .default_val(0.0333333333f)
        .min(0)
        .max(0.2f);
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
    static NodeTypeInfo time_gain_ntype;

    strcpy(time_gain_ntype.ui_name, "Time Gain");
    strcpy(time_gain_ntype.id_name, "geom_time_gain");

    geo_node_type_base(&time_gain_ntype);
    time_gain_ntype.node_execute = node_exec_time_gain;
    time_gain_ntype.declare = node_declare_time_gain;
    time_gain_ntype.ALWAYS_REQUIRED = true;
    nodeRegisterType(&time_gain_ntype);

    static NodeTypeInfo time_code_ntype;

    strcpy(time_code_ntype.ui_name, "Time Code");
    strcpy(time_code_ntype.id_name, "geom_time_code");

    geo_node_type_base(&time_code_ntype);
    time_code_ntype.node_execute = node_exec_time_code;
    time_code_ntype.declare = node_declare_time_code;
    time_code_ntype.ALWAYS_REQUIRED = true;
    nodeRegisterType(&time_code_ntype);
}

NOD_REGISTER_NODE(node_register)
}  // namespace USTC_CG::node_time_code
