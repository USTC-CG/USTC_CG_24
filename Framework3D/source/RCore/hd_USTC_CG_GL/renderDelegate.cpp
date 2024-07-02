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
// #

#include "renderDelegate.h"

#include <dxgi1_5.h>

#include <iostream>

#include "Nodes/node_exec.hpp"
#include "Nodes/node_exec_eager.hpp"
#include "Nodes/node_tree.hpp"
#include "Utils/Logging/Logging.h"
#include "config.h"
#include "geometries/mesh.h"
#include "instancer.h"
#include "light.h"
#include "material.h"
#include "nvrhi/d3d12.h"
#include "nvrhi/validation.h"
#include "pxr/imaging/hd/camera.h"
#include "pxr/imaging/hd/extComputation.h"
#include "renderBuffer.h"
#include "renderPass.h"
#include "renderer.h"

#define HR_RETURN(hr) \
    if (FAILED(hr))   \
        assert(false);

USTC_CG_NAMESPACE_OPEN_SCOPE
using namespace pxr;
TF_DEFINE_PUBLIC_TOKENS(
    HdEmbreeRenderSettingsTokens,
    HDEMBREE_RENDER_SETTINGS_TOKENS);

struct MessageCallBack : public nvrhi::IMessageCallback {
    void message(nvrhi::MessageSeverity severity, const char* messageText)
        override
    {
        logging(messageText, Error);
    }

    static MessageCallBack callback;
};
MessageCallBack MessageCallBack::callback;

// Find an adapter whose name contains the given string.
static RefCountPtr<IDXGIAdapter> FindAdapter(const std::wstring& targetName)
{
    RefCountPtr<IDXGIAdapter> targetAdapter;
    RefCountPtr<IDXGIFactory1> DXGIFactory;
    HRESULT hres = CreateDXGIFactory1(IID_PPV_ARGS(&DXGIFactory));
    if (hres != S_OK) {
        logging(
            "ERROR in CreateDXGIFactory.\n"
            "For more info, get log from debug D3D runtime: (1) Install DX "
            "SDK, and enable Debug "
            "D3D from DX Control Panel Utility. (2) Install and start DbgView. "
            "(3) Try running the "
            "program again.\n");
        return targetAdapter;
    }

    unsigned int adapterNo = 0;
    while (SUCCEEDED(hres)) {
        RefCountPtr<IDXGIAdapter> pAdapter;
        hres = DXGIFactory->EnumAdapters(adapterNo, &pAdapter);

        if (SUCCEEDED(hres)) {
            DXGI_ADAPTER_DESC aDesc;
            pAdapter->GetDesc(&aDesc);

            // If no name is specified, return the first adapater.  This is the
            // same behaviour as the default specified for D3D11CreateDevice
            // when no adapter is specified.
            if (targetName.length() == 0) {
                targetAdapter = pAdapter;
                break;
            }

            std::wstring aName = aDesc.Description;

            if (aName.find(targetName) != std::string::npos) {
                targetAdapter = pAdapter;
                break;
            }
        }

        adapterNo++;
    }

    return targetAdapter;
}

const TfTokenVector Hd_USTC_CG_RenderDelegate::SUPPORTED_RPRIM_TYPES = {
    HdPrimTypeTokens->mesh,
};

const TfTokenVector Hd_USTC_CG_RenderDelegate::SUPPORTED_SPRIM_TYPES = {
    HdPrimTypeTokens->camera,      HdPrimTypeTokens->simpleLight,
    HdPrimTypeTokens->sphereLight, HdPrimTypeTokens->domeLight,
    HdPrimTypeTokens->material,
};

const TfTokenVector Hd_USTC_CG_RenderDelegate::SUPPORTED_BPRIM_TYPES = {
    HdPrimTypeTokens->renderBuffer,
};

Hd_USTC_CG_RenderDelegate::Hd_USTC_CG_RenderDelegate() : HdRenderDelegate()
{
    _Initialize();
}

Hd_USTC_CG_RenderDelegate::Hd_USTC_CG_RenderDelegate(
    const HdRenderSettingsMap& settingsMap)
    : HdRenderDelegate(settingsMap)
{
    _Initialize();
}

static void _RenderCallback(
    Hd_USTC_CG_Renderer* renderer,
    HdRenderThread* renderThread)
{
    renderer->Clear();
    renderer->Render(renderThread);
}

