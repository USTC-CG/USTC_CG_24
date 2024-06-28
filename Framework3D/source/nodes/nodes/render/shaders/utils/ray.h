#pragma once

#include "cpp_shader_macro.h"
// Constants
struct RayDesc {
#ifdef __cplusplus
    USING_PXR_MATH_TYPES
#endif
    // Just keep it simple

    float3 Origin;
    float3 Direction;
    float TMin;
    float TMax;
};