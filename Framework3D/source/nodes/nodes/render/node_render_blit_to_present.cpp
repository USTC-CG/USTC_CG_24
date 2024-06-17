#include "NODES_FILES_DIR.h"
#include "Nodes/node.hpp"
#include "Nodes/node_declare.hpp"
#include "Nodes/node_register.h"
#include "RCore/Backend.hpp"
#include "nvrhi/utils.h"
#include "pxr/base/gf/range2f.h"
#include "render_node_base.h"
#include "resource_allocator_instance.hpp"
#include "shaders/utils/blit_cb.h"
#include "utils/compile_shader.h"

namespace USTC_CG::node_render_blit_to_present {

enum class BlitSampler { Point, Linear, Sharpen };

struct BlitParameters {
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
    auto output = resource_allocator.create(output_desc);

    bool first_attempt;
    auto& blit_parameters = params.get_storage<BlitParameters&>();

    auto commandList = resource_allocator.create(CommandListDesc{});
    assert(commandList);

    auto framebuffer_desc = FramebufferDesc{};

    framebuffer_desc.colorAttachments.push_back(
        nvrhi::FramebufferAttachment{ output.Get() });

    auto targetFramebuffer = resource_allocator.create(framebuffer_desc);

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

    nvrhi::BindingLayoutDescVector binding_layout_descs;
    std::string error_string;

    auto vertex_shader = compile_shader(
        "main",
        nvrhi::ShaderType::Vertex,
        "shaders/utils/" + vs_name,
        binding_layout_descs,
        error_string,
        macro_defines);

    auto pixel_shader = compile_shader(
        "main",
        nvrhi::ShaderType::Pixel,
        "shaders/utils/" + ps_name,
        binding_layout_descs,
        error_string,
        macro_defines);

    // This is how it should look like.
    // BindingLayoutDesc layoutDesc;
    // layoutDesc.visibility = nvrhi::ShaderType::All;
    // layoutDesc.bindings = { nvrhi::BindingLayoutItem::PushConstants(
    //                             0, sizeof(BlitConstants)),
    //                         nvrhi::BindingLayoutItem::Texture_SRV(0),
    //                         nvrhi::BindingLayoutItem::Sampler(0) };

    auto samplerDesc =
        nvrhi::SamplerDesc().setAllFilters(false).setAllAddressModes(
            nvrhi::SamplerAddressMode::Clamp);
    m_PointClampSampler = resource_allocator.create(samplerDesc);

    samplerDesc.setAllFilters(true);
    m_LinearClampSampler = resource_allocator.create(samplerDesc);

    auto m_BlitBindingLayout =
        resource_allocator.create(binding_layout_descs[0]);

    nvrhi::GraphicsPipelineDesc psoDesc;
    psoDesc.bindingLayouts = { m_BlitBindingLayout };
    psoDesc.VS = vertex_shader;
    psoDesc.PS = pixel_shader;
    psoDesc.primType = nvrhi::PrimitiveType::TriangleStrip;
    psoDesc.renderState.rasterState.setCullNone();
    psoDesc.renderState.depthStencilState.depthTestEnable = false;
    psoDesc.renderState.depthStencilState.stencilEnable = false;
    psoDesc.renderState.blendState.targets[0] = blit_parameters.blendState;

    auto pso = resource_allocator.create(psoDesc, targetFramebuffer);

    nvrhi::BindingSetDesc bindingSetDesc;
    {
        auto sourceDimension = sourceDesc.dimension;
        if (sourceDimension == nvrhi::TextureDimension::TextureCube ||
            sourceDimension == nvrhi::TextureDimension::TextureCubeArray)
            sourceDimension = nvrhi::TextureDimension::Texture2DArray;

        auto sourceSubresources = nvrhi::TextureSubresourceSet(
            blit_parameters.sourceMip, 1, blit_parameters.sourceArraySlice, 1);

        bindingSetDesc.bindings = {
            nvrhi::BindingSetItem::PushConstants(0, sizeof(BlitConstants)),
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

    //// If a binding cache is provided, get the binding set from the cache.
    //// Otherwise, create one and then release it.
    // nvrhi::BindingSetHandle sourceBindingSet;
    // if (bindingCache)
    //     sourceBindingSet = bindingCache->GetOrCreateBindingSet(
    //         bindingSetDesc, m_BlitBindingLayout);
    // else
    //     sourceBindingSet =
    //         m_Device->createBindingSet(bindingSetDesc,
    //         m_BlitBindingLayout);

    // nvrhi::GraphicsState state;
    // state.pipeline = pso;
    // state.framebuffer = targetFramebuffer;
    // state.bindings = { sourceBindingSet };
    // state.viewport.addViewport(targetViewport);
    // state.viewport.addScissorRect(nvrhi::Rect(targetViewport));
    // state.blendConstantColor = blendConstantColor;

    // BlitConstants blitConstants = {};
    // blitConstants.sourceOrigin = float2(sourceBox.m_mins);
    // blitConstants.sourceSize = sourceBox.diagonal();
    // blitConstants.targetOrigin = float2(targetBox.m_mins);
    // blitConstants.targetSize = targetBox.diagonal();

    // commandList->setGraphicsState(state);

    // commandList->setPushConstants(&blitConstants, sizeof(blitConstants));

    // nvrhi::DrawArguments args;
    // args.instanceCount = 1;
    // args.vertexCount = 4;
    // commandList->draw(args);

    resource_allocator.destroy(commandList);
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
