// pbrt is Copyright(c) 1998-2020 Matt Pharr, Wenzel Jakob, and Greg Humphreys.
// The pbrt source code is licensed under the Apache License, Version 2.0.
// SPDX: Apache-2.0

// Borrowed from pbrt-v4

#pragma once

// GPU Launch Function Declarations

#include <vector_types.h>

#include <map>
#include <typeindex>
#include <utility>

#include "Utils/CUDA/CUDAException.h"

#pragma once

#if defined(__CUDACC__) || defined(__CUDABE__)
#define HOST_DEVICE __host__ __device__
#define INLINE      __forceinline__
#define CONST_STATIC_INIT(...)
#else
#define HOST_DEVICE
#define INLINE                 inline
#define CONST_STATIC_INIT(...) = __VA_ARGS__
#endif

template<typename F>
inline int GetBlockSize(const char* description, F kernel)
{
    // Note: this isn't reentrant, but that's fine for our purposes...
    static std::map<std::type_index, int> kernelBlockSizes;

    auto index = std::type_index(typeid(F));

    auto iter = kernelBlockSizes.find(index);
    if (iter != kernelBlockSizes.end())
        return iter->second;

    int minGridSize, blockSize;
    CUDA_CHECK(cudaOccupancyMaxPotentialBlockSize(
        &minGridSize, &blockSize, kernel, 0, 0));
    kernelBlockSizes[index] = blockSize;

    return blockSize;
}

template<typename F>
void GPUParallelFor(const char* description, int nItems, F func);

template<typename F>
void GPUParallelFor2D(const char* description, int2 resolution, F func);
#ifdef __CUDACC__

template<typename F>
__global__ void Kernel(F func, int nItems)
{
    int tid = blockIdx.x * blockDim.x + threadIdx.x;
    if (tid >= nItems)
        return;

    func(tid);
}

template<typename F>
__global__ void Kernel2D(F func, int2 resolution)
{
    int tid = blockIdx.x * blockDim.x + threadIdx.x;

    int i = tid / resolution.x;  // row num
    int j = tid % resolution.x;  // collum num

    if (i >= resolution.y)
        return;

    func(i, j);
}

#include <iostream>

template<typename F>
void GPUParallelFor(const char* description, int nItems, F func)
{
#ifdef NVTX
    nvtxRangePush(description);
#endif

    // #ifdef _DEBUG
    // std::cerr << "Launching " << std::string(description) << " with size "
    //          << nItems << std::endl;
    // #endif
    if (nItems > 0) {
        auto kernel = &Kernel<F>;

        int blockSize = GetBlockSize(description, kernel);

        int gridSize = (nItems + blockSize - 1) / blockSize;
        kernel<<<gridSize, blockSize>>>(func, nItems);
    }

#ifdef NVTX
    nvtxRangePop();
#endif

    CUDA_SYNC_CHECK();
}

template<typename F>
void GPUParallelFor2D(const char* description, int2 resolution, F func)
{
#ifdef NVTX
    nvtxRangePush(description);
#endif

//#ifdef _DEBUG
//    std::cerr << "Launching " << std::string(description) << " with size ("
//              << resolution.x << ", " << resolution.y << ")" << std::endl;
//#endif

    auto kernel = &Kernel2D<F>;

    int blockSize = GetBlockSize(description, kernel);

    int gridSize = (resolution.x * resolution.y + blockSize - 1) / blockSize;
    kernel<<<gridSize, blockSize>>>(func, resolution);

#ifdef NVTX
    nvtxRangePop();
#endif
}

#endif

#define GPU_LAMBDA(...)    [ =, *this ] __device__(__VA_ARGS__) mutable
#define GPU_LAMBDA_Ex(...) [=] __device__(__VA_ARGS__) mutable
