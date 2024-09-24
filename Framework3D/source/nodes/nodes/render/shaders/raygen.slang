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
#include "utils/random.slangh"
#include "view_cb.h"

RWStructuredBuffer<RayDesc> rays : register(u0);
RWTexture2D<uint> random_seeds : register(u1);
RWStructuredBuffer<uint2> pixel_targets : register(u2);

// ---[ Constant Buffers ]---
ConstantBuffer<PlanarViewConstants> viewConstant : register(b0);

// ---[ Resources ]---

RayDesc
setupPrimaryRay(uint2 pixelPosition, PlanarViewConstants view, inout uint seed)
{
    float2 pixelPositionF = float2(pixelPosition);
    pixelPositionF.x += random_float(seed);
    pixelPositionF.y += random_float(seed);

    float2 uv = pixelPositionF * view.viewportSizeInv;
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

[numthreads(8, 8, 1)]
void main(uint3 DTid: SV_DispatchThreadID)
{
    uint2 LaunchIndex = DTid.xy;
    uint2 LaunchDimensions = viewConstant.resolution;

    if (all(LaunchIndex.xy <= LaunchDimensions.xy)) {
        var seed = random_seeds[LaunchIndex];
        // Setup the ray
        RayDesc ray = setupPrimaryRay(LaunchIndex, viewConstant, seed);

        rays[LaunchIndex.y * LaunchDimensions.x + LaunchIndex.x] = ray;
        pixel_targets[LaunchIndex.y * LaunchDimensions.x + LaunchIndex.x] =
            LaunchIndex;
        random_seeds[LaunchIndex] = seed;
    }
}
