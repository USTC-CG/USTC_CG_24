#ifndef CPP_SHADER_MACRO
#define CPP_SHADER_MACRO

#ifdef __cplusplus
#include "pxr/base/gf/matrix4f.h"
#define USING_PXR_MATH_TYPES          \
    using uint = unsigned;            \
    using float4x4 = pxr::GfMatrix4f; \
    using float2 = pxr::GfVec2f;      \
    using float3 = pxr::GfVec3f;      \
    using float4 = pxr::GfVec4f;      \
    using int2 = pxr::GfVec2i;        \
    using int3 = pxr::GfVec3i;        \
    using int4 = pxr::GfVec4i;
#endif

// #ifdef __cplusplus
// USING_PXR_MATH_TYPES
// #endif

#endif  // !CPP_SHADER_MACRO
