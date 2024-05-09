// shader.slang

//TEST_INPUT:ubuffer(random(float, 4096, -1.0, 1.0), stride=4):name=ioBuffer
struct Holy
{
    int a;
    int b;
    int c;
    int d;
};


RWStructuredBuffer<float> ioBuffer;
RWStructuredBuffer<Holy> holy_ioBuffer;
RWStructuredBuffer<int> int_ioBuffer;
ByteAddressBuffer byteBuffer;


RaytracingAccelerationStructure SceneBVH : register(t0);
Buffer<float3> positions : register(t1);

RWStructuredBuffer<uint2> Pairs : register(u0);
StructuredBuffer<uint4> PairsCount : register(u1);


[shader("compute")]
[numthreads(4, 1, 1)]
void computeMain(uint3 dispatchThreadID : SV_DispatchThreadID)
{
    uint tid = dispatchThreadID.x;

    float i = ioBuffer[tid];
    float o = i < 0.5 ? (i + i) : sqrt(i);

    ioBuffer[tid] = o;
    
    InterlockedAdd(int_ioBuffer[tid], 1);

}
