#include "Nodes/node.hpp"
#include "Nodes/node_declare.hpp"
#include "Nodes/node_register.h"
#include "RCore/Backend.hpp"
#include "nvrhi/utils.h"
#include "render_node_base.h"
#include "resource_allocator_instance.hpp"

namespace USTC_CG::node_scene_ray_launch {
static void node_declare(NodeDeclarationBuilder& b)
{
    b.add_input<decl::Camera>("Camera");
    b.add_output<decl::Texture>("Result");
}

float triangle_corner[9] = { -0.6, -0.3, 0, 0.6, -0.3, 0, 0, 0.3, 0 };

static void node_exec(ExeParams params)
{
    Hd_USTC_CG_Camera* free_camera = get_free_camera(params);
    auto size = free_camera->_dataWindow.GetSize();

    // 0. Prepare the output texture
    nvrhi::TextureDesc output_desc;
    output_desc.width = size[0];
    output_desc.height = size[1];
    output_desc.format = nvrhi::Format::RGBA32_FLOAT;
    output_desc.isUAV = true;

    auto result_texture = resource_allocator.create(output_desc);

    // 1. Build the acceleration struct. You should do this in the 'Hd_USTC_CG_Mesh' sync, and don't
    // try to use resource_allocator. Use things like device->create... Directly. Note that there
    // are BLAS and TLAS. Get familiar with these ideas first.
    BufferDesc buffer_desc;

    buffer_desc.byteSize = 3 * 3 * sizeof(float);
    buffer_desc.format = nvrhi::Format::RGB32_FLOAT;
    buffer_desc.isAccelStructBuildInput = true;
    buffer_desc.cpuAccess = nvrhi::CpuAccessMode::Write;
    auto vertexBuffer = resource_allocator.create(buffer_desc);

    auto buffer = resource_allocator.device->mapBuffer(vertexBuffer, nvrhi::CpuAccessMode::Write);
    memcpy(buffer, triangle_corner, 3 * 3 * sizeof(float));

    nvrhi::rt::AccelStructDesc accel_struct_desc;
    nvrhi::rt::GeometryDesc blas_desc;
    blas_desc.geometryType = nvrhi::rt::GeometryType::Triangles;
    nvrhi::rt::GeometryTriangles triangles;
    triangles.setVertexBuffer(vertexBuffer)
        .setVertexCount(3)
        .setVertexFormat(nvrhi::Format::RGB32_FLOAT);
    blas_desc.setTriangles(triangles);
    accel_struct_desc.addBottomLevelGeometry(blas_desc);
    auto accel_struct = resource_allocator.create(accel_struct_desc);

    BindingLayoutDesc binding_layout_desc;
    auto binding_layout = resource_allocator.create(binding_layout_desc);

    BindingSetDesc binding_set_desc;
    auto binding_set = resource_allocator.create(binding_set_desc, binding_layout.Get());

    nvrhi::ShaderDesc chs_desc;
    chs_desc.entryName = "__chs__triangle";
    auto chs_shader = resource_allocator.create(chs_desc, nullptr, 0);

    nvrhi::rt::PipelineDesc pipeline_desc;
    nvrhi::rt::PipelineHitGroupDesc hitgroup_desc;
    hitgroup_desc.closestHitShader = chs_shader;
    pipeline_desc.addHitGroup(hitgroup_desc);

    auto raytracing_pipeline = resource_allocator.create(pipeline_desc);

    auto command_list = resource_allocator.device->createCommandList();

    nvrhi::rt::State state;
    command_list->setRayTracingState(state);

    command_list->open();
    command_list->clearTextureFloat(result_texture, {}, nvrhi::Color{ 1, 0, 1, 1 });
    command_list->close();
    resource_allocator.device->executeCommandList(command_list.Get());

    resource_allocator.destroy(raytracing_pipeline);
    params.set_output("Result", result_texture);
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
