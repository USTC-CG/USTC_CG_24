
#include <cuda_runtime_api.h>
#include <driver_types.h>

#include <codecvt>
#ifdef _WIN64
#define VK_USE_PLATFORM_WIN32_KHR
#include <d3d12.h>
#else
using HANDLE = int;
#endif

#include <cuda.h>

#include <unordered_map>

#include "Utils/CUDA/CUDAException.h"
#include "nvrhi/nvrhi.h"

inline std::wstring string_2_wstring(const std::string& s)
{
    std::wstring_convert<std::codecvt_utf8<wchar_t>> cvt;
    std::wstring ws = cvt.from_bytes(s);
    return ws;
}

static int idx = 0;

// Here the resourcetype could be texture or buffer now.
template<typename ResourceType>
HANDLE getSharedApiHandle(nvrhi::IDevice* device, ResourceType* texture_handle)
{
    return texture_handle->getNativeObject(nvrhi::ObjectTypes::SharedHandle);
}

struct BitInformation {
    int redBits;
    int greenBits;
    int blueBits;
    int alphaBits;
    int depthBits;
    int stencilBits;
};

static std::unordered_map<nvrhi::Format, BitInformation> formatBitsInfo = {
    { nvrhi::Format::UNKNOWN,
      {
          0,
          0,
          0,
          0,
          0,
          0,
      } },
    { nvrhi::Format::R8_UINT,
      {
          8,
          0,
          0,
          0,
          0,
          0,
      } },
    { nvrhi::Format::RG8_UINT,
      {
          8,
          8,
          0,
          0,
          0,
          0,
      } },
    { nvrhi::Format::RG8_UNORM,
      {
          8,
          8,
          0,
          0,
          0,
          0,
      } },
    { nvrhi::Format::R16_UINT,
      {
          16,
          0,
          0,
          0,
          0,
          0,
      } },
    { nvrhi::Format::R16_UNORM,
      {
          16,
          0,
          0,
          0,
          0,
          0,
      } },
    { nvrhi::Format::R16_FLOAT,
      {
          16,
          0,
          0,
          0,
          0,
          0,
      } },
    { nvrhi::Format::RGBA8_UNORM,
      {
          8,
          8,
          8,
          8,
          0,
          0,
      } },
    { nvrhi::Format::RGBA8_SNORM,
      {
          8,
          8,
          8,
          8,
          0,
          0,
      } },
    { nvrhi::Format::BGRA8_UNORM,
      {
          8,
          8,
          8,
          8,
          0,
          0,
      } },
    { nvrhi::Format::SRGBA8_UNORM,
      {
          8,
          8,
          8,
          8,
          0,
          0,
      } },
    { nvrhi::Format::SBGRA8_UNORM,
      {
          8,
          8,
          8,
          8,
          0,
          0,
      } },
    { nvrhi::Format::R10G10B10A2_UNORM,
      {
          10,
          10,
          10,
          2,
          0,
          0,
      } },
    { nvrhi::Format::R11G11B10_FLOAT,
      {
          11,
          11,
          10,
          0,
          0,
          0,
      } },
    { nvrhi::Format::RG16_UINT,
      {
          16,
          16,
          0,
          0,
          0,
          0,
      } },
    { nvrhi::Format::RG16_FLOAT,
      {
          16,
          16,
          0,
          0,
          0,
          0,
      } },
    { nvrhi::Format::R32_UINT,
      {
          32,
          0,
          0,
          0,
          0,
          0,
      } },
    { nvrhi::Format::R32_FLOAT,
      {
          32,
          0,
          0,
          0,
          0,
          0,
      } },
    { nvrhi::Format::RGBA16_FLOAT,
      {
          16,
          16,
          16,
          16,
          0,
          0,
      } },
    { nvrhi::Format::RGBA16_UNORM,
      {
          16,
          16,
          16,
          16,
          0,
          0,
      } },
    { nvrhi::Format::RGBA16_SNORM,
      {
          16,
          16,
          16,
          16,
          0,
          0,
      } },
    { nvrhi::Format::RG32_UINT,
      {
          32,
          32,
          0,
          0,
          0,
          0,
      } },
    { nvrhi::Format::RG32_FLOAT,
      {
          32,
          32,
          0,
          0,
          0,
          0,
      } },
    { nvrhi::Format::RGB32_UINT,
      {
          32,
          32,
          32,
          0,
          0,
          0,
      } },
    { nvrhi::Format::RGB32_FLOAT,
      {
          32,
          32,
          32,
          0,
          0,
          0,
      } },
    { nvrhi::Format::RGBA32_UINT,
      {
          32,
          32,
          32,
          32,
          0,
          0,
      } },
    { nvrhi::Format::RGBA32_FLOAT,
      {
          32,
          32,
          32,
          32,
          0,
          0,
      } }
};

