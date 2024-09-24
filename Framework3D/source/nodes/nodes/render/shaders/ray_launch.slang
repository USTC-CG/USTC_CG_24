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
StructuredBuffer<int2> inPixelTarget : register(t1);
RWStructuredBuffer<RayDesc> rays : register(u0);
RWStructuredBuffer<HitObjectInfo> hitObjectInfo : register(u1);
RWStructuredBuffer<int2> PixelTarget : register(u2);

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
    uint LaunchIndex = DispatchRaysIndex().x;
    uint LaunchDimensions = DispatchRaysDimensions().x;
    // Setup the ray
    RayDesc ray = rays[LaunchIndex];

    // Trace the ray
    HitInfo payload;
    payload.ShadedColorAndHitT = float4(0, 0, 0, 0);

    TraceRay(SceneBVH, RAY_FLAG_NONE, 0xFF, 0, 0, 0, ray, payload);
    var color = payload.ShadedColorAndHitT.rgb;
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
    uint stride_count = 0;
    uint stride_size = 0;
    hitObjectInfo.GetDimensions(stride_count, stride_size);

    uint index = 0;
    InterlockedAdd(hitObjectInfo[stride_count - 1].InstanceIndex, 1, index);


    PixelTarget[index] = inPixelTarget[DispatchRaysIndex().x];
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
