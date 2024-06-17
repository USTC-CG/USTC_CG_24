// Random number generation using XOR-Shift algorithm
#include "random.slangh"

RWTexture2D<uint4> inout_random : register(u0);

[numthreads(16, 16, 1)]  // Number of threads in a group
void main(uint3 DTid: SV_DispatchThreadID)
{
    uint2 coord = DTid.xy;
    uint seed = coord.x + coord.y * 32;
    uint randomValue = random_unsigned(seed);
    inout_random[coord] = uint4(randomValue, randomValue, 1, randomValue);
}