std::mutex Hd_USTC_CG_RenderDelegate::_mutexResourceRegistry;
std::atomic_int Hd_USTC_CG_RenderDelegate::_counterResourceRegistry;
HdResourceRegistrySharedPtr Hd_USTC_CG_RenderDelegate::_resourceRegistry;

void Hd_USTC_CG_RenderDelegate::CreateD3DDevice()
{
    nvrhi::d3d12::DeviceDesc deviceDesc;
    deviceDesc.errorCB = &MessageCallBack::callback;
    {
        UINT windowStyle =
            m_DeviceParams.startFullscreen
                ? (WS_POPUP | WS_SYSMENU | WS_VISIBLE)
            : m_DeviceParams.startMaximized
                ? (WS_OVERLAPPEDWINDOW | WS_VISIBLE | WS_MAXIMIZE)
                : (WS_OVERLAPPEDWINDOW | WS_VISIBLE);

        RECT rect = { 0,
                      0,
                      LONG(m_DeviceParams.backBufferWidth),
                      LONG(m_DeviceParams.backBufferHeight) };
        AdjustWindowRect(&rect, windowStyle, FALSE);

        RefCountPtr<IDXGIAdapter> targetAdapter;

        if (m_DeviceParams.adapter) {
            targetAdapter = m_DeviceParams.adapter;
        }
        else {
            targetAdapter = FindAdapter(m_DeviceParams.adapterNameSubstring);

            if (!targetAdapter) {
                std::wstring adapterNameStr(
                    m_DeviceParams.adapterNameSubstring.begin(),
                    m_DeviceParams.adapterNameSubstring.end());
                assert(false);
            }
        }

        {
            DXGI_ADAPTER_DESC aDesc;
            targetAdapter->GetDesc(&aDesc);

            std::wstring adapterName = aDesc.Description;

            // A stupid but non-deprecated and portable way of converting a
            // wstring to a string
            std::stringstream ss;
            std::wstringstream wss;
            for (auto c : adapterName)
                ss << wss.narrow(c, '?');
        }

        HRESULT hr = E_FAIL;

        RECT clientRect;
        GetClientRect(m_hWnd, &clientRect);
        UINT width = clientRect.right - clientRect.left;
        UINT height = clientRect.bottom - clientRect.top;

        if (m_DeviceParams.enableDebugRuntime) {
            RefCountPtr<ID3D12Debug> pDebug;
            hr = D3D12GetDebugInterface(IID_PPV_ARGS(&pDebug));
            HR_RETURN(hr)

            pDebug->EnableDebugLayer();
        }

        RefCountPtr<IDXGIFactory2> pDxgiFactory;
        UINT dxgiFactoryFlags =
            m_DeviceParams.enableDebugRuntime ? DXGI_CREATE_FACTORY_DEBUG : 0;
        hr = CreateDXGIFactory2(dxgiFactoryFlags, IID_PPV_ARGS(&pDxgiFactory));
        HR_RETURN(hr)

        RefCountPtr<IDXGIFactory5> pDxgiFactory5;
        if (SUCCEEDED(
                pDxgiFactory->QueryInterface(IID_PPV_ARGS(&pDxgiFactory5)))) {
            BOOL supported = 0;
            if (SUCCEEDED(pDxgiFactory5->CheckFeatureSupport(
                    DXGI_FEATURE_PRESENT_ALLOW_TEARING,
                    &supported,
                    sizeof(supported))))
                m_TearingSupported = (supported != 0);
        }

        hr = D3D12CreateDevice(
            targetAdapter,
            m_DeviceParams.featureLevel,
            IID_PPV_ARGS(&m_Device12));

        D3D12_COMMAND_QUEUE_DESC queueDesc;
        ZeroMemory(&queueDesc, sizeof(queueDesc));
        queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
        queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
        queueDesc.NodeMask = 1;
        hr = m_Device12->CreateCommandQueue(
            &queueDesc, IID_PPV_ARGS(&m_GraphicsQueue));
        HR_RETURN(hr)
        m_GraphicsQueue->SetName(L"Graphics Queue");

        if (m_DeviceParams.enableComputeQueue) {
            queueDesc.Type = D3D12_COMMAND_LIST_TYPE_COMPUTE;
            hr = m_Device12->CreateCommandQueue(
                &queueDesc, IID_PPV_ARGS(&m_ComputeQueue));
            HR_RETURN(hr)
            m_ComputeQueue->SetName(L"Compute Queue");
        }

        if (m_DeviceParams.enableCopyQueue) {
            queueDesc.Type = D3D12_COMMAND_LIST_TYPE_COPY;
            hr = m_Device12->CreateCommandQueue(
                &queueDesc, IID_PPV_ARGS(&m_CopyQueue));
            HR_RETURN(hr)
            m_CopyQueue->SetName(L"Copy Queue");
        }
    }

    deviceDesc.pDevice = m_Device12;
    deviceDesc.pGraphicsCommandQueue = m_GraphicsQueue;
    deviceDesc.pComputeCommandQueue = m_ComputeQueue;
    deviceDesc.pCopyCommandQueue = m_CopyQueue;
    nvrhi_device = nvrhi::d3d12::createDevice(deviceDesc);

    if (m_DeviceParams.enableNvrhiValidationLayer) {
        nvrhi_device = nvrhi::validation::createValidationLayer(nvrhi_device);
    }
}

void Hd_USTC_CG_RenderDelegate::_Initialize()
{
    // Initialize the settings and settings descriptors.
    _settingDescriptors.resize(5);
    _settingDescriptors[0] = {
        "Enable Scene Colors",
        HdEmbreeRenderSettingsTokens->enableSceneColors,
        VtValue(HdEmbreeConfig::GetInstance().useFaceColors)
    };
    _settingDescriptors[1] = {
        "Enable Ambient Occlusion",
        HdEmbreeRenderSettingsTokens->enableAmbientOcclusion,
        VtValue(HdEmbreeConfig::GetInstance().ambientOcclusionSamples > 0)
    };
    _settingDescriptors[2] = {
        "Ambient Occlusion Samples",
        HdEmbreeRenderSettingsTokens->ambientOcclusionSamples,
        VtValue(static_cast<int>(
            HdEmbreeConfig::GetInstance().ambientOcclusionSamples))
    };
    _settingDescriptors[3] = {
        "Samples To Convergence",
        HdRenderSettingsTokens->convergedSamplesPerPixel,
        VtValue(static_cast<int>(
            HdEmbreeConfig::GetInstance().samplesToConvergence))
    };

    _settingDescriptors[4] = { "Render Mode",
                               HdEmbreeRenderSettingsTokens->renderMode,
                               VtValue(0) };
    _PopulateDefaultSettings(_settingDescriptors);

    CreateD3DDevice();

    _renderParam = std::make_shared<Hd_USTC_CG_RenderParam>(
        &_renderThread,
        &_sceneVersion,
        &lights,
        &cameras,
        &meshes,
        &materials,
        nvrhi_device.Get());

    _renderer = std::make_shared<Hd_USTC_CG_Renderer>(_renderParam.get());

    // Set the background render thread's rendering entrypoint to
    // HdEmbreeRenderer::Render.
    _renderThread.SetRenderCallback(
        std::bind(_RenderCallback, _renderer.get(), &_renderThread));
    _renderThread.StartThread();

    // Initialize one resource registry for all embree plugins
    std::lock_guard<std::mutex> guard(_mutexResourceRegistry);

    if (_counterResourceRegistry.fetch_add(1) == 0) {
        _resourceRegistry = std::make_shared<HdResourceRegistry>();
    }
    _resourceRegistry = std::make_shared<HdResourceRegistry>();
}

HdAovDescriptor Hd_USTC_CG_RenderDelegate::GetDefaultAovDescriptor(
    const TfToken& name) const
{
    logging("Attempting to acquire aov " + name.GetString());

    if (name == HdAovTokens->color) {
        return HdAovDescriptor(
            HdFormatFloat32Vec4, false, VtValue(GfVec4f(0.0f)));
    }
    if (name == HdAovTokens->normal || name == HdAovTokens->Neye) {
        return HdAovDescriptor(
            HdFormatFloat32Vec3, false, VtValue(GfVec3f(-1.0f)));
    }
    if (name == HdAovTokens->depth) {
        return HdAovDescriptor(HdFormatFloat32, false, VtValue(1.0f));
    }
    if (name == HdAovTokens->primId || name == HdAovTokens->instanceId ||
        name == HdAovTokens->elementId) {
        return HdAovDescriptor(HdFormatInt32, false, VtValue(-1));
    }
    HdParsedAovToken aovId(name);
    if (aovId.isPrimvar) {
        return HdAovDescriptor(
            HdFormatFloat32Vec3, false, VtValue(GfVec3f(0.0f)));
    }

    return HdAovDescriptor();
}

