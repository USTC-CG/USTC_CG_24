/*
 * Copyright (c) 2014-2021, NVIDIA CORPORATION. All rights reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */

// ---[ Structures ]---
#include "matrix_op.h"
#include "utils/HitObject.h"
#include "utils/ray.h"
#include "view_cb.h"

RaytracingAccelerationStructure SceneBVH : register(t0);
RWStructuredBuffer<RayDesc> rays : register(u0);
RWTexture2D<float4> BaryCentric : register(u1);
RWTexture2D<float4> WorldPosition : register(u2);
RWStructuredBuffer<HitObjectInfo> hitObjectInfo : register(u3);

struct HitInfo {
    float2 barycentric;
    float3 position;
    float4 ShadedColorAndHitT : SHADED_COLOR_AND_HIT_T;
};

struct Attributes {
    float2 uv;
};

// ---[ Ray Generation Shader ]---

[shader("raygeneration")]
void RayGen()
{
    uint2 LaunchIndex = DispatchRaysIndex().xy;
    uint2 LaunchDimensions = DispatchRaysDimensions().xy;
    // Setup the ray
    RayDesc ray = rays[LaunchIndex.y * LaunchDimensions.x + LaunchIndex.x];

    // Trace the ray
    HitInfo payload;
    payload.ShadedColorAndHitT = float4(0, 0, 0, 0);

    TraceRay(SceneBVH, RAY_FLAG_NONE, 0xFF, 0, 0, 0, ray, payload);
    var color = payload.ShadedColorAndHitT.rgb;
    BaryCentric[LaunchIndex.xy] = float4(payload.barycentric, 0, 1);
    WorldPosition[LaunchIndex.xy] = float4(payload.position, 1);
}

// ---[ Closest Hit Shader ]---

[shader("closesthit")]
void ClosestHit(
    inout HitInfo payload: SV_RayPayload,
    Attributes attrib: SV_IntersectionAttributes)
{
    payload.barycentric = attrib.uv;
    payload.position = WorldRayOrigin() + WorldRayDirection() * RayTCurrent();

    // Push hit information into hitObjectInfo buffer
    uint index = 0;
    InterlockedAdd(hitObjectInfo[0].InstanceIndex, 1, index);
    index += 1;
    hitObjectInfo[index].InstanceIndex = InstanceIndex();
    hitObjectInfo[index].GeometryIndex = GeometryIndex();
    hitObjectInfo[index].PrimitiveIndex = PrimitiveIndex();
    hitObjectInfo[index].HitKind = HitKind();

    hitObjectInfo[index].RayContributionToHitGroupIndex = 0;
    hitObjectInfo[index].MultiplierForGeometryContributionToHitGroupIndex = 0;
    hitObjectInfo[index].Ray = {
        WorldRayOrigin(), RayTMin(), WorldRayDirection(), RayTCurrent()
    };
    hitObjectInfo[index].attributes = payload.barycentric;
}

// ---[ Miss Shader ]---

[shader("miss")]
void Miss(inout HitInfo payload: SV_RayPayload)
{
    payload.barycentric = 0;
    payload.position = 1.0 / 0.0f;
}
