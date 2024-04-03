#include "Nodes/node.hpp"
#include "Nodes/node_declare.hpp"
#include "Nodes/node_register.h"
#include "comp_node_base.h"

namespace USTC_CG::node_comp_read_usd {
static void node_declare(NodeDeclarationBuilder& b)
{
    b.add_input<decl::String>("File Name").default_val("Default");
    b.add_output<decl::Layer>("Layer");
}

static void node_exec(ExeParams params)
{
    auto file_name = params.get_input<std::string>("File Name");
    auto stage = pxr::UsdStage::Open(file_name.c_str());
    if (!stage) {
        throw std::runtime_error("Stage not found.");
    }
    params.set_output("Layer", stage);
}

static void node_register()
{
    static NodeTypeInfo ntype;

    strcpy(ntype.ui_name, "Read USD");
    strcpy_s(ntype.id_name, "comp_read_usd");

    comp_node_type_base(&ntype);
    ntype.node_execute = node_exec;
    ntype.declare = node_declare;
    nodeRegisterType(&ntype);
}

NOD_REGISTER_NODE(node_register)
}  // namespace USTC_CG::node_comp_read_usd
