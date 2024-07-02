#pragma once

#include "cpp_shader_macro.h"
// Constants
struct FrameConstants {
#ifdef __cplusplus
    USING_PXR_MATH_TYPES
#endif
    float2 Resolution;  // Resolution of the render target
};