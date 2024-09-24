#include "NODES_FILES_DIR.h"
#include "Nodes/node.hpp"
#include "Nodes/node_declare.hpp"
#include "Nodes/node_register.h"
#include "RCore/Backend.hpp"
#include "Utils/Math/math.h"
#include "nvrhi/utils.h"
#include "render_node_base.h"
#include "resource_allocator_instance.hpp"
#include "shaders/utils/cpp_shader_macro.h"
#include "utils/compile_shader.h"

namespace USTC_CG::node_render_scatter_contribution {
static void node_declare(NodeDeclarationBuilder& b)
{
    b.add_input<decl::Buffer>("PixelTarget");
    b.add_input<decl::Buffer>("Eval");
    b.add_input<decl::Int>("Buffer Size");
    b.add_input<decl::Texture>("Source Texture");

    b.add_output<decl::Texture>("Result Texture");
}

static void node_exec(ExeParams params)
{
    using namespace nvrhi;

    auto pixel_target_buffer = params.get_input<BufferHandle>("PixelTarget");
    auto eval_buffer = params.get_input<BufferHandle>("Eval");
    auto source_texture = params.get_input<TextureHandle>("Source Texture");
    auto length = params.get_input<int>("Buffer Size");

    std::string error_string;
    nvrhi::BindingLayoutDescVector binding_layout_desc;
    auto compute_shader = compile_shader(
        "main",
        ShaderType::Compute,
        "shaders/scatter.slang",
        binding_layout_desc,
        error_string,
        {},
        false);
    MARK_DESTROY_NVRHI_RESOURCE(compute_shader);

    // Constant buffer contains the size of the length (single float), and I can
    // write if from CPU
    auto cb_desc = BufferDesc{}
                       .setByteSize(sizeof(float))
                       .setInitialState(ResourceStates::CopyDest)
                       .setKeepInitialState(true)
                       .setCpuAccess(CpuAccessMode::Write)
                       .setIsConstantBuffer(true);

    auto cb = resource_allocator.create(cb_desc);
    MARK_DESTROY_NVRHI_RESOURCE(cb);

    auto binding_layout = resource_allocator.create(binding_layout_desc[0]);
    MARK_DESTROY_NVRHI_RESOURCE(binding_layout);

    // BindingSet and BindingSetLayout
    BindingSetDesc binding_set_desc;
    binding_set_desc.bindings = {
        nvrhi::BindingSetItem::StructuredBuffer_SRV(0, eval_buffer),
        nvrhi::BindingSetItem::StructuredBuffer_SRV(1, pixel_target_buffer),
        nvrhi::BindingSetItem::ConstantBuffer(0, cb),
        nvrhi::BindingSetItem::Texture_UAV(0, source_texture)
    };

    auto binding_set =
        resource_allocator.create(binding_set_desc, binding_layout.Get());
    MARK_DESTROY_NVRHI_RESOURCE(binding_set);
    if (!binding_set) {
        // Handle error
        return;
    }
    // Execute the shader
    ComputePipelineDesc pipeline_desc;
    pipeline_desc.CS = compute_shader;
    pipeline_desc.bindingLayouts = { binding_layout };
    auto pipeline = resource_allocator.create(pipeline_desc);

    MARK_DESTROY_NVRHI_RESOURCE(pipeline);
    if (!pipeline) {
        // Handle error
        return;
    }

    ComputeState compute_state;
    compute_state.pipeline = pipeline;
    compute_state.bindings = { binding_set };

    CommandListHandle command_list =
        resource_allocator.create(CommandListDesc{});
    MARK_DESTROY_NVRHI_RESOURCE(command_list);

    command_list->open();
    command_list->writeBuffer(cb, &length, sizeof(length));
    command_list->setComputeState(compute_state);
    command_list->dispatch(div_ceil(length, 64), 1, 1);
    command_list->close();
    resource_allocator.device->executeCommandList(command_list);

    params.set_output("Result Texture", source_texture);
}

static void node_register()
{
    static NodeTypeInfo ntype;

    strcpy(ntype.ui_name, "render_scatter_contribution");
    strcpy(ntype.id_name, "node_render_scatter_contribution");

    render_node_type_base(&ntype);
    ntype.node_execute = node_exec;
    ntype.declare = node_declare;
    nodeRegisterType(&ntype);
}

NOD_REGISTER_NODE(node_register)
}  // namespace USTC_CG::node_render_scatter_contribution
