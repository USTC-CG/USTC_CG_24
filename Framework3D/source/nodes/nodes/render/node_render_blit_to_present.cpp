#include "Nodes/node.hpp"
#include "Nodes/node_declare.hpp"
#include "Nodes/node_register.h"
#include "render_node_base.h"
#include "resource_allocator_instance.hpp"
#include "shaders/utils/blit_cb.h"
#include "utils/compile_shader.h"

namespace USTC_CG::node_render_blit_to_present {

enum class BlitSampler { Point, Linear, Sharpen };

struct BlitParameters {
    void deserialize(const nlohmann::json& info)
    {
    }

    nvrhi::Viewport targetViewport;

    pxr::GfRange2f targetBox = pxr::GfRange2f({ 0.f, 0.f }, { 1.f, 1.f });

    uint32_t sourceArraySlice = 0;
    uint32_t sourceMip = 0;
    pxr::GfRange2f sourceBox = pxr::GfRange2f({ 0.f, 0.f }, { 1.f, 1.f });

    BlitSampler sampler = BlitSampler::Linear;
    nvrhi::BlendState::RenderTarget blendState;
    nvrhi::Color blendConstantColor = nvrhi::Color(0.f);
};

static void node_declare(NodeDeclarationBuilder& b)
{
    b.add_input<decl::Texture>("Tex");
    b.add_output<decl::Texture>("Tex");
    b.add_storage<BlitParameters>();
}

static bool IsSupportedBlitDimension(nvrhi::TextureDimension dimension)
{
    return dimension == nvrhi::TextureDimension::Texture2D ||
           dimension == nvrhi::TextureDimension::Texture2DArray ||
           dimension == nvrhi::TextureDimension::TextureCube ||
           dimension == nvrhi::TextureDimension::TextureCubeArray;
}

static bool IsTextureArray(nvrhi::TextureDimension dimension)
{
    return dimension == nvrhi::TextureDimension::Texture2DArray ||
           dimension == nvrhi::TextureDimension::TextureCube ||
           dimension == nvrhi::TextureDimension::TextureCubeArray;
}



static void node_exec(ExeParams params)
{
    auto sourceTexture = params.get_input<TextureHandle>("Tex");
    auto output_desc = sourceTexture->getDesc();
    output_desc.format = nvrhi::Format::RGBA32_FLOAT;
    output_desc.isRenderTarget = true;
    auto output = resource_allocator.create(output_desc);
    MARK_DESTROY_NVRHI_RESOURCE(output);

    auto& blit_parameters = params.get_storage<BlitParameters&>();

    auto commandList = resource_allocator.create(CommandListDesc{});
    MARK_DESTROY_NVRHI_RESOURCE(commandList);

    assert(commandList);

    auto framebuffer_desc = FramebufferDesc{};

    framebuffer_desc.colorAttachments.push_back(
        nvrhi::FramebufferAttachment{ output.Get() });

    auto targetFramebuffer = resource_allocator.create(framebuffer_desc);
    MARK_DESTROY_NVRHI_RESOURCE(targetFramebuffer);

    const nvrhi::FramebufferDesc& targetFramebufferDesc =
        targetFramebuffer->getDesc();
    assert(targetFramebufferDesc.colorAttachments.size() == 1);
    assert(targetFramebufferDesc.colorAttachments[0].valid());
    assert(!targetFramebufferDesc.depthAttachment.valid());

    const nvrhi::FramebufferInfoEx& fbinfo =
        targetFramebuffer->getFramebufferInfo();
    const nvrhi::TextureDesc& sourceDesc = sourceTexture->getDesc();

    assert(IsSupportedBlitDimension(sourceDesc.dimension));
    bool isTextureArray = IsTextureArray(sourceDesc.dimension);

    nvrhi::Viewport targetViewport = blit_parameters.targetViewport;
    if (targetViewport.width() == 0 && targetViewport.height() == 0) {
        // If no viewport is specified, create one based on the framebuffer
        // dimensions. Note that the FB dimensions may not be the same as target
        // texture dimensions, in case a non-zero mip level is used.
        targetViewport =
            nvrhi::Viewport(float(fbinfo.width), float(fbinfo.height));
    }

    std::vector macro_defines = { ShaderMacro("TEXTURE_ARRAY", "0") };

    std::string vs_name = "rect_vs.hlsl";
    std::string ps_name = "blit_ps.hlsl";

    nvrhi::BindingLayoutDescVector vs_binding_layout_descs;
    std::string error_string;

    auto vertex_shader = compile_shader(
        "main",
        nvrhi::ShaderType::Vertex,
        "shaders/utils/" + vs_name,
        vs_binding_layout_descs,
        error_string,
        macro_defines);
    MARK_DESTROY_NVRHI_RESOURCE(vertex_shader);

    nvrhi::BindingLayoutDescVector ps_binding_layout_descs;

    auto pixel_shader = compile_shader(
        "main",
        nvrhi::ShaderType::Pixel,
        "shaders/utils/" + ps_name,
        ps_binding_layout_descs,
        error_string,
        macro_defines);
    MARK_DESTROY_NVRHI_RESOURCE(pixel_shader);

    nvrhi::BindingLayoutDescVector binding_layout_descs =
        mergeBindingLayoutDescVectors(
            vs_binding_layout_descs, ps_binding_layout_descs);

    auto samplerDesc =
        nvrhi::SamplerDesc().setAllFilters(false).setAllAddressModes(
            nvrhi::SamplerAddressMode::Clamp);
    auto m_PointClampSampler = resource_allocator.create(samplerDesc);
    MARK_DESTROY_NVRHI_RESOURCE(m_PointClampSampler);

    samplerDesc.setAllFilters(true);
    auto m_LinearClampSampler = resource_allocator.create(samplerDesc);
    MARK_DESTROY_NVRHI_RESOURCE(m_LinearClampSampler);

    auto binding_layout = resource_allocator.create(binding_layout_descs[0]);
    MARK_DESTROY_NVRHI_RESOURCE(binding_layout);

    nvrhi::GraphicsPipelineDesc psoDesc;
    psoDesc.bindingLayouts = { binding_layout };
    psoDesc.VS = vertex_shader;
    psoDesc.PS = pixel_shader;
    psoDesc.primType = nvrhi::PrimitiveType::TriangleStrip;
    psoDesc.renderState.rasterState.setCullNone();
    psoDesc.renderState.depthStencilState.depthTestEnable = false;
    psoDesc.renderState.depthStencilState.stencilEnable = false;
    psoDesc.renderState.blendState.targets[0] = blit_parameters.blendState;

    auto constant_buffer = resource_allocator.create(
        BufferDesc{ .byteSize = sizeof(BlitParameters),
                    .debugName = "BlitParameters",
                    .isConstantBuffer = true,
                    .initialState = nvrhi::ResourceStates::ConstantBuffer,
                    .cpuAccess = nvrhi::CpuAccessMode::Write });
    MARK_DESTROY_NVRHI_RESOURCE(constant_buffer);

    auto pso = resource_allocator.create(psoDesc, targetFramebuffer);
    MARK_DESTROY_NVRHI_RESOURCE(pso);

    BindingSetDesc binding_set_desc;
    {
        auto sourceDimension = sourceDesc.dimension;
        if (sourceDimension == nvrhi::TextureDimension::TextureCube ||
            sourceDimension == nvrhi::TextureDimension::TextureCubeArray)
            sourceDimension = nvrhi::TextureDimension::Texture2DArray;

        auto sourceSubresources = nvrhi::TextureSubresourceSet(
            blit_parameters.sourceMip, 1, blit_parameters.sourceArraySlice, 1);

        binding_set_desc.bindings = {
            nvrhi::BindingSetItem::ConstantBuffer(0, constant_buffer),
            nvrhi::BindingSetItem::Texture_SRV(
                0,
                sourceTexture,
                sourceTexture->getDesc().format,
                sourceSubresources,
                sourceDimension),
            nvrhi::BindingSetItem::Sampler(
                0,
                blit_parameters.sampler == BlitSampler::Point
                    ? m_PointClampSampler
                    : m_LinearClampSampler)
        };
    }

    auto sourceBindingSet =
        resource_allocator.create(binding_set_desc, binding_layout.Get());
    MARK_DESTROY_NVRHI_RESOURCE(sourceBindingSet);

    nvrhi::GraphicsState state;
    state.pipeline = pso;
    state.framebuffer = targetFramebuffer;
    state.bindings = { sourceBindingSet };
    state.viewport.addViewport(targetViewport);
    state.viewport.addScissorRect(nvrhi::Rect(targetViewport));

    BlitConstants blitConstants = {};
    blitConstants.sourceOrigin = blit_parameters.sourceBox.GetMin();
    blitConstants.sourceSize = blit_parameters.sourceBox.GetSize();
    blitConstants.targetOrigin = blit_parameters.targetBox.GetMin();
    blitConstants.targetSize = blit_parameters.targetBox.GetSize();

    commandList->open();

    commandList->setGraphicsState(state);

    commandList->writeBuffer(
        constant_buffer.Get(), &blitConstants, sizeof(blitConstants));

    nvrhi::DrawArguments args;
    args.instanceCount = 1;
    args.vertexCount = 4;
    commandList->draw(args);

    commandList->close();
    resource_allocator.device->executeCommandList(commandList);

    params.set_output("Tex", output);
}

static void node_register()
{
    static NodeTypeInfo ntype;

    strcpy(ntype.ui_name, "Blit to Present");
    strcpy(ntype.id_name, "node_render_blit_to_present");

    render_node_type_base(&ntype);
    ntype.node_execute = node_exec;
    ntype.declare = node_declare;
    nodeRegisterType(&ntype);
}

NOD_REGISTER_NODE(node_register)
}  // namespace USTC_CG::node_render_blit_to_present
