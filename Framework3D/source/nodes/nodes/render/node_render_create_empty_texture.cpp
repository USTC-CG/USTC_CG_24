#include "NODES_FILES_DIR.h"
#include "Nodes/node.hpp"
#include "Nodes/node_declare.hpp"
#include "Nodes/node_register.h"
#include "RCore/Backend.hpp"
#include "nvrhi/utils.h"
#include "render_node_base.h"
#include "resource_allocator_instance.hpp"

namespace USTC_CG::node_render_create_empty_texture {
static void node_declare(NodeDeclarationBuilder& b)
{
    b.add_output<decl::Texture>("Texture");
}

static void node_exec(ExeParams params)
{
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
    output_desc.clearValue = nvrhi::Color{ 0, 0, 0, 1 };
    auto output = resource_allocator.create(output_desc);

    auto command_list = resource_allocator.create(CommandListDesc{});
    MARK_DESTROY_NVRHI_RESOURCE(command_list);

    command_list->open();
    command_list->clearTextureFloat(output, {}, nvrhi::Color{ 0, 0, 0, 1 });
    command_list->close();

    resource_allocator.device->executeCommandList(command_list);

    params.set_output("Texture", output);
}

static void node_register()
{
    static NodeTypeInfo ntype;

    strcpy(ntype.ui_name, "render_create_empty_texture");
    strcpy(ntype.id_name, "node_render_create_empty_texture");

    render_node_type_base(&ntype);
    ntype.node_execute = node_exec;
    ntype.declare = node_declare;
    nodeRegisterType(&ntype);
}

NOD_REGISTER_NODE(node_register)
}  // namespace USTC_CG::node_render_create_empty_texture
