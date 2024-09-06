// TAAComputeShader.hlsl
#pragma once
#include "utils/taa_cb.h"
// Texture inputs
Texture2D<float4> CurrentFrame : register(t0);    // Current frame color
Texture2D<float4> PreviousFrame : register(t1);   // Previous frame color
Texture2D<float2> VelocityBuffer : register(t2);  // Screen-space motion vectors

// UAV output
RWTexture2D<float4> OutputFrame : register(u0);  // Output TAA frame

// Sampler
SamplerState PointSampler : register(s0);

ConstantBuffer<FrameConstants> frameConstants : register(b0);

[numthreads(8, 8, 1)]
void main(uint3 DTid: SV_DispatchThreadID)
{
    uint2 pixelCoord = DTid.xy;
    float2 uv = (pixelCoord+0.5) / frameConstants.Resolution;

    // Read velocity buffer
    float2 velocity = VelocityBuffer.Load(int3(pixelCoord, 0)).xy;

    // Calculate previous frame UV using velocity and jitter
    float2 prevUV = uv + velocity;

    // Sample colors
    float4 currentColor = CurrentFrame.Load(int3(pixelCoord, 0));
    float4 prevColor = PreviousFrame.SampleLevel(PointSampler, prevUV, 0);

    float spp = 16;
    // Simple blending for TAA
    float blendFactor = (spp - 1) / spp;  // Adjust blend factor as needed
    float4 resultColor = lerp(currentColor, prevColor, blendFactor);
    // resultColor = float4(uv.x, uv.y, 1, 1);
    //  Output the result
    OutputFrame[pixelCoord] = resultColor;
}
