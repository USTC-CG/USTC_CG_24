// Random number generation using XOR-Shift algorithm
#include "random.slangh"

RWTexture2D<uint> inout_random : register(u0);

[numthreads(16, 16, 1)]  // Number of threads in a group
void main(uint3 DTid: SV_DispatchThreadID)
{
    uint2 coord = DTid.xy;
    var seed = inout_random[coord];
    uint randomValue = random_unsigned(seed);
    inout_random[coord] = randomValue;
}
