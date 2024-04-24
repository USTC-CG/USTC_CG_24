#include "Nodes/node.hpp"
#include "Nodes/node_declare.hpp"
#include "Nodes/node_register.h"
#include "geom_node_base.h"

namespace USTC_CG::node_single_spring {
static void node_declare(NodeDeclarationBuilder& b)
{
    b.add_input<decl::Float>("p_z");
    b.add_input<decl::Float>("v_z");
    b.add_input<decl::Float>("k").default_val(1).min(0.2).max(20);
    b.add_input<decl::Float>("h");
    b.add_input<decl::Float>("time_code");

    b.add_output<decl::Float>("p_z");
    b.add_output<decl::Float>("v_z");
}

static void node_exec(ExeParams params)
{
    auto time_code = params.get_input<float>("time_code");
    auto p_z = params.get_input<float>("p_z");
    auto v_z = params.get_input<float>("v_z");
    auto k = params.get_input<float>("k");
    auto h = params.get_input<float>("h");

    if (time_code == 0) {
        p_z = 1;
        v_z = 0;
        params.set_output("p_z", p_z);
        params.set_output("v_z", v_z);

        return;
    }

    v_z += -h * p_z * k;
    p_z += h * v_z;

    params.set_output("p_z", p_z);
    params.set_output("v_z", v_z);
}

static void node_register()
{
    static NodeTypeInfo ntype;

    strcpy(ntype.ui_name, "Single Spring");
    strcpy_s(ntype.id_name, "geom_single_spring");

    geo_node_type_base(&ntype);
    ntype.node_execute = node_exec;
    ntype.declare = node_declare;
    nodeRegisterType(&ntype);
}

NOD_REGISTER_NODE(node_register)
}  // namespace USTC_CG::node_single_spring
