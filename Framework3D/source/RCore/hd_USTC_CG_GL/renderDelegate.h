//
// Copyright 2020 Pixar
//
// Licensed under the Apache License, Version 2.0 (the "Apache License")
// with the following modification; you may not use this file except in
// compliance with the Apache License and the following modification to it:
// Section 6. Trademarks. is deleted and replaced with:
//
// 6. Trademarks. This License does not grant permission to use the trade
//    names, trademarks, service marks, or product names of the Licensor
//    and its affiliates, except as required to comply with Section 4(c) of
//    the License and to reproduce the content of the NOTICE file.
//
// You may obtain a copy of the Apache License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the Apache License with the above modification is
// distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
// KIND, either express or implied. See the Apache License for the specific
// language governing permissions and limitations under the Apache License.
//
#ifndef EXTRAS_IMAGING_EXAMPLES_HD_TINY_RENDER_DELEGATE_H
#define EXTRAS_IMAGING_EXAMPLES_HD_TINY_RENDER_DELEGATE_H

#include <dxgi.h>
#include <dxgi1_2.h>

#include "Nodes/node_exec.hpp"
#include "Nodes/node_tree.hpp"
#include "nvrhi/d3d12.h"
#include "pxr/base/tf/staticTokens.h"
#include "pxr/imaging/hd/renderDelegate.h"
#include "pxr/pxr.h"
#include "renderParam.h"
#include "renderer.h"

USTC_CG_NAMESPACE_OPEN_SCOPE
class Hd_USTC_CG_Material;
class Hd_USTC_CG_Light;
using namespace pxr;
#define HDEMBREE_RENDER_SETTINGS_TOKENS \
    (enableAmbientOcclusion)(enableSceneColors)(ambientOcclusionSamples)(renderMode)
// Also: HdRenderSettingsTokens->convergedSamplesPerPixel

TF_DECLARE_PUBLIC_TOKENS(HdEmbreeRenderSettingsTokens, HDEMBREE_RENDER_SETTINGS_TOKENS);
using nvrhi::RefCountPtr;

class Hd_USTC_CG_RenderDelegate final : public HdRenderDelegate {
   public:
    /// Render delegate constructor.
    Hd_USTC_CG_RenderDelegate();
    /// Render delegate constructor.
    Hd_USTC_CG_RenderDelegate(const HdRenderSettingsMap& settingsMap);
    /// Render delegate destructor.
    ~Hd_USTC_CG_RenderDelegate() override;

    /// Supported types
    const TfTokenVector& GetSupportedRprimTypes() const override;
    const TfTokenVector& GetSupportedSprimTypes() const override;
    const TfTokenVector& GetSupportedBprimTypes() const override;

    // Basic value to return from the RD
    HdResourceRegistrySharedPtr GetResourceRegistry() const override;

    // Prims
    HdRenderPassSharedPtr CreateRenderPass(
        HdRenderIndex* index,
        const HdRprimCollection& collection) override;

    HdInstancer* CreateInstancer(HdSceneDelegate* delegate, const SdfPath& id) override;
    void DestroyInstancer(HdInstancer* instancer) override;

    HdRprim* CreateRprim(const TfToken& typeId, const SdfPath& rprimId) override;
    void DestroyRprim(HdRprim* rPrim) override;

    HdSprim* CreateSprim(const TfToken& typeId, const SdfPath& sprimId) override;
    HdSprim* CreateFallbackSprim(const TfToken& typeId) override;
    void DestroySprim(HdSprim* sprim) override;

    HdBprim* CreateBprim(const TfToken& typeId, const SdfPath& bprimId) override;
    HdBprim* CreateFallbackBprim(const TfToken& typeId) override;
    void DestroyBprim(HdBprim* bprim) override;

    void CommitResources(HdChangeTracker* tracker) override;

    HdRenderParam* GetRenderParam() const override;
    void SetRenderSetting(const TfToken& key, const VtValue& value) override;

   private:
    static const TfTokenVector SUPPORTED_RPRIM_TYPES;
    static const TfTokenVector SUPPORTED_SPRIM_TYPES;
    static const TfTokenVector SUPPORTED_BPRIM_TYPES;

    void _Initialize();

    std::atomic<int> _sceneVersion;
    std::shared_ptr<Hd_USTC_CG_RenderParam> _renderParam;
    HdRenderThread _renderThread;
    std::shared_ptr<Hd_USTC_CG_Renderer> _renderer;
    std::unique_ptr<NodeTreeExecutor> executor;
    pxr::VtArray<Hd_USTC_CG_Light*> lights;
    pxr::VtArray<Hd_USTC_CG_Camera*> cameras;
    pxr::TfHashMap<SdfPath, Hd_USTC_CG_Material*, TfHash> materials;
    pxr::VtArray<Hd_USTC_CG_Mesh*> meshes;
    nvrhi::d3d12::DeviceHandle nvrhi_device;

