#include "NODES_FILES_DIR.h"
#include "Nodes/node.hpp"
#include "Nodes/node_declare.hpp"
#include "Nodes/node_register.h"
#include "RCore/Backend.hpp"
#include "nvrhi/utils.h"
#include "render_node_base.h"
#include "resource_allocator_instance.hpp"

namespace USTC_CG::node_render_material_eval_sample_pdf {
static void node_declare(NodeDeclarationBuilder& b)
{
    b.add_input<decl::Buffer>("HitInfo");
    b.add_output<decl::Buffer>("PixelTarget");
    b.add_output<decl::Buffer>("Eval");
    b.add_output<decl::Buffer>("Sample");
    b.add_output<decl::Buffer>("Pdf");
}

static void node_exec(ExeParams params)
{
}

static void node_register()
{
    static NodeTypeInfo ntype;

    strcpy(ntype.ui_name, "Material Eval");
    strcpy(ntype.id_name, "node_render_material_eval_sample_pdf");

    render_node_type_base(&ntype);
    ntype.node_execute = node_exec;
    ntype.declare = node_declare;
    nodeRegisterType(&ntype);
}

NOD_REGISTER_NODE(node_register)
}  // namespace USTC_CG::node_render_material_eval_sample_pdf
