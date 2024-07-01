#pragma once
#include "cpp_shader_macro.h"
#include "ray.h"

struct HitObjectInfo {
#ifdef __cplusplus
    USING_PXR_MATH_TYPES
#endif
    uint InstanceIndex;
    uint GeometryIndex;
    uint PrimitiveIndex;
    uint HitKind;
    uint RayContributionToHitGroupIndex;
    uint MultiplierForGeometryContributionToHitGroupIndex;
    RayDesc Ray;
    uint attributes[4];
};