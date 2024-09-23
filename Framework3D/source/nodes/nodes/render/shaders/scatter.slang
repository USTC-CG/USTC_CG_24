// Compute shader that take two buffers, one with the pixel id and the other
// with the color, write the color to a texture with the specified id.

// Input buffers
StructuredBuffer<float4> inputColor : register(t0);
StructuredBuffer<uint2> inputPixelID : register(t1);

// Output texture
RWTexture2D<float4> outputTexture : register(u0);

[numthreads(64, 1, 1)]
void main(uint3 DTid: SV_DispatchThreadID)
{
    var pixelID = inputPixelID[DTid.x];
    float4 color = inputColor[DTid.x];
    outputTexture[DTid.xy] += color;
}
