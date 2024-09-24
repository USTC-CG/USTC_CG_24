#pragma once

#include "cpp_shader_macro.h"
// Constants
#ifdef __cplusplus
struct RayDesc {
    USING_PXR_MATH_TYPES

    float3 Origin;
    float TMin;
    float3 Direction;
    float TMax;
};
#endif