Hd_USTC_CG_RenderDelegate::~Hd_USTC_CG_RenderDelegate()
{
    _renderParam->executor->prepare_tree(_renderParam->node_tree);
    for (auto&& node : _renderParam->node_tree->nodes) {
        node->runtime_storage.reset();
    }
    _resourceRegistry.reset();
    _renderer.reset();
    std::cout << "Destroying Tiny RenderDelegate" << std::endl;
}

const TfTokenVector& Hd_USTC_CG_RenderDelegate::GetSupportedRprimTypes() const
{
    return SUPPORTED_RPRIM_TYPES;
}

const TfTokenVector& Hd_USTC_CG_RenderDelegate::GetSupportedSprimTypes() const
{
    return SUPPORTED_SPRIM_TYPES;
}

const TfTokenVector& Hd_USTC_CG_RenderDelegate::GetSupportedBprimTypes() const
{
    return SUPPORTED_BPRIM_TYPES;
}

HdResourceRegistrySharedPtr Hd_USTC_CG_RenderDelegate::GetResourceRegistry()
    const
{
    return _resourceRegistry;
}

void Hd_USTC_CG_RenderDelegate::CommitResources(HdChangeTracker* tracker)
{
}

HdRenderPassSharedPtr Hd_USTC_CG_RenderDelegate::CreateRenderPass(
    HdRenderIndex* index,
    const HdRprimCollection& collection)
{
    std::cout << "Create RenderPass with Collection=" << collection.GetName()
              << std::endl;

    return std::make_shared<Hd_USTC_CG_RenderPass>(
        index, collection, &_renderThread, _renderer.get(), &_sceneVersion);
}

HdRprim* Hd_USTC_CG_RenderDelegate::CreateRprim(
    const TfToken& typeId,
    const SdfPath& rprimId)
{
    std::cout << "Create Rprim type=" << typeId.GetText() << " id=" << rprimId
              << std::endl;

    if (typeId == HdPrimTypeTokens->mesh) {
        auto mesh = new Hd_USTC_CG_Mesh(rprimId);
        meshes.push_back(mesh);
        return mesh;
    }
    TF_CODING_ERROR(
        "Unknown Rprim type=%s id=%s", typeId.GetText(), rprimId.GetText());
    return nullptr;
}

void Hd_USTC_CG_RenderDelegate::DestroyRprim(HdRprim* rPrim)
{
    logging(
        "Destroy Tiny Rprim id=" + rPrim->GetId().GetString(), USTC_CG::Info);
    meshes.erase(
        std::remove(meshes.begin(), meshes.end(), rPrim), meshes.end());
    delete rPrim;
}

HdSprim* Hd_USTC_CG_RenderDelegate::CreateSprim(
    const TfToken& typeId,
    const SdfPath& sprimId)
{
    if (typeId == HdPrimTypeTokens->camera) {
        auto camera = new Hd_USTC_CG_Camera(sprimId);
        cameras.push_back(camera);
        return camera;
    }
    else if (typeId == HdPrimTypeTokens->extComputation) {
        return new HdExtComputation(sprimId);
    }
    else if (typeId == HdPrimTypeTokens->material) {
        auto material = new Hd_USTC_CG_Material(sprimId);
        materials[sprimId] = material;
        return material;
    }
    else if (
        typeId == HdPrimTypeTokens->simpleLight ||
        typeId == HdPrimTypeTokens->sphereLight) {
        auto light = new Hd_USTC_CG_Light(sprimId, typeId);
        lights.push_back(light);
        return light;
    }
    else if (typeId == HdPrimTypeTokens->domeLight) {
        auto light = new Hd_USTC_CG_Dome_Light(sprimId, typeId);
        lights.push_back(light);
        return light;
    }
    else {
        TF_CODING_ERROR("Unknown Sprim Type %s", typeId.GetText());
    }

    return nullptr;
}

