#ifdef _WIN32
#include <Windows.h>
#include <dxcapi.h>
#endif

#include "NODES_FILES_DIR.h"
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

float triangle_corner[9] = { -0.6, -0.3, 0, 0.6, -0.3, 0, 0, 0.6, 0 };

unsigned indices[3] = { 0, 1, 2 };

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

    buffer_desc.byteSize = 3 * sizeof(unsigned);
    buffer_desc.format = nvrhi::Format ::R32_UINT;
    auto indexBuffer = resource_allocator.create(buffer_desc);

    auto buffer = resource_allocator.device->mapBuffer(vertexBuffer, nvrhi::CpuAccessMode::Write);
    memcpy(buffer, triangle_corner, 3 * 3 * sizeof(float));
    resource_allocator.device->unmapBuffer(vertexBuffer);

    buffer = resource_allocator.device->mapBuffer(indexBuffer, nvrhi::CpuAccessMode::Write);
    memcpy(buffer, indices, 3 * sizeof(unsigned));
    resource_allocator.device->unmapBuffer(indexBuffer);

    nvrhi::rt::AccelStructDesc blas_desc;
    nvrhi::rt::GeometryDesc geometry_desc;
    geometry_desc.geometryType = nvrhi::rt::GeometryType::Triangles;
    nvrhi::rt::GeometryTriangles triangles;
    triangles.setVertexBuffer(vertexBuffer)
        .setIndexBuffer(indexBuffer)
        .setIndexCount(3)
        .setVertexCount(3)
        .setVertexStride(3 * sizeof(float))
        .setVertexFormat(nvrhi::Format::RGB32_FLOAT)
        .setIndexFormat(nvrhi::Format::R32_UINT);
    geometry_desc.setTriangles(triangles);
    blas_desc.addBottomLevelGeometry(geometry_desc);
    blas_desc.isTopLevel = false;
    AccelStructHandle blas = resource_allocator.create(blas_desc);

    nvrhi::rt::AccelStructDesc tlasDesc;
    tlasDesc.isTopLevel = true;
    tlasDesc.topLevelMaxInstances = 1;

    auto m_TopLevelAS = resource_allocator.create(tlasDesc);

    nvrhi::rt::InstanceDesc instanceDesc;
    instanceDesc.bottomLevelAS = blas;
    instanceDesc.instanceMask = 1;
    instanceDesc.flags = nvrhi::rt::InstanceFlags::TriangleFrontCounterclockwise;
    float transform[12] = { 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0 };
    memcpy(instanceDesc.transform, &transform, sizeof(transform));

    auto m_CommandList = resource_allocator.create(CommandListDesc{});
    m_CommandList->open();
    nvrhi::utils::BuildBottomLevelAccelStruct(m_CommandList, blas, blas_desc);
    m_CommandList->buildTopLevelAccelStruct(m_TopLevelAS, &instanceDesc, 1);

    m_CommandList->close();
    resource_allocator.device->executeCommandList(m_CommandList);

    // 2. Prepare the shader

    ShaderCompileDesc shader_compile_desc;
    shader_compile_desc.set_path(
        std::filesystem::path(RENDER_NODES_FILES_DIR) /
        std::filesystem::path("shaders/ray_launch.hlsl"));
    shader_compile_desc.shaderType = nvrhi::ShaderType::AllRayTracing;
    // shader_compile_desc.set_entry_name("ClosestHit");

    auto raytrace_compiled = resource_allocator.create(shader_compile_desc);

    ShaderDesc shader_desc;
    shader_desc.entryName = "RayGen";
    shader_desc.shaderType = nvrhi::ShaderType::RayGeneration;
    shader_desc.debugName =
        std::to_string(reinterpret_cast<long long>(raytrace_compiled->blob->GetBufferPointer()));

    if (raytrace_compiled->blob->GetBufferSize()) {
        auto raygen_shader = resource_allocator.create(
            shader_desc,
            raytrace_compiled->blob->GetBufferPointer(),
            raytrace_compiled->blob->GetBufferSize());

        shader_desc.entryName = "ClosestHit";
        shader_desc.shaderType = nvrhi::ShaderType::ClosestHit;
        auto chs_shader = resource_allocator.create(
            shader_desc,
            raytrace_compiled->blob->GetBufferPointer(),
            raytrace_compiled->blob->GetBufferSize());

        shader_desc.entryName = "Miss";
        shader_desc.shaderType = nvrhi::ShaderType::Miss;
        auto miss_shader = resource_allocator.create(
            shader_desc,
            raytrace_compiled->blob->GetBufferPointer(),
            raytrace_compiled->blob->GetBufferSize());

        // 3. Prepare the hitgroup and pipeline

        nvrhi::BindingLayoutDesc globalBindingLayoutDesc;
        globalBindingLayoutDesc.visibility = nvrhi::ShaderType::All;
        globalBindingLayoutDesc.bindings = { { 0, nvrhi::ResourceType::RayTracingAccelStruct },
                                             { 0, nvrhi::ResourceType::Texture_UAV } };
        auto globalBindingLayout = resource_allocator.create(globalBindingLayoutDesc);

        nvrhi::rt::PipelineDesc pipeline_desc;
        pipeline_desc.maxPayloadSize = 4 * sizeof(float);
        pipeline_desc.globalBindingLayouts = { globalBindingLayout };
        pipeline_desc.shaders = { { "", raygen_shader, nullptr }, { "", miss_shader, nullptr } };

        pipeline_desc.hitGroups = { {
            "HitGroup",
            chs_shader,
            nullptr,  // anyHitShader
            nullptr,  // intersectionShader
            nullptr,  // bindingLayout
            false     // isProceduralPrimitive
        } };

        auto raytracing_pipeline = resource_allocator.create(pipeline_desc);
        BindingSetDesc binding_set_desc;
        binding_set_desc.bindings = nvrhi::BindingSetItemArray{
            nvrhi::BindingSetItem::RayTracingAccelStruct(0, m_TopLevelAS.Get()),
            nvrhi::BindingSetItem::Texture_UAV(0, result_texture.Get())
        };
        auto binding_set = resource_allocator.create(binding_set_desc, globalBindingLayout.Get());

        auto command_list = resource_allocator.create(CommandListDesc{});

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
        resource_allocator.device->waitForIdle();  // This is not fully efficient.

        resource_allocator.destroy(raytracing_pipeline);
        resource_allocator.destroy(globalBindingLayout);
        resource_allocator.destroy(binding_set);
        resource_allocator.destroy(raygen_shader);
        resource_allocator.destroy(chs_shader);
        resource_allocator.destroy(miss_shader);
        resource_allocator.destroy(command_list);
    }
    resource_allocator.destroy(vertexBuffer);
    resource_allocator.destroy(indexBuffer);
    resource_allocator.destroy(blas);
    resource_allocator.destroy(m_TopLevelAS);
    resource_allocator.destroy(m_CommandList);
    auto error = raytrace_compiled->error_string;
    auto buffer_size = raytrace_compiled->blob->GetBufferSize();
    resource_allocator.destroy(raytrace_compiled);

    params.set_output("Result", result_texture);
    if (!buffer_size && error.size()) {
        throw std::runtime_error(error);
    }
}

static void node_register()
{
    static NodeTypeInfo ntype;

    strcpy(ntype.ui_name, "Ray Launch");
    strcpy_s(ntype.id_name, "render_ray_launch");

    render_node_type_base(&ntype);
    ntype.node_execute = node_exec;
    ntype.declare = node_declare;
    nodeRegisterType(&ntype);
}

NOD_REGISTER_NODE(node_register)
}  // namespace USTC_CG::node_scene_ray_launch
