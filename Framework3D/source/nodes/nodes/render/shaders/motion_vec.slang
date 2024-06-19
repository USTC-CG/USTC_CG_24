// MotionVectorComputeShader.hlsl
#pragma once

// Texture inputs
Texture2D<float4> DepthTexture : register(t0); // Depth texture of the current frame

// UAV output
RWTexture2D<float2> MotionVectorTexture : register(u0); // Output motion vector texture

// Sampler
SamplerState PointSampler : register(s0);

// Constants
cbuffer FrameConstants : register(b0)
{
    float4x4 CurrentViewProjMatrix;  // Current frame's view-projection matrix
    float4x4 PrevViewProjMatrix;     // Previous frame's view-projection matrix
    float4x4 InvCurrentViewProjMatrix; // Inverse of current frame's view-projection matrix
    float2 Resolution;               // Resolution of the render target
};

[numthreads(8, 8, 1)]
void CSMain(uint3 DTid : SV_DispatchThreadID)
{
    uint2 pixelCoord = DTid.xy;
    float2 uv = pixelCoord / Resolution;
    float depth = DepthTexture.Load(int3(pixelCoord, 0)).r;

    // Reconstruct current frame's view-space position
    float4 ndcPos = float4(uv * 2.0 - 1.0, depth, 1.0); // Normalized Device Coordinates (NDC)
    float4 viewPos = mul(ndcPos, InvCurrentViewProjMatrix);
    viewPos /= viewPos.w; // Convert from homogeneous coordinates

    // Transform view position to previous frame's clip space
    float4 prevClipPos = mul(viewPos, PrevViewProjMatrix);
    float2 prevUV = prevClipPos.xy / prevClipPos.w * 0.5 + 0.5; // NDC to UV coordinates

    // Calculate motion vector
    float2 motionVector = prevUV - uv;

    // Output the motion vector
    MotionVectorTexture[pixelCoord] = motionVector;
}
