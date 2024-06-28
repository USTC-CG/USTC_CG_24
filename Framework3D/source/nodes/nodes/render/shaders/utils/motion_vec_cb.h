#pragma once

#include "cpp_shader_macro.h"
struct FrameConstants {
#ifdef __cplusplus
    USING_PXR_MATH_TYPES
#endif
    float4x4 CurrentViewProjMatrix;  // Current frame's view-projection matrix
    float4x4 PrevViewProjMatrix;     // Previous frame's view-projection matrix
    float4x4 InvCurrentViewProjMatrix;  // Inverse of current frame's
                                        // view-projection matrix
    float2 Resolution;                  // Resolution of the render target
};