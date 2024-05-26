#pragma once
#include "USTC_CG.h"
#include "nvrhi/nvrhi.h"

USTC_CG_NAMESPACE_OPEN_SCOPE
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
USTC_CG_NAMESPACE_CLOSE_SCOPE