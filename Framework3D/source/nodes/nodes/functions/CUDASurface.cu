#include <vector_functions.h>

#include "CUDASurface.cuh"
#include "GPUParallel.cuh"

void BlitLinearBufferToSurface(
    float4* buffer,
    cudaSurfaceObject_t surface_object,
    int width,
    int height)
{
    GPUParallelFor2D(
        "Copy float4 to surface object",
        make_int2(width, height),
        GPU_LAMBDA_Ex(int i, int j) {
            float4 val = buffer[i * width + j];
            surf2Dwrite(val, surface_object, sizeof(float4) * j, i);
        });
}

void BlitLinearBufferToSurface(
    float3* buffer,
    cudaSurfaceObject_t surface_object,
    int width,
    int height)
{
    GPUParallelFor2D(
        "Copy float3 to surface object",
        make_int2(width, height),
        GPU_LAMBDA_Ex(int i, int j) {
            auto val_3 = buffer[i * width + j];
            float4 val = make_float4(val_3.x, val_3.y, val_3.z, 1);

            surf2Dwrite(val, surface_object, sizeof(float4) * j, i);
        });
}

void ComposeChannels(
    float4* target,
    float* x,
    float* y,
    float* z,
    float* w,
    int size)
{
    GPUParallelFor(
        "Compose 4 channels into float4", size, GPU_LAMBDA_Ex(int i) {
            target[i] = make_float4(x[i], y[i], z[i], w[i]);
        });
}

void ComposeChannels(float4* target, float* x, float* y, float* z, int size)
{
    GPUParallelFor(
        "Compose 3 channels into float4", size, GPU_LAMBDA_Ex(int i) {
            target[i] = make_float4(x[i], y[i], z[i], 1);
        });
}

void ComposeChannels(float3* target, float* x, float* y, float* z, int size)
{
    GPUParallelFor(
        "Compose 4 channels into float3", size, GPU_LAMBDA_Ex(int i) {
            target[i] = make_float3(x[i], y[i], z[i]);
        });
}
