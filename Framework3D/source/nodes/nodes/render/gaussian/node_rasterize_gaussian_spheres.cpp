#include <torch/torch.h>

#include "../NODES_FILES_DIR.h"
#include "../render_node_base.h"
#include "../resource_allocator_instance.hpp"
#include "Nodes/node.hpp"
#include "Nodes/node_declare.hpp"
#include "Nodes/node_register.h"
#include "RCore/Backend.hpp"
#include "nvrhi/utils.h"

namespace USTC_CG::node_rasterize_gaussian_spheres {
static void node_declare(NodeDeclarationBuilder& b)
{
    b.add_input<decl::Buffer>("xyz");
    b.add_output<decl::Texture>("Rasterized");
}

namespace bp = boost::python;

static void node_exec(ExeParams params)
{
    auto buffer = params.get_input<nvrhi::BufferHandle>("xyz");

    Hd_USTC_CG_Camera* free_camera =
        params.get_global_params<RenderGlobalParams>().camera;
    auto size = free_camera->dataWindow.GetSize();

    // 0. Prepare the output texture
    nvrhi::TextureDesc output_desc;
    output_desc.width = size[0];
    output_desc.height = size[1];
    output_desc.format = nvrhi::Format::RGBA32_FLOAT;
    output_desc.initialState = nvrhi::ResourceStates::UnorderedAccess;
    output_desc.keepInitialState = true;
    output_desc.isUAV = true;

    auto output = resource_allocator.create(output_desc);
    auto m_CommandList = resource_allocator.create(CommandListDesc{});
    MARK_DESTROY_NVRHI_RESOURCE(m_CommandList);
}

static void node_register()
{
    static NodeTypeInfo ntype;

    strcpy(ntype.ui_name, "rasterize_gaussian_spheres");
    strcpy(ntype.id_name, "node_rasterize_gaussian_spheres");

    render_node_type_base(&ntype);
    ntype.node_execute = node_exec;
    ntype.declare = node_declare;
    ntype.ALWAYS_REQUIRED = true;  // For debug
    nodeRegisterType(&ntype);
}

NOD_REGISTER_NODE(node_register)
}  // namespace USTC_CG::node_rasterize_gaussian_spheres
