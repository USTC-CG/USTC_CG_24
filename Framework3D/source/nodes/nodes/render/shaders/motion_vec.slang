#pragma once

#include "utils/motion_vec_cb.h"
// Texture inputs
Texture2D<float4> WorldPositionTexture
    : register(t0);  // World position texture of the current frame

// UAV output
RWTexture2D<float2> MotionVectorTexture
    : register(u0);  // Output motion vector texture

// Sampler
SamplerState PointSampler : register(s0);

ConstantBuffer<FrameConstants> params : register(b0);

[numthreads(8, 8, 1)]
void main(uint3 DTid: SV_DispatchThreadID)
{
    uint2 pixelCoord = DTid.xy;
    float2 uv = (pixelCoord + 0.5) / params.Resolution;
    float4 worldPos = WorldPositionTexture.Load(int3(pixelCoord, 0));
    float4 prevClipPos = mul(worldPos, params.PrevProjViewMatrix);
    float2 prevUV =
        prevClipPos.xy / prevClipPos.w * 0.5 + 0.5;  // NDC to UV coordinates

    // Calculate motion vector
    float2 motionVector = prevUV - uv;

    if (any(isnan(motionVector)))
        motionVector = 0.0;

    // Output the motion vector
    MotionVectorTexture[pixelCoord] = motionVector;
}
