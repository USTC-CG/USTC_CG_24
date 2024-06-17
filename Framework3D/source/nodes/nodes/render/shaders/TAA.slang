// Define the resources
Texture2D<float4> currentFrame : register(t0);
Texture2D<float4> previousFrame : register(t1);
Texture2D<float4> motionVectors : register(t2); // Contains motion vectors for each pixel
RWTexture2D<float4> outputTexture : register(u0); // Output texture

// Temporal blending factor
cbuffer BlendFactorBuffer : register(b0)
{
    float blendFactor; // Example blend factor, should be tuned
};

// Define the compute shader
[numthreads(8, 8, 1)] // Number of threads in a group
void CSMain(uint3 dispatchThreadID : SV_DispatchThreadID)
{
    uint2 coord = dispatchThreadID.xy;
    
    // Normalize coordinates
    float2 texCoord = coord / float2(outputTexture.GetDimensions());
    
    // Fetch the current frame color
    float4 currentColor = currentFrame.Load(int3(coord, 0));
    
    // Fetch the motion vector
    float2 motionVector = motionVectors.Load(int3(coord, 0)).xy;
    
    // Reproject the texture coordinates to the previous frame
    float2 prevTexCoord = texCoord + motionVector;
    
    // Fetch the previous frame color
    float4 prevColor = previousFrame.SampleLevel(samLinear, prevTexCoord, 0);
    
    // Blend the current and previous frame colors
    float4 blendedColor = lerp(currentColor, prevColor, blendFactor);
    
    // Write the result to the output texture
    outputTexture[coord] = blendedColor;
}
