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
#include "view_cb.h"

struct HitInfo {
    float4 ShadedColorAndHitT : SHADED_COLOR_AND_HIT_T;
};

struct Attributes {
    float2 uv;
};

// ---[ Constant Buffers ]---
ConstantBuffer<PlanarViewConstants> viewConstant : register(b0);

// ---[ Resources ]---

RWTexture2D<float4> RTOutput : register(u0);
RaytracingAccelerationStructure SceneBVH : register(t0);

RayDesc setupPrimaryRay(uint2 pixelPosition, PlanarViewConstants view)
{
    float2 uv = (float2(pixelPosition) + 0.5) * view.viewportSizeInv;
    float4 clipPos = float4(uv.x * 2.0 - 1.0, uv.y * 2.0 - 1, 1.0, 1);
    float4 worldPos = mul(clipPos, view.matClipToWorld);
    worldPos.xyz /= worldPos.w;

    RayDesc ray;
    ray.Origin = view.cameraDirectionOrPosition.xyz;
    ray.Direction = normalize(worldPos.xyz - ray.Origin);
    ray.TMin = 0;
    ray.TMax = 1000;
    return ray;
}

// ---[ Ray Generation Shader ]---

[shader("raygeneration")]
void RayGen()
{
    uint2 LaunchIndex = DispatchRaysIndex().xy;
    uint2 LaunchDimensions = DispatchRaysDimensions().xy;
    // Setup the ray
    RayDesc ray = setupPrimaryRay(LaunchIndex, viewConstant);

    // Trace the ray
    HitInfo payload;
    payload.ShadedColorAndHitT = float4(0, 0, 0, 0);

    TraceRay(SceneBVH, RAY_FLAG_NONE, 0xFF, 0, 0, 0, ray, payload);
    var color = payload.ShadedColorAndHitT.rgb;
    RTOutput[LaunchIndex.xy] = float4(color, 1.f);
}

// ---[ Closest Hit Shader ]---

[shader("closesthit")]
void ClosestHit(
    inout HitInfo payload: SV_RayPayload,
    Attributes attrib: SV_IntersectionAttributes)
{
    float3 barycentrics =
        float3((1.0f - attrib.uv.x - attrib.uv.y), attrib.uv.x, attrib.uv.y);
    payload.ShadedColorAndHitT = float4(barycentrics, RayTCurrent());
}

// ---[ Miss Shader ]---

[shader("miss")]
void Miss(inout HitInfo payload: SV_RayPayload)
{
    payload.ShadedColorAndHitT = float4(0.1f, 0.2f, 0.3f, 1.f);
}
