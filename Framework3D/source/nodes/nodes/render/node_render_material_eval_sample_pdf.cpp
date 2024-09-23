#include "NODES_FILES_DIR.h"
#include "Nodes/node.hpp"
#include "Nodes/node_declare.hpp"
#include "Nodes/node_register.h"
#include "RCore/Backend.hpp"
#include "Utils/Math/math.h"
#include "nvrhi/utils.h"
#include "render_node_base.h"
#include "resource_allocator_instance.hpp"
#include "shaders/utils/HitObject.h"
#include "utils/compile_shader.h"

namespace USTC_CG::node_render_material_eval_sample_pdf {
static void node_declare(NodeDeclarationBuilder& b)
{
    b.add_input<decl::AccelStruct>("Accel Struct");

    b.add_input<decl::Buffer>("HitInfo");

    b.add_output<decl::Buffer>("PixelTarget");
    b.add_output<decl::Buffer>("Eval");
    b.add_output<decl::Buffer>("Sample");
    b.add_output<decl::Buffer>("Weight");
    b.add_output<decl::Buffer>("Pdf");
}

static void node_exec(ExeParams params)
{
    using namespace nvrhi;

    // 0. Get the 'HitObjectInfos'

    auto hit_info_buffer = params.get_input<BufferHandle>("HitInfo");

    auto length = hit_info_buffer->getDesc().byteSize / sizeof(HitObjectInfo);

    // The Eval, Pixel Target together should be the same size, and should
    // together be able to store the result of the material evaluation

    // 'Sample' should be like {Next RayDesc, Weight}

    // 'Pdf Should be just like float...'

    // Should this be a ray tracing pipeline or compute pipeline? Depending on:
    // do we need the reordering function here?

    // Yes!

    // So it should be a ray tracing pipeline.

    // Build the raytracing pipeline.

    // 2. Prepare the shader

    auto m_CommandList = resource_allocator.create(CommandListDesc{});
    MARK_DESTROY_NVRHI_RESOURCE(m_CommandList);

    ShaderCompileDesc shader_compile_desc;
    shader_compile_desc.set_path(
        std::filesystem::path(RENDER_NODES_FILES_DIR) /
        std::filesystem::path("shaders/material_eval_sample_pdf.slang"));
    shader_compile_desc.shaderType = nvrhi::ShaderType::AllRayTracing;

    auto raytrace_compiled = resource_allocator.create(shader_compile_desc);

    if (raytrace_compiled->get_error_string().empty()) {
        ShaderDesc shader_desc;
        shader_desc.entryName = "RayGen";
        shader_desc.shaderType = nvrhi::ShaderType::RayGeneration;
        shader_desc.debugName = std::to_string(
            reinterpret_cast<long long>(raytrace_compiled->getBufferPointer()));
        auto raygen_shader = resource_allocator.create(
            shader_desc,
            raytrace_compiled->getBufferPointer(),
            raytrace_compiled->getBufferSize());
        MARK_DESTROY_NVRHI_RESOURCE(raygen_shader);

        shader_desc.entryName = "ClosestHit";
        shader_desc.shaderType = nvrhi::ShaderType::ClosestHit;
        auto chs_shader = resource_allocator.create(
            shader_desc,
            raytrace_compiled->getBufferPointer(),
            raytrace_compiled->getBufferSize());
        MARK_DESTROY_NVRHI_RESOURCE(chs_shader);

        shader_desc.entryName = "Miss";
        shader_desc.shaderType = nvrhi::ShaderType::Miss;
        auto miss_shader = resource_allocator.create(
            shader_desc,
            raytrace_compiled->getBufferPointer(),
            raytrace_compiled->getBufferSize());
        MARK_DESTROY_NVRHI_RESOURCE(miss_shader);

        // 3. Prepare the hitgroup and pipeline

        nvrhi::BindingLayoutDesc globalBindingLayoutDesc;
        globalBindingLayoutDesc.visibility = nvrhi::ShaderType::All;
        globalBindingLayoutDesc.bindings = {
            { 0, nvrhi::ResourceType::RayTracingAccelStruct },
            { 0, nvrhi::ResourceType::StructuredBuffer_UAV },
            { 1, nvrhi::ResourceType::Texture_UAV },
            { 2, nvrhi::ResourceType::Texture_UAV },
            { 3, nvrhi::ResourceType::StructuredBuffer_UAV }
        };
        auto globalBindingLayout =
            resource_allocator.create(globalBindingLayoutDesc);
        MARK_DESTROY_NVRHI_RESOURCE(globalBindingLayout);

        nvrhi::rt::PipelineDesc pipeline_desc;
        pipeline_desc.maxPayloadSize = 16 * sizeof(float);
        pipeline_desc.globalBindingLayouts = { globalBindingLayout };
        pipeline_desc.shaders = { { "", raygen_shader, nullptr },
                                  { "", miss_shader, nullptr } };

        pipeline_desc.hitGroups = { {
            "HitGroup",
            chs_shader,
            nullptr,  // anyHitShader
            nullptr,  // intersectionShader
            nullptr,  // bindingLayout
            false     // isProceduralPrimitive
        } };
        auto m_TopLevelAS = params.get_input<AccelStructHandle>("Accel Struct");
        auto raytracing_pipeline = resource_allocator.create(pipeline_desc);
        MARK_DESTROY_NVRHI_RESOURCE(raytracing_pipeline);

        BindingSetDesc binding_set_desc;
        binding_set_desc.bindings = nvrhi::BindingSetItemArray{
            nvrhi::BindingSetItem::RayTracingAccelStruct(0, m_TopLevelAS.Get()),
            nvrhi::BindingSetItem::StructuredBuffer_UAV(
                1, hit_info_buffer.Get()),

        };
        auto binding_set = resource_allocator.create(
            binding_set_desc, globalBindingLayout.Get());
        MARK_DESTROY_NVRHI_RESOURCE(binding_set);

        nvrhi::rt::State state;
        nvrhi::rt::ShaderTableHandle sbt =
            raytracing_pipeline->createShaderTable();
        sbt->setRayGenerationShader("RayGen");
        sbt->addHitGroup("HitGroup");
        sbt->addMissShader("Miss");
        state.setShaderTable(sbt).addBindingSet(binding_set);

        m_CommandList->open();

        m_CommandList->setRayTracingState(state);
        nvrhi::rt::DispatchRaysArguments args;
        args.width = length;
        args.height = 1;
        m_CommandList->dispatchRays(args);
        m_CommandList->close();
        resource_allocator.device->executeCommandList(m_CommandList);
        resource_allocator.device
            ->waitForIdle();  // This is not fully efficient.
    }
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