void FetchD3DMemory(
    nvrhi::IResource* resource_handle,
    nvrhi::IDevice* device,
    size_t& actualSize,
    HANDLE sharedHandle,
    cudaExternalMemoryHandleDesc& externalMemoryHandleDesc)
{
#ifdef _WIN64
    ID3D12Resource* resource =
        resource_handle->getNativeObject(nvrhi::ObjectTypes::D3D12_Resource);
    ID3D12Device* native_device =
        device->getNativeObject(nvrhi::ObjectTypes::D3D12_Device);

    D3D12_RESOURCE_ALLOCATION_INFO d3d12ResourceAllocationInfo;

    D3D12_RESOURCE_DESC texture_desc = resource->GetDesc();

    d3d12ResourceAllocationInfo =
        native_device->GetResourceAllocationInfo(0, 1, &texture_desc);
    actualSize = d3d12ResourceAllocationInfo.SizeInBytes;

    externalMemoryHandleDesc.type = cudaExternalMemoryHandleTypeD3D12Resource;
    externalMemoryHandleDesc.handle.win32.handle = sharedHandle;
    externalMemoryHandleDesc.size = actualSize;
    externalMemoryHandleDesc.flags = cudaExternalMemoryDedicated;
#else
    throw std::runtime_error("D3D12 in Windows only.");
#endif
}

cudaExternalMemory_t FetchExternalTextureMemory(
    nvrhi::ITexture* image_handle,
    nvrhi::IDevice* device,
    size_t& actualSize,
    HANDLE sharedHandle)
{
    cudaExternalMemoryHandleDesc externalMemoryHandleDesc;
    memset(&externalMemoryHandleDesc, 0, sizeof(externalMemoryHandleDesc));

    if (device->getGraphicsAPI() == nvrhi::GraphicsAPI::D3D12) {
        FetchD3DMemory(
            image_handle,
            device,
            actualSize,
            sharedHandle,
            externalMemoryHandleDesc);
    }
    else if (device->getGraphicsAPI() == nvrhi::GraphicsAPI::VULKAN) {
#ifdef VULKAN_ENABLED
        vk::Device native_device =
            VkDevice(device->getNativeObject(nvrhi::ObjectTypes::VK_Device));
        VkImage image =
            image_handle->getNativeObject(nvrhi::ObjectTypes::VK_Image);

        vk::MemoryRequirements vkMemoryRequirements = {};
        native_device.getImageMemoryRequirements(image, &vkMemoryRequirements);

        FetchVulkanMemory(
            actualSize,
            sharedHandle,
            externalMemoryHandleDesc,
            vkMemoryRequirements);
#endif
    }

    cudaExternalMemory_t externalMemory;
    CUDA_CHECK(
        cudaImportExternalMemory(&externalMemory, &externalMemoryHandleDesc));
    return externalMemory;
}

#ifdef VULKAN_ENABLED
void FetchVulkanMemory(
    size_t& actualSize,
    HANDLE sharedHandle,
    cudaExternalMemoryHandleDesc& externalMemoryHandleDesc,
    vk::MemoryRequirements vkMemoryRequirements)
{
    actualSize = vkMemoryRequirements.size;
#ifdef _WIN64
    externalMemoryHandleDesc.type = cudaExternalMemoryHandleTypeOpaqueWin32;
    externalMemoryHandleDesc.handle.win32.handle = sharedHandle;
#else
    externalMemoryHandleDesc.type = cudaExternalMemoryHandleTypeOpaqueFd;
    externalMemoryHandleDesc.handle.fd = sharedHandle;
#endif
    externalMemoryHandleDesc.size = actualSize;
    externalMemoryHandleDesc.flags = 0;
}

#endif

