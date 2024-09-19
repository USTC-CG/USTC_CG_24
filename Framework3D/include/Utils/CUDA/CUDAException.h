#pragma once
//------------------------------------------------------------------------------
//
// CUDA error-checking
//
//------------------------------------------------------------------------------
#include <cuda_runtime_api.h>

#include <sstream>

#define CUDA_CHECK(call)                                              \
    do {                                                              \
        cudaError_t error = call;                                     \
        if (error != cudaSuccess) {                                   \
            std::stringstream ss;                                     \
            ss << "CUDA call (" << #call << " ) failed with error: '" \
               << cudaGetErrorString(error) << "' (" __FILE__ << ":"  \
               << __LINE__ << ")\n";                                  \
            printf("%s", ss.str().c_str());                           \
        }                                                             \
    } while (0)

#define OPTIX_CHECK_LOG(call)                                              \
    do {                                                                   \
        OptixResult res = call;                                            \
        const size_t sizeof_log_returned = sizeof_log;                     \
        sizeof_log =                                                       \
            sizeof(optix_log); /* reset sizeof_log for future calls */     \
        if (res != OPTIX_SUCCESS) {                                        \
            std::stringstream ss;                                          \
            ss << "Optix call '" << #call << "' failed: " __FILE__ ":"     \
               << __LINE__ << ")\nLog:\n"                                  \
               << optix_log                                                \
               << (sizeof_log_returned > sizeof(optix_log) ? "<TRUNCATED>" \
                                                           : "")           \
               << "\n";                                                    \
            printf("%s", ss.str().c_str());                                \
        }                                                                  \
    } while (0)

#define OPTIX_CHECK(call)                                              \
    do {                                                               \
        OptixResult res = call;                                        \
        if (res != OPTIX_SUCCESS) {                                    \
            std::stringstream ss;                                      \
            ss << "Optix call '" << #call << "' failed: " __FILE__ ":" \
               << __LINE__ << ")\n";                                   \
            printf("%s", ss.str().c_str());                            \
        }                                                              \
    } while (0)

#define CUDA_SYNC_CHECK()                                            \
    do {                                                             \
        cudaDeviceSynchronize();                                     \
        cudaError_t error = cudaGetLastError();                      \
        if (error != cudaSuccess) {                                  \
            std::stringstream ss;                                    \
            ss << "CUDA error on synchronize with error '"           \
               << cudaGetErrorString(error) << "' (" __FILE__ << ":" \
               << __LINE__ << ")\n";                                 \
            printf("%s", ss.str().c_str());                          \
        }                                                            \
    } while (0)