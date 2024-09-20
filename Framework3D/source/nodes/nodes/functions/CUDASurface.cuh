#pragma once
#include <surface_types.h>
#include <vector_types.h>

#include "USTC_CG.h"

void USTC_CG_API BlitLinearBufferToSurface(
    float4* buffer,
    cudaSurfaceObject_t surfaceObject,
    int width,
    int height);

void USTC_CG_API BlitLinearBufferToSurface(
    float3* buffer,
    cudaSurfaceObject_t surfaceObject,
    int width,
    int height);

void USTC_CG_API ComposeChannels(
    float4* target,
    float* x,
    float* y,
    float* z,
    float* w,
    int size);
void ComposeChannels(float4* target, float* x, float* y, float* z, int size);
void ComposeChannels(float3* target, float* x, float* y, float* z, int size);