cudaExternalMemory_t FetchExternalBufferMemory(
    nvrhi::IBuffer* buffer_handle,
    nvrhi::IDevice* device,
    size_t& actualSize,
    HANDLE sharedHandle)
{
    cudaExternalMemoryHandleDesc externalMemoryHandleDesc;
    memset(&externalMemoryHandleDesc, 0, sizeof(externalMemoryHandleDesc));

    if (device->getGraphicsAPI() == nvrhi::GraphicsAPI::D3D12) {
        FetchD3DMemory(
            buffer_handle,
            device,
            actualSize,
            sharedHandle,
            externalMemoryHandleDesc);
    }
    else if (device->getGraphicsAPI() == nvrhi::GraphicsAPI::VULKAN) {
#ifdef VULKAN_ENABLED
        vk::Device native_device =
            VkDevice(device->getNativeObject(nvrhi::ObjectTypes::VK_Device));
        VkBuffer buffer =
            buffer_handle->getNativeObject(nvrhi::ObjectTypes::VK_Buffer);

        vk::MemoryRequirements vkMemoryRequirements = {};
        native_device.getBufferMemoryRequirements(
            buffer, &vkMemoryRequirements);
        FetchVulkanMemory(
            actualSize,
            sharedHandle,
            externalMemoryHandleDesc,
            vkMemoryRequirements);
#endif
    }

    cudaExternalMemory_t externalMemory;
    CUDA_CHECK(
        cudaImportExternalMemory(&externalMemory, &externalMemoryHandleDesc));
    return externalMemory;
}

bool importBufferToBuffer(
    nvrhi::IBuffer* buffer_handle,
    void*& devPtr,
    uint32_t cudaUsageFlags,
    nvrhi::IDevice* device)
{
    HANDLE sharedHandle = getSharedApiHandle(device, buffer_handle);
    if (sharedHandle == NULL) {
        throw std::runtime_error(
            "FalcorCUDA::importTextureToMipmappedArray - texture shared handle "
            "creation failed");
        return false;
    }

    size_t actualSize;

    cudaExternalMemory_t externalMemory = FetchExternalBufferMemory(
        buffer_handle, device, actualSize, sharedHandle);

    cudaExternalMemoryBufferDesc externalMemBufferDesc;
    memset(&externalMemBufferDesc, 0, sizeof(externalMemBufferDesc));

    externalMemBufferDesc.offset = 0;
    externalMemBufferDesc.size = actualSize;
    externalMemBufferDesc.flags = cudaUsageFlags;
    CUDA_SYNC_CHECK();
    CUDA_CHECK(cudaExternalMemoryGetMappedBuffer(
        &devPtr, externalMemory, &externalMemBufferDesc));

    return true;
}

bool importTextureToBuffer(
    nvrhi::ITexture* image_handle,
    void*& devPtr,
    uint32_t cudaUsageFlags,
    nvrhi::IDevice* device)
{
    HANDLE sharedHandle = getSharedApiHandle(device, image_handle);
    if (sharedHandle == NULL) {
        throw std::runtime_error(
            "FalcorCUDA::importTextureToMipmappedArray - texture shared handle "
            "creation failed");
        return false;
    }

    size_t actualSize;

    cudaExternalMemory_t externalMemory = FetchExternalTextureMemory(
        image_handle, device, actualSize, sharedHandle);

    cudaExternalMemoryBufferDesc externalMemBufferDesc;
    memset(&externalMemBufferDesc, 0, sizeof(externalMemBufferDesc));

    externalMemBufferDesc.offset = 0;
    externalMemBufferDesc.size = actualSize;
    externalMemBufferDesc.flags = cudaUsageFlags;

    CUDA_CHECK(cudaExternalMemoryGetMappedBuffer(
        &devPtr, externalMemory, &externalMemBufferDesc));

    return true;
}

bool importTextureToMipmappedArray(
    nvrhi::ITexture* image_handle,
    cudaMipmappedArray_t& mipmappedArray,
    uint32_t cudaUsageFlags,
    nvrhi::IDevice* device)
{
    HANDLE sharedHandle = getSharedApiHandle(device, image_handle);
    if (sharedHandle == NULL) {
        throw std::runtime_error(
            "FalcorCUDA::importTextureToMipmappedArray - texture shared handle "
            "creation failed");
        return false;
    }

    size_t actualSize;

    cudaExternalMemory_t externalMemory = FetchExternalTextureMemory(
        image_handle, device, actualSize, sharedHandle);

    cudaExternalMemoryMipmappedArrayDesc mipDesc;
    memset(&mipDesc, 0, sizeof(mipDesc));

    nvrhi::Format format = image_handle->getDesc().format;
    mipDesc.formatDesc.x = formatBitsInfo[format].redBits;
    mipDesc.formatDesc.y = formatBitsInfo[format].greenBits;
    mipDesc.formatDesc.z = formatBitsInfo[format].blueBits;
    mipDesc.formatDesc.w = formatBitsInfo[format].alphaBits;
    mipDesc.formatDesc.f =
        (nvrhi::getFormatInfo(format).kind == nvrhi::FormatKind::Float)
            ? cudaChannelFormatKindFloat
            : cudaChannelFormatKindUnsigned;

    mipDesc.extent.depth = 0;
    mipDesc.extent.width = image_handle->getDesc().width;
    mipDesc.extent.height = image_handle->getDesc().height;
    mipDesc.flags = cudaUsageFlags;
    mipDesc.numLevels = 1;
    mipDesc.offset = 0;

    CUDA_CHECK(cudaExternalMemoryGetMappedMipmappedArray(
        &mipmappedArray, externalMemory, &mipDesc));

    // CloseHandle(sharedHandle);
    return true;
}

