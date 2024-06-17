#include "Nodes/node.hpp"
#include "Nodes/node_declare.hpp"
#include "Nodes/node_register.h"
#include "Utils/Math/math.h"
#include "nvrhi/utils.h"
#include "render_node_base.h"
#include "resource_allocator_instance.hpp"
#include "utils/compile_shader.h"

namespace USTC_CG::node_render_rng_texture {
struct RNGStorage {
    nvrhi::TextureHandle random_number = nullptr;
};

// This texture is for repeated read and write.
static void node_declare(NodeDeclarationBuilder& b)
{
    b.add_input<decl::Camera>("Camera");
    b.add_output<decl::Texture>("Random Number");
    b.add_storage<RNGStorage>();
}

static void node_exec(ExeParams params)
{
    Hd_USTC_CG_Camera* free_camera = get_free_camera(params);
    auto size = free_camera->_dataWindow.GetSize();

    TextureDesc output_desc;
    output_desc.debugName = "Random Number Texture";
    output_desc.width = size[0];
    output_desc.height = size[1];
    output_desc.format = nvrhi::Format::RGBA32_UINT;
    output_desc.initialState = nvrhi::ResourceStates::UnorderedAccess;
    output_desc.keepInitialState = true;
    output_desc.isUAV = true;

    auto random = resource_allocator.create(output_desc);

    bool first_attempt;
    auto& stored_rng = params.get_runtime_storage<RNGStorage&>(first_attempt);
    if (stored_rng.random_number != random.Get())
        stored_rng.random_number = random.Get();

    auto device = resource_allocator.device;
    std::string entryName = "main";

    nvrhi::BindingLayoutDescVector binding_layout_descs;
    std::string error_string;

    auto compute_shader = compile_shader(
        entryName,
        nvrhi::ShaderType::Compute,
        "shaders/random_init.slang",
        binding_layout_descs,
        error_string);
    if (!compute_shader) {
        throw std::runtime_error(error_string);
    }

    nvrhi::BindingLayoutVector binding_layouts;
    binding_layouts.resize(binding_layout_descs.size());

    for (int i = 0; i < binding_layout_descs.size(); ++i) {
        binding_layouts[i] = resource_allocator.create(binding_layout_descs[i]);
    }

    nvrhi::ComputePipelineDesc pipeline_desc;
    pipeline_desc.CS = compute_shader;
    pipeline_desc.bindingLayouts = binding_layouts;

    auto compute_pipeline = resource_allocator.create(pipeline_desc);

    auto command_list = resource_allocator.create(CommandListDesc{});

    BindingSetDesc binding_set_desc = { { nvrhi::BindingSetItem::Texture_UAV(
        0, stored_rng.random_number) } };

    auto binding_set_0 =
        resource_allocator.create(binding_set_desc, binding_layouts[0]);

    command_list->open();

    nvrhi::ComputeState compute_state;
    compute_state.pipeline = compute_pipeline;
    compute_state.bindings = { binding_set_0 };

    command_list->setComputeState(compute_state);

    auto texture_info = stored_rng.random_number->getDesc();
    command_list->dispatch(
        div_ceil(texture_info.width, 16), div_ceil(texture_info.height, 16));
    command_list->close();
    resource_allocator.device->executeCommandList(command_list);

    resource_allocator.destroy(compute_shader);
    for (int i = 0; i < binding_layouts.size(); ++i) {
        resource_allocator.destroy(binding_layouts[0]);
    }
    resource_allocator.destroy(binding_set_0);
    resource_allocator.destroy(compute_pipeline);
    resource_allocator.destroy(command_list);

    params.set_output("Random Number", stored_rng.random_number);
}

static void node_register()
{
    static NodeTypeInfo ntype;

    strcpy(ntype.ui_name, "Random Number Texture");
    strcpy(ntype.id_name, "node_render_rng_texture");

    render_node_type_base(&ntype);
    ntype.node_execute = node_exec;
    ntype.declare = node_declare;
    nodeRegisterType(&ntype);
}

NOD_REGISTER_NODE(node_register)
}  // namespace USTC_CG::node_render_rng_texture
