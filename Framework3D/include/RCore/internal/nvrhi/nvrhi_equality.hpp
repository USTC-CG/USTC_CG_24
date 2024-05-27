#pragma once
#include "USTC_CG.h"
#include "nvrhi/nvrhi.h"

namespace nvrhi {
inline bool operator==(const TextureDesc& lhs, const TextureDesc& rhs)
{
    return lhs.width == rhs.width && lhs.height == rhs.height && lhs.depth == rhs.depth &&
           lhs.arraySize == rhs.arraySize && lhs.mipLevels == rhs.mipLevels &&
           lhs.sampleCount == rhs.sampleCount && lhs.sampleQuality == rhs.sampleQuality &&
           lhs.format == rhs.format && lhs.dimension == rhs.dimension &&
           lhs.debugName == rhs.debugName && lhs.isShaderResource == rhs.isShaderResource &&
           lhs.isRenderTarget == rhs.isRenderTarget && lhs.isUAV == rhs.isUAV &&
           lhs.isTypeless == rhs.isTypeless &&
           lhs.isShadingRateSurface == rhs.isShadingRateSurface &&
           lhs.sharedResourceFlags == rhs.sharedResourceFlags && lhs.isVirtual == rhs.isVirtual &&
           lhs.clearValue == rhs.clearValue && lhs.useClearValue == rhs.useClearValue &&
           lhs.initialState == rhs.initialState && lhs.keepInitialState == rhs.keepInitialState;
}

inline bool operator!=(const TextureDesc& lhs, const TextureDesc& rhs)
{
    return !(lhs == rhs);
}
inline bool operator==(const ShaderDesc& lhs, const ShaderDesc& rhs)
{
    return lhs.shaderType == rhs.shaderType && lhs.debugName == rhs.debugName &&
           lhs.entryName == rhs.entryName && lhs.hlslExtensionsUAV == rhs.hlslExtensionsUAV &&
           lhs.useSpecificShaderExt == rhs.useSpecificShaderExt &&
           lhs.numCustomSemantics == rhs.numCustomSemantics &&
           lhs.pCustomSemantics == rhs.pCustomSemantics && lhs.fastGSFlags == rhs.fastGSFlags &&
           lhs.pCoordinateSwizzling == rhs.pCoordinateSwizzling;
}

inline bool operator!=(const ShaderDesc& lhs, const ShaderDesc& rhs)
{
    return !(lhs == rhs);
}

inline bool operator==(const BufferDesc& lhs, const BufferDesc& rhs)
{
    return lhs.byteSize == rhs.byteSize && lhs.structStride == rhs.structStride &&
           lhs.maxVersions == rhs.maxVersions && lhs.debugName == rhs.debugName &&
           lhs.format == rhs.format && lhs.canHaveUAVs == rhs.canHaveUAVs &&
           lhs.canHaveTypedViews == rhs.canHaveTypedViews &&
           lhs.canHaveRawViews == rhs.canHaveRawViews && lhs.isVertexBuffer == rhs.isVertexBuffer &&
           lhs.isIndexBuffer == rhs.isIndexBuffer && lhs.isConstantBuffer == rhs.isConstantBuffer &&
           lhs.isDrawIndirectArgs == rhs.isDrawIndirectArgs &&
           lhs.isAccelStructBuildInput == rhs.isAccelStructBuildInput &&
           lhs.isAccelStructStorage == rhs.isAccelStructStorage &&
           lhs.isShaderBindingTable == rhs.isShaderBindingTable &&
           lhs.isVolatile == rhs.isVolatile && lhs.isVirtual == rhs.isVirtual &&
           lhs.initialState == rhs.initialState && lhs.keepInitialState == rhs.keepInitialState &&
           lhs.cpuAccess == rhs.cpuAccess && lhs.sharedResourceFlags == rhs.sharedResourceFlags;
}

inline bool operator!=(const BufferDesc& lhs, const BufferDesc& rhs)
{
    return !(lhs == rhs);
}

using nvrhi::static_vector;
using nvrhi::VulkanBindingOffsets;

template<typename T, uint32_t _max_elements>
inline bool operator==(
    const static_vector<T, _max_elements>& lhs,
    const static_vector<T, _max_elements>& rhs)
{
    return std::equal(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
}

inline bool operator==(const VulkanBindingOffsets& lhs, const VulkanBindingOffsets& rhs)
{
    return lhs.shaderResource == rhs.shaderResource && lhs.sampler == rhs.sampler &&
           lhs.constantBuffer == rhs.constantBuffer && lhs.unorderedAccess == rhs.unorderedAccess;
}

inline bool operator!=(const VulkanBindingOffsets& lhs, const VulkanBindingOffsets& rhs)
{
    return !(lhs == rhs);
}

inline bool operator==(const BindingLayoutDesc& lhs, const BindingLayoutDesc& rhs)
{
    return lhs.visibility == rhs.visibility && lhs.registerSpace == rhs.registerSpace &&
           lhs.registerSpaceIsDescriptorSet == rhs.registerSpaceIsDescriptorSet &&
           lhs.bindings == rhs.bindings && lhs.bindingOffsets == rhs.bindingOffsets;
}

inline bool operator!=(const BindingLayoutDesc& lhs, const BindingLayoutDesc& rhs)
{
    return !(lhs == rhs);
}
namespace rt {
    inline bool operator==(const PipelineHitGroupDesc& lhs, const PipelineHitGroupDesc& rhs)
    {
        return lhs.exportName == rhs.exportName && lhs.closestHitShader == rhs.closestHitShader &&
               lhs.anyHitShader == rhs.anyHitShader &&
               lhs.intersectionShader == rhs.intersectionShader &&
               lhs.bindingLayout == rhs.bindingLayout &&
               lhs.isProceduralPrimitive == rhs.isProceduralPrimitive;
    }

    inline bool operator!=(const PipelineHitGroupDesc& lhs, const PipelineHitGroupDesc& rhs)
    {
        return !(lhs == rhs);
    }

    inline bool operator==(const PipelineShaderDesc& lhs, const PipelineShaderDesc& rhs)
    {
        return lhs.exportName == rhs.exportName && lhs.shader == rhs.shader &&
               lhs.bindingLayout == rhs.bindingLayout;
    }

    inline bool operator!=(const PipelineShaderDesc& lhs, const PipelineShaderDesc& rhs)
    {
        return !(lhs == rhs);
    }

    inline bool operator==(const PipelineDesc& lhs, const PipelineDesc& rhs)
    {
        return lhs.shaders == rhs.shaders && lhs.hitGroups == rhs.hitGroups &&
               lhs.globalBindingLayouts == rhs.globalBindingLayouts &&
               lhs.maxPayloadSize == rhs.maxPayloadSize &&
               lhs.maxAttributeSize == rhs.maxAttributeSize &&
               lhs.maxRecursionDepth == rhs.maxRecursionDepth &&
               lhs.hlslExtensionsUAV == rhs.hlslExtensionsUAV;
    }

    inline bool operator!=(const PipelineDesc& lhs, const PipelineDesc& rhs)
    {
        return !(lhs == rhs);
    }

    inline bool operator==(const GeometryTriangles& lhs, const GeometryTriangles& rhs)
    {
        return lhs.indexBuffer == rhs.indexBuffer && lhs.vertexBuffer == rhs.vertexBuffer &&
               lhs.indexFormat == rhs.indexFormat && lhs.vertexFormat == rhs.vertexFormat &&
               lhs.indexOffset == rhs.indexOffset && lhs.vertexOffset == rhs.vertexOffset &&
               lhs.indexCount == rhs.indexCount && lhs.vertexCount == rhs.vertexCount &&
               lhs.vertexStride == rhs.vertexStride && lhs.opacityMicromap == rhs.opacityMicromap &&
               lhs.ommIndexBuffer == rhs.ommIndexBuffer &&
               lhs.ommIndexBufferOffset == rhs.ommIndexBufferOffset &&
               lhs.ommIndexFormat == rhs.ommIndexFormat &&
               lhs.pOmmUsageCounts == rhs.pOmmUsageCounts &&
               lhs.numOmmUsageCounts == rhs.numOmmUsageCounts;
    }

    inline bool operator!=(const GeometryTriangles& lhs, const GeometryTriangles& rhs)
    {
        return !(lhs == rhs);
    }

    inline bool operator==(const GeometryDesc& lhs, const GeometryDesc& rhs)
    {
        return lhs.geometryData.triangles == rhs.geometryData.triangles &&
               lhs.useTransform == rhs.useTransform && lhs.flags == rhs.flags &&
               lhs.geometryType == rhs.geometryType &&
               memcmp(lhs.transform, rhs.transform, sizeof(nvrhi::rt::AffineTransform)) == 0;
    }

    inline bool operator!=(const GeometryDesc& lhs, const GeometryDesc& rhs)
    {
        return !(lhs == rhs);
    }

    inline bool operator==(const AccelStructDesc& lhs, const AccelStructDesc& rhs)
    {
        return lhs.topLevelMaxInstances == rhs.topLevelMaxInstances &&
               lhs.bottomLevelGeometries == rhs.bottomLevelGeometries &&
               lhs.buildFlags == rhs.buildFlags && lhs.debugName == rhs.debugName &&
               lhs.trackLiveness == rhs.trackLiveness && lhs.isTopLevel == rhs.isTopLevel &&
               lhs.isVirtual == rhs.isVirtual;
    }

    inline bool operator!=(const AccelStructDesc& lhs, const AccelStructDesc& rhs)
    {
        return !(lhs == rhs);
    }
}  // namespace rt
}  // namespace nvrhi