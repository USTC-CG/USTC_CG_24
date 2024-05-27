#ifdef _WIN32
#include <Windows.h>
#include <dxcapi.h>
#endif

#include "NODES_FILES_DIR.h"
#include "Nodes/node.hpp"
#include "Nodes/node_declare.hpp"
#include "Nodes/node_register.h"
#include "RCore/Backend.hpp"
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
    output_desc.initialState = nvrhi::ResourceStates::UnorderedAccess;
    output_desc.keepInitialState = true;
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
    resource_allocator.device->unmapBuffer(vertexBuffer);

    nvrhi::rt::AccelStructDesc accel_struct_desc;
    nvrhi::rt::GeometryDesc blas_desc;
    blas_desc.geometryType = nvrhi::rt::GeometryType::Triangles;
    nvrhi::rt::GeometryTriangles triangles;
    triangles.setVertexBuffer(vertexBuffer)
        .setVertexCount(3)
        .setVertexFormat(nvrhi::Format::RGB32_FLOAT);
    blas_desc.setTriangles(triangles);
    accel_struct_desc.addBottomLevelGeometry(blas_desc);
    AccelStructHandle accel_struct = resource_allocator.create(accel_struct_desc);

    // 2. Prepare the shader

    ShaderCompileDesc shader_compile_desc;
    shader_compile_desc.set_path(
        std::filesystem::path(RENDER_NODES_FILES_DIR) /
        std::filesystem::path("shaders/ray_launch.hlsl"));
    shader_compile_desc.shaderType = nvrhi::ShaderType::AllRayTracing;
    // shader_compile_desc.set_entry_name("ClosestHit");

    auto raytrace_compiled = resource_allocator.create(shader_compile_desc);

    auto m_ShaderLibrary = resource_allocator.device->createShaderLibrary(
        raytrace_compiled->blob->GetBufferPointer(), raytrace_compiled->blob->GetBufferSize());

    // 3. Prepare the hitgroup and pipeline

    nvrhi::BindingLayoutDesc globalBindingLayoutDesc;
    globalBindingLayoutDesc.visibility = nvrhi::ShaderType::All;
    globalBindingLayoutDesc.bindings = { { 0, nvrhi::ResourceType::RayTracingAccelStruct },
                                         { 0, nvrhi::ResourceType::Texture_UAV } };
    auto globalBindingLayout = resource_allocator.create(globalBindingLayoutDesc);

    nvrhi::rt::PipelineDesc pipeline_desc;
    pipeline_desc.maxPayloadSize = 4 * sizeof(float);
    pipeline_desc.globalBindingLayouts = { globalBindingLayout };
    pipeline_desc.shaders = {
        { "", m_ShaderLibrary->getShader("RayGen", nvrhi::ShaderType::RayGeneration), nullptr },
        { "", m_ShaderLibrary->getShader("Miss", nvrhi::ShaderType::Miss), nullptr }
    };

    pipeline_desc.hitGroups = { {
        "HitGroup",
        m_ShaderLibrary->getShader("ClosestHit", nvrhi::ShaderType::ClosestHit),
        nullptr,  // anyHitShader
        nullptr,  // intersectionShader
        nullptr,  // bindingLayout
        false     // isProceduralPrimitive
    } };

    auto raytracing_pipeline = resource_allocator.create(pipeline_desc);
    BindingSetDesc binding_set_desc;
    binding_set_desc.bindings =
        nvrhi::BindingSetItemArray{ nvrhi::BindingSetItem::RayTracingAccelStruct(
                                        0, accel_struct.Get()),
                                    nvrhi::BindingSetItem::Texture_UAV(0, result_texture.Get()) };
    auto binding_set = resource_allocator.create(binding_set_desc, globalBindingLayout.Get());

    auto command_list = resource_allocator.device->createCommandList();

    nvrhi::rt::State state;
    nvrhi::rt::ShaderTableHandle sbt = raytracing_pipeline->createShaderTable();
    sbt->setRayGenerationShader("RayGen");
    sbt->addHitGroup("HitGroup");
    sbt->addMissShader("Miss");
    state.setShaderTable(sbt).addBindingSet(binding_set);

    command_list->open();
    command_list->setRayTracingState(state);
    nvrhi::rt::DispatchRaysArguments args;
    args.width = size[0];
    args.height = size[1];
    command_list->dispatchRays(args);
    command_list->close();
    resource_allocator.device->executeCommandList(command_list);
    resource_allocator.device->waitForIdle();

    resource_allocator.destroy(raytracing_pipeline);
    resource_allocator.destroy(vertexBuffer);
    resource_allocator.destroy(accel_struct);
    resource_allocator.destroy(raytrace_compiled);
    resource_allocator.destroy(globalBindingLayout);
    resource_allocator.destroy(binding_set);

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