    static std::mutex _mutexResourceRegistry;
    static std::atomic_int _counterResourceRegistry;
    static HdResourceRegistrySharedPtr _resourceRegistry;

    Hd_USTC_CG_RenderDelegate(const Hd_USTC_CG_RenderDelegate&) = delete;
    Hd_USTC_CG_RenderDelegate& operator=(const Hd_USTC_CG_RenderDelegate&) = delete;

   public:
    HdAovDescriptor GetDefaultAovDescriptor(const TfToken& name) const override;

   private:
    // A list of render setting exports.
    HdRenderSettingDescriptorList _settingDescriptors;

   private:

#define USE_DX12 1
    struct DeviceCreationParameters {
        bool startMaximized = false;
        bool startFullscreen = false;
        bool allowModeSwitch = true;
        int windowPosX = -1;  // -1 means use default placement
        int windowPosY = -1;
        uint32_t backBufferWidth = 1280;
        uint32_t backBufferHeight = 720;
        uint32_t refreshRate = 0;
        uint32_t swapChainBufferCount = 3;
        nvrhi::Format swapChainFormat = nvrhi::Format::SRGBA8_UNORM;
        uint32_t swapChainSampleCount = 1;
        uint32_t swapChainSampleQuality = 0;
        uint32_t maxFramesInFlight = 2;
        bool enableDebugRuntime = false;
        bool enableNvrhiValidationLayer = false;
        bool vsyncEnabled = false;
        bool enableRayTracingExtensions = false;  // for vulkan
        bool enableComputeQueue = false;
        bool enableCopyQueue = false;


#if USE_DX11 || USE_DX12
        // Adapter to create the device on. Setting this to non-null overrides adapterNameSubstring.
        // If device creation fails on the specified adapter, it will *not* try any other adapters.
        IDXGIAdapter* adapter = nullptr;
#endif

        // For use in the case of multiple adapters; only effective if 'adapter' is null. If this is
        // non-null, device creation will try to match the given string against an adapter name.  If
        // the specified string exists as a sub-string of the adapter name, the device and window
        // will be created on that adapter.  Case sensitive.
        std::wstring adapterNameSubstring = L"";

        // set to true to enable DPI scale factors to be computed per monitor
        // this will keep the on-screen window size in pixels constant
        //
        // if set to false, the DPI scale factors will be constant but the system
        // may scale the contents of the window based on DPI
        //
        // note that the backbuffer size is never updated automatically; if the app
        // wishes to scale up rendering based on DPI, then it must set this to true
        // and respond to DPI scale factor changes by resizing the backbuffer explicitly
        bool enablePerMonitorDPI = false;

#if USE_DX11 || USE_DX12
        DXGI_USAGE swapChainUsage = DXGI_USAGE_SHADER_INPUT | DXGI_USAGE_RENDER_TARGET_OUTPUT;
        D3D_FEATURE_LEVEL featureLevel = D3D_FEATURE_LEVEL_11_1;
#endif

#if USE_VK
        std::vector<std::string> requiredVulkanInstanceExtensions;
        std::vector<std::string> requiredVulkanDeviceExtensions;
        std::vector<std::string> requiredVulkanLayers;
        std::vector<std::string> optionalVulkanInstanceExtensions;
        std::vector<std::string> optionalVulkanDeviceExtensions;
        std::vector<std::string> optionalVulkanLayers;
        std::vector<size_t> ignoredVulkanValidationMessageLocations;
        std::function<void(vk::DeviceCreateInfo&)> deviceCreateInfoCallback;
#endif
    };
    DeviceCreationParameters m_DeviceParams;

    RefCountPtr<ID3D12Device> m_Device12;
    RefCountPtr<ID3D12CommandQueue> m_GraphicsQueue;
    RefCountPtr<ID3D12CommandQueue> m_ComputeQueue;
    RefCountPtr<ID3D12CommandQueue> m_CopyQueue;
    RefCountPtr<IDXGIAdapter> m_DxgiAdapter;
    HWND m_hWnd = nullptr;
    bool m_TearingSupported = false;
};

USTC_CG_NAMESPACE_CLOSE_SCOPE

#endif  // EXTRAS_IMAGING_EXAMPLES_HD_TINY_RENDER_DELEGATE_H
