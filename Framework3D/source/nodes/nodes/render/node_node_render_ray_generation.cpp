#include "NODES_FILES_DIR.h"
#include "Nodes/node.hpp"
#include "Nodes/node_declare.hpp"
#include "Nodes/node_register.h"
#include "RCore/Backend.hpp"
#include "Utils/Math/math.h"
#include "nvrhi/utils.h"
#include "render_node_base.h"
#include "resource_allocator_instance.hpp"
#include "shaders/utils/ray.h"
#include "utils/cam_to_view_contants.h"
#include "utils/compile_shader.h"

namespace USTC_CG::node_node_render_ray_generation {
static void node_declare(NodeDeclarationBuilder& b)
{
    b.add_input<decl::Camera>("Camera");
    b.add_input<decl::Texture>("random seeds");
    b.add_output<decl::Buffer>("Rays");
}

static void node_exec(ExeParams params)
{
    Hd_USTC_CG_Camera* free_camera = get_free_camera(params);
    auto size = free_camera->dataWindow.GetSize();

    // 0. Prepare the output buffer
    BufferDesc ray_buffer_desc;
    ray_buffer_desc.byteSize = size[0] * size[1] * sizeof(RayDesc);
    ray_buffer_desc.structStride = sizeof(RayDesc);
    ray_buffer_desc.canHaveUAVs = true;
    ray_buffer_desc.initialState = nvrhi::ResourceStates::UnorderedAccess;
    ray_buffer_desc.keepInitialState = true;
    auto result_rays = resource_allocator.create(ray_buffer_desc);

    // 2. Prepare the shader
    nvrhi::BindingLayoutDescVector binding_layout_desc_vec;
    std::string error_string;
    auto compute_shader = compile_shader(
        "main",
        nvrhi::ShaderType::Compute,
        "shaders/raygen.slang",
        binding_layout_desc_vec,
        error_string);
    MARK_DESTROY_NVRHI_RESOURCE(compute_shader);

    if (!error_string.empty()) {
        resource_allocator.destroy(result_rays);
        throw std::runtime_error(error_string);
    }
    auto binding_layout = resource_allocator.create(binding_layout_desc_vec[0]);
    MARK_DESTROY_NVRHI_RESOURCE(binding_layout);

    auto constant_buffer = resource_allocator.create(
        BufferDesc{ .byteSize = sizeof(PlanarViewConstants),
                    .debugName = "constantBuffer",
                    .isConstantBuffer = true,
                    .initialState = nvrhi::ResourceStates::ConstantBuffer,
                    .cpuAccess = nvrhi::CpuAccessMode::Write });

    MARK_DESTROY_NVRHI_RESOURCE(constant_buffer);

    ComputePipelineDesc pipeline_desc;
    pipeline_desc.CS = compute_shader;
    pipeline_desc.bindingLayouts = { binding_layout };
    auto compute_pipeline = resource_allocator.create(pipeline_desc);
    MARK_DESTROY_NVRHI_RESOURCE(compute_pipeline);

    auto command_list = resource_allocator.create(CommandListDesc{});
    MARK_DESTROY_NVRHI_RESOURCE(command_list);

    auto random_seeds = params.get_input<TextureHandle>("random seeds");

    BindingSetDesc binding_set_desc;
    binding_set_desc.bindings = {
        nvrhi::BindingSetItem::StructuredBuffer_UAV(0, result_rays),
        nvrhi::BindingSetItem::Texture_UAV(1, random_seeds),
        nvrhi::BindingSetItem::ConstantBuffer(0, constant_buffer)
    };
    auto binding_set =
        resource_allocator.create(binding_set_desc, binding_layout.Get());
    MARK_DESTROY_NVRHI_RESOURCE(binding_set);

    command_list->open();
    PlanarViewConstants view_constant = camera_to_view_constants(free_camera);
    command_list->writeBuffer(
        constant_buffer.Get(), &view_constant, sizeof(PlanarViewConstants));
    nvrhi::ComputeState compute_state;
    compute_state.pipeline = compute_pipeline;
    compute_state.addBindingSet(binding_set);
    command_list->setComputeState(compute_state);
    command_list->dispatch(div_ceil(size[0], 8), div_ceil(size[1], 8));
    command_list->close();

    resource_allocator.device->executeCommandList(command_list);
    params.set_output("Rays", result_rays);
}

static void node_register()
{
    static NodeTypeInfo ntype;

    strcpy(ntype.ui_name, "Ray Generation");
    strcpy(ntype.id_name, "node_node_render_ray_generation");

    render_node_type_base(&ntype);
    ntype.node_execute = node_exec;
    ntype.declare = node_declare;
    nodeRegisterType(&ntype);
}

NOD_REGISTER_NODE(node_register)
}  // namespace USTC_CG::node_node_render_ray_generation