CUsurfObject mapTextureToSurface(
    nvrhi::ITexture* image_handle,
    uint32_t cudaUsageFlags,
    nvrhi::IDevice* device)
{
    // Create a mipmapped array from the texture
    cudaMipmappedArray_t mipmap;

    if (!importTextureToMipmappedArray(
            image_handle, mipmap, cudaUsageFlags, device)) {
        throw std::runtime_error(
            "Failed to import texture into a mipmapped array");
        return 0;
    }

    // Grab level 0
    cudaArray_t cudaArray;
    CUDA_CHECK(cudaGetMipmappedArrayLevel(&cudaArray, mipmap, 0));

    // Create cudaSurfObject_t from CUDA array
    cudaResourceDesc resDesc;
    memset(&resDesc, 0, sizeof(resDesc));
    resDesc.res.array.array = cudaArray;
    resDesc.resType = cudaResourceTypeArray;

    cudaSurfaceObject_t surface;
    CUDA_CHECK(cudaCreateSurfaceObject(&surface, &resDesc));
    return surface;
}

CUtexObject mapTextureToCudaTex(
    nvrhi::ITexture* image_handle,
    uint32_t cudaUsageFlags,
    nvrhi::IDevice* device)
{
    // Create a mipmapped array from the texture
    cudaMipmappedArray_t mipmap;

    if (!importTextureToMipmappedArray(
            image_handle, mipmap, cudaUsageFlags, device)) {
        throw std::runtime_error(
            "Failed to import texture into a mipmapped array");
        return 0;
    }

    // Grab level 0
    cudaArray_t cudaArray;
    CUDA_CHECK(cudaGetMipmappedArrayLevel(&cudaArray, mipmap, 0));

    // Create cudaSurfObject_t from CUDA array
    cudaResourceDesc resDesc;
    memset(&resDesc, 0, sizeof(resDesc));
    resDesc.res.mipmap.mipmap = mipmap;
    resDesc.resType = cudaResourceTypeMipmappedArray;

    cudaTextureObject_t texture;
    auto desc = image_handle->getDesc();
    auto formatInfo = nvrhi::getFormatInfo(desc.format);
    auto mipLevels = image_handle->getDesc().mipLevels;

    cudaTextureDesc texDescr;
    memset(&texDescr, 0, sizeof(cudaTextureDesc));
    texDescr.normalizedCoords = true;
    texDescr.filterMode = cudaFilterModeLinear;
    texDescr.mipmapFilterMode = cudaFilterModeLinear;

    texDescr.addressMode[0] = cudaAddressModeWrap;
    texDescr.addressMode[1] = cudaAddressModeWrap;

    texDescr.sRGB = formatInfo.isSRGB;

    texDescr.maxMipmapLevelClamp = float(mipLevels - 1);
    texDescr.readMode = cudaReadModeNormalizedFloat;

    CUDA_CHECK(cudaCreateTextureObject(&texture, &resDesc, &texDescr, nullptr));
    return texture;
}

CUdeviceptr mapTextureToCUDABuffer(
    nvrhi::ITexture* pTex,
    uint32_t cudaUsageFlags,
    nvrhi::IDevice* device)
{
    // Create a mipmapped array from the texture

    void* devicePtr;
    if (!importTextureToBuffer(pTex, devicePtr, cudaUsageFlags, device)) {
        throw std::runtime_error("Failed to import texture into a buffer");
    }

    return (CUdeviceptr)devicePtr;
}

CUdeviceptr mapBufferToCUDABuffer(
    nvrhi::IBuffer* pBuf,
    uint32_t cudaUsageFlags,
    nvrhi::IDevice* device)
{
    // Create a mipmapped array from the texture

    void* devicePtr;
    if (!importBufferToBuffer(pBuf, devicePtr, cudaUsageFlags, device)) {
        throw std::runtime_error("Failed to import texture into a buffer");
    }

    return (CUdeviceptr)devicePtr;
}