HdSprim* Hd_USTC_CG_RenderDelegate::CreateFallbackSprim(const TfToken& typeId)
{
    // For fallback sprims, create objects with an empty scene path.
    // They'll use default values and won't be updated by a scene delegate.
    if (typeId == HdPrimTypeTokens->camera) {
        auto camera = new Hd_USTC_CG_Camera(SdfPath::EmptyPath());
        cameras.push_back(camera);
        return camera;
    }
    else if (typeId == HdPrimTypeTokens->extComputation) {
        return new HdExtComputation(SdfPath::EmptyPath());
    }
    else if (typeId == HdPrimTypeTokens->material) {
        auto material = new Hd_USTC_CG_Material(SdfPath::EmptyPath());
        materials[SdfPath::EmptyPath()] = material;
        return material;
    }
    else if (
        typeId == HdPrimTypeTokens->simpleLight ||
        typeId == HdPrimTypeTokens->sphereLight) {
        auto light = new Hd_USTC_CG_Light(SdfPath::EmptyPath(), typeId);
        lights.push_back(light);
        return light;
    }
    else if (typeId == HdPrimTypeTokens->domeLight) {
        auto light = new Hd_USTC_CG_Dome_Light(SdfPath::EmptyPath(), typeId);
        lights.push_back(light);
        return light;
    }
    else {
        TF_CODING_ERROR("Unknown Sprim Type %s", typeId.GetText());
    }

    return nullptr;
}

void Hd_USTC_CG_RenderDelegate::DestroySprim(HdSprim* sPrim)
{
    logging(sPrim->GetId().GetAsString() + " destroyed", USTC_CG::Info);
    lights.erase(
        std::remove(lights.begin(), lights.end(), sPrim), lights.end());
    cameras.erase(
        std::remove(cameras.begin(), cameras.end(), sPrim), cameras.end());
    materials.erase(sPrim->GetId());
    delete sPrim;
}

HdBprim* Hd_USTC_CG_RenderDelegate::CreateBprim(
    const TfToken& typeId,
    const SdfPath& bprimId)
{
    if (typeId == HdPrimTypeTokens->renderBuffer) {
        logging(
            "Create bprim: type id=" + typeId.GetString() +
                ",prim id = " + bprimId.GetString(),
            USTC_CG::Info);

        return new Hd_USTC_CG_RenderBufferGL(bprimId);
    }
    TF_CODING_ERROR("Unknown Bprim Type %s", typeId.GetText());
    return nullptr;
}

HdBprim* Hd_USTC_CG_RenderDelegate::CreateFallbackBprim(const TfToken& typeId)
{
    if (typeId == HdPrimTypeTokens->renderBuffer) {
        return new Hd_USTC_CG_RenderBufferGL(SdfPath::EmptyPath());
    }
    TF_CODING_ERROR("Unknown Bprim Type %s", typeId.GetText());
    return nullptr;
}

void Hd_USTC_CG_RenderDelegate::DestroyBprim(HdBprim* bPrim)
{
    std::string sentence = "Destroy Bprim";
    auto bprim_name = bPrim->GetId().GetString();
    if (!bprim_name.empty()) {
        sentence += " id=" + bprim_name;
    }
    logging(sentence, USTC_CG::Info);
    delete bPrim;
}

HdInstancer* Hd_USTC_CG_RenderDelegate::CreateInstancer(
    HdSceneDelegate* delegate,
    const SdfPath& id)
{
    return new Hd_USTC_CG_GL_Instancer(delegate, id);
}

void Hd_USTC_CG_RenderDelegate::DestroyInstancer(HdInstancer* instancer)
{
    TF_CODING_ERROR("Destroy instancer not supported");
}

HdRenderParam* Hd_USTC_CG_RenderDelegate::GetRenderParam() const
{
    return _renderParam.get();
}

void Hd_USTC_CG_RenderDelegate::SetRenderSetting(
    const TfToken& key,
    const VtValue& value)
{
    HdRenderDelegate::SetRenderSetting(key, value);
    if (key == TfToken("RenderNodeTree")) {
        _renderParam->node_tree = static_cast<NodeTree*>(value.Get<void*>());
    }
    if (key == TfToken("RenderNodeTreeExecutor")) {
        _renderParam->executor =
            static_cast<NodeTreeExecutor*>(value.Get<void*>());
        auto&& context = entt::locator<entt::meta_ctx>::value_or();
        _renderParam->context = &_renderParam->executor->get_meta_ctx();
    }
}

USTC_CG_NAMESPACE_CLOSE_SCOPE
