#ifndef CPP_SHADER_MACRO
#define CPP_SHADER_MACRO

#define USING_PXR_MATH_TYPES          \
    using float4x4 = pxr::GfMatrix4f; \
    using float2 = pxr::GfVec2f;      \
    using float3 = pxr::GfVec3f;       \
    using float4 = pxr::GfVec4f;

#endif  // !CPP_SHADER_MACRO
