#include "NODES_FILES_DIR.h"
#include "Nodes/node.hpp"
#include "Nodes/node_declare.hpp"
#include "Nodes/node_register.h"
#include "RCore/Backend.hpp"
#include "nvrhi/utils.h"
#include "render_node_base.h"
#include "resource_allocator_instance.hpp"
#include "utils/python_conversion.hpp"

namespace USTC_CG::node_render_rayleigh_sommerfeld {
static void node_declare(NodeDeclarationBuilder& b)
{
    b.add_input<decl::Texture>("Input");

    b.add_output<decl::Texture>("Output");
}

static void node_exec(ExeParams params)
{
}

static void node_register()
{
    static NodeTypeInfo ntype;

    strcpy(ntype.ui_name, "Rayleigh Sommerfeld");
    strcpy_s(ntype.id_name, "node_render_rayleigh_sommerfeld");

    render_node_type_base(&ntype);
    ntype.node_execute = node_exec;
    ntype.declare = node_declare;
    nodeRegisterType(&ntype);
}

NOD_REGISTER_NODE(node_register)
}  // namespace USTC_CG::node_render_rayleigh_sommerfeld
