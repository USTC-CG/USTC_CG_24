#include "Nodes/node.hpp"
#include "Nodes/node_declare.hpp"
#include "Nodes/node_register.h"
#include "RCore/Backend.hpp"
#include "render_node_base.h"
#include "resource_allocator_instance.hpp"

namespace USTC_CG::node_scene_ray_launch {
static void node_declare(NodeDeclarationBuilder& b)
{
    b.add_output<decl::Texture>("Result");
}

static void node_exec(ExeParams params)
{
    nvrhi::TextureDesc output_desc;
    auto texture = resource_allocator.create(output_desc);

    params.set_output("Result", texture);
}

static void node_register()
{
    static NodeTypeInfo ntype;

    strcpy(ntype.ui_name, "Ray Launch");
    strcpy_s(ntype.id_name, "render_ray_launch");

    render_node_type_base(&ntype);
    ntype.node_execute = node_exec;
    ntype.ALWAYS_REQUIRED = true;
    ntype.declare = node_declare;
    nodeRegisterType(&ntype);
}

NOD_REGISTER_NODE(node_register)
}  // namespace USTC_CG::node_scene_ray_launch
