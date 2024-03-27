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
//#

#include "renderDelegate.h"
#include "geometries/mesh.h"
#include "renderPass.h"

#include <iostream>

#include "config.h"
#include "instancer.h"
#include "light.h"
#include "renderBuffer.h"
#include "renderer.h"
#include "rendererEmbree.h"
#include "Utils/Logging/Logging.h"
#include "pxr/imaging/hd/extComputation.h"
#include "pxr/imaging/hd/camera.h"

PXR_NAMESPACE_OPEN_SCOPE
TF_DEFINE_PUBLIC_TOKENS(
    HdEmbreeRenderSettingsTokens,
    HDEMBREE_RENDER_SETTINGS_TOKENS);


const TfTokenVector Hd_USTC_CG_RenderDelegate::SUPPORTED_RPRIM_TYPES =
{
    HdPrimTypeTokens->mesh,
};

const TfTokenVector Hd_USTC_CG_RenderDelegate::SUPPORTED_SPRIM_TYPES =
{
    HdPrimTypeTokens->camera,
    HdPrimTypeTokens->simpleLight,
    HdPrimTypeTokens->sphereLight,
};

const TfTokenVector Hd_USTC_CG_RenderDelegate::SUPPORTED_BPRIM_TYPES =
{
    HdPrimTypeTokens->renderBuffer,
};

Hd_USTC_CG_RenderDelegate::Hd_USTC_CG_RenderDelegate()
    : HdRenderDelegate()
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


void
Hd_USTC_CG_RenderDelegate::_Initialize()
{
    // Initialize the settings and settings descriptors.
    _settingDescriptors.resize(4);
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
        VtValue(
            static_cast<int>(HdEmbreeConfig::GetInstance().
                ambientOcclusionSamples))
    };
    _settingDescriptors[3] = {
        "Samples To Convergence",
        HdRenderSettingsTokens->convergedSamplesPerPixel,
        VtValue(
            static_cast<int>(HdEmbreeConfig::GetInstance().
                samplesToConvergence))
    };
    _PopulateDefaultSettings(_settingDescriptors);

    // Initialize the embree library handle (_rtcDevice).
    _rtcDevice = rtcNewDevice(nullptr);

    // Register our error message callback.
    rtcSetDeviceErrorFunction(_rtcDevice, HandleRtcError, NULL);

    // Create the top-level scene.
    //
    // RTC_SCENE_DYNAMIC indicates we'll be updating the scene between draw
    // calls. RTC_INTERSECT1 indicates we'll be casting single rays, and
    // RTC_INTERPOLATE indicates we'll be storing primvars in embree objects
    // and querying them with rtcInterpolate.
    //
    // XXX: Investigate ray packets.
    _rtcScene = rtcNewScene(_rtcDevice);
    // RTC_SCENE_FLAG_DYNAMIC: Provides better build performance for dynamic
    // scenes (but also higher memory consumption).
    rtcSetSceneFlags(_rtcScene, RTC_SCENE_FLAG_DYNAMIC);

    // RTC_BUILD_QUALITY_LOW: Create lower quality data structures,
    // e.g. for dynamic scenes. A two-level spatial index structure is built
    // when enabling this mode, which supports fast partial scene updates,
    // and allows for setting a per-geometry build quality through
    // the rtcSetGeometryBuildQuality function.
    rtcSetSceneBuildQuality(_rtcScene, RTC_BUILD_QUALITY_LOW);

    _renderParam = std::make_shared<HdEmbreeRenderParam>(
        _rtcDevice,
        _rtcScene,
        &_renderThread,
        &_sceneVersion);

    _renderer = std::make_shared<Hd_USTC_CG_Renderer_Embree>();

    // Pass the scene handle to the renderer.
    _renderer->SetScene(_rtcScene);

    // Set the background render thread's rendering entrypoint to
    // HdEmbreeRenderer::Render.
    _renderThread.SetRenderCallback(
        std::bind(_RenderCallback, _renderer.get(), &_renderThread));
    _renderThread.StartThread();

    // Initialize one resource registry for all embree plugins
    std::lock_guard<std::mutex> guard(_mutexResourceRegistry);

    if (_counterResourceRegistry.fetch_add(1) == 0)
    {
        _resourceRegistry = std::make_shared<HdResourceRegistry>();
    }
    _resourceRegistry = std::make_shared<HdResourceRegistry>();
}

HdAovDescriptor Hd_USTC_CG_RenderDelegate::GetDefaultAovDescriptor(
    const TfToken& name) const
{
    if (name == HdAovTokens->color)
    {
        return HdAovDescriptor(
            HdFormatUNorm8Vec4,
            true,
            VtValue(GfVec4f(0.0f)));
    }
    if (name == HdAovTokens->normal || name == HdAovTokens->Neye)
    {
        return HdAovDescriptor(
            HdFormatFloat32Vec3,
            false,
            VtValue(GfVec3f(-1.0f)));
    }
    if (name == HdAovTokens->depth)
    {
        return HdAovDescriptor(HdFormatFloat32, false, VtValue(1.0f));
    }
    if (name == HdAovTokens->cameraDepth)
    {
        return HdAovDescriptor(HdFormatFloat32, false, VtValue(0.0f));
    }
    if (
        name == HdAovTokens->primId || name == HdAovTokens->instanceId ||
        name == HdAovTokens->elementId)
    {
        return HdAovDescriptor(HdFormatInt32, false, VtValue(-1));
    }
    HdParsedAovToken aovId(name);
    if (aovId.isPrimvar)
    {
        return HdAovDescriptor(
            HdFormatFloat32Vec3,
            false,
            VtValue(GfVec3f(0.0f)));
    }

    return HdAovDescriptor();
}

void Hd_USTC_CG_RenderDelegate::HandleRtcError(
    void* userPtr,
    RTCError code,
    const char* msg)
{
    // Forward RTC error messages through to hydra logging.
    switch (code)
    {
        case RTC_ERROR_UNKNOWN:
            TF_CODING_ERROR("Embree unknown error: %s", msg);
            break;
        case RTC_ERROR_INVALID_ARGUMENT:
            TF_CODING_ERROR("Embree invalid argument: %s", msg);
            break;
        case RTC_ERROR_INVALID_OPERATION:
            TF_CODING_ERROR("Embree invalid operation: %s", msg);
            break;
        case RTC_ERROR_OUT_OF_MEMORY:
            TF_CODING_ERROR("Embree out of memory: %s", msg);
            break;
        case RTC_ERROR_UNSUPPORTED_CPU:
            TF_CODING_ERROR("Embree unsupported CPU: %s", msg);
            break;
        case RTC_ERROR_CANCELLED:
            TF_CODING_ERROR("Embree cancelled: %s", msg);
            break;
        default: TF_CODING_ERROR("Embree invalid error code: %s", msg);
            break;
    }
}

Hd_USTC_CG_RenderDelegate::~Hd_USTC_CG_RenderDelegate()
{
    _resourceRegistry.reset();
    std::cout << "Destroying Tiny RenderDelegate" << std::endl;
}

const TfTokenVector&
Hd_USTC_CG_RenderDelegate::GetSupportedRprimTypes() const
{
    return SUPPORTED_RPRIM_TYPES;
}

const TfTokenVector&
Hd_USTC_CG_RenderDelegate::GetSupportedSprimTypes() const
{
    return SUPPORTED_SPRIM_TYPES;
}

const TfTokenVector&
Hd_USTC_CG_RenderDelegate::GetSupportedBprimTypes() const
{
    return SUPPORTED_BPRIM_TYPES;
}

HdResourceRegistrySharedPtr
Hd_USTC_CG_RenderDelegate::GetResourceRegistry() const
{
    return _resourceRegistry;
}

void
Hd_USTC_CG_RenderDelegate::CommitResources(HdChangeTracker* tracker)
{
}

HdRenderPassSharedPtr
Hd_USTC_CG_RenderDelegate::CreateRenderPass(
    HdRenderIndex* index,
    const HdRprimCollection& collection)
{
    std::cout << "Create RenderPass with Collection="
        << collection.GetName() << std::endl;

    return std::make_shared<Hd_USTC_CG_RenderPass>(
        index,
        collection,
        &_renderThread,
        _renderer.get(),
        &_sceneVersion);
}

HdRprim*
Hd_USTC_CG_RenderDelegate::CreateRprim(
    const TfToken& typeId,
    const SdfPath& rprimId)
{
    std::cout << "Create Tiny Rprim type=" << typeId.GetText()
        << " id=" << rprimId
        << std::endl;

    if (typeId == HdPrimTypeTokens->mesh)
    {
        return new Hd_USTC_CG_Mesh(rprimId);
    }
    TF_CODING_ERROR(
        "Unknown Rprim type=%s id=%s",
        typeId.GetText(),
        rprimId.GetText());
    return nullptr;
}

void
Hd_USTC_CG_RenderDelegate::DestroyRprim(HdRprim* rPrim)
{
    logging(
        "Destroy Tiny Rprim id=" + rPrim->GetId().GetString(),
        USTC_CG::Info);
    delete rPrim;
}

HdSprim*
Hd_USTC_CG_RenderDelegate::CreateSprim(
    const TfToken& typeId,
    const SdfPath& sprimId)
{
    if (typeId == HdPrimTypeTokens->camera)
    {
        return new Hd_USTC_CG_Camera(sprimId);
    }
    else if (typeId == HdPrimTypeTokens->extComputation)
    {
        return new HdExtComputation(sprimId);
    }
    else if (
        typeId == HdPrimTypeTokens->simpleLight ||
        typeId == HdPrimTypeTokens->sphereLight)
    {
        return new Hd_USTC_CG_Light(sprimId, typeId);
    }
    else
    {
        TF_CODING_ERROR("Unknown Sprim Type %s", typeId.GetText());
    }

    return nullptr;
}

HdSprim*
Hd_USTC_CG_RenderDelegate::CreateFallbackSprim(const TfToken& typeId)
{
    // For fallback sprims, create objects with an empty scene path.
    // They'll use default values and won't be updated by a scene delegate.
    if (typeId == HdPrimTypeTokens->camera)
    {
        return new HdCamera(SdfPath::EmptyPath());
    }
    else if (typeId == HdPrimTypeTokens->extComputation)
    {
        return new HdExtComputation(SdfPath::EmptyPath());
    }
    else if (
        typeId == HdPrimTypeTokens->simpleLight ||
        typeId == HdPrimTypeTokens->sphereLight)
    {
        return new Hd_USTC_CG_Light(SdfPath::EmptyPath(), typeId);
    }
    else
    {
        TF_CODING_ERROR("Unknown Sprim Type %s", typeId.GetText());
    }

    return nullptr;
}

void
Hd_USTC_CG_RenderDelegate::DestroySprim(HdSprim* sPrim)
{
    logging(sPrim->GetId().GetAsString() + " destroyed", USTC_CG::Info);
    delete sPrim;
}

HdBprim*
Hd_USTC_CG_RenderDelegate::CreateBprim(
    const TfToken& typeId,
    const SdfPath& bprimId)
{
    if (typeId == HdPrimTypeTokens->renderBuffer)
    {
        logging(
            "Create bprim: type id=" + typeId.GetString() + ",prim id = " +
            bprimId.GetString(),
            USTC_CG::Info);

        return new Hd_USTC_CG_RenderBuffer(bprimId);
    }
    TF_CODING_ERROR("Unknown Bprim Type %s", typeId.GetText());
    return nullptr;
}

HdBprim*
Hd_USTC_CG_RenderDelegate::CreateFallbackBprim(const TfToken& typeId)
{
    if (typeId == HdPrimTypeTokens->renderBuffer)
    {
        return new Hd_USTC_CG_RenderBuffer(SdfPath::EmptyPath());
    }
    TF_CODING_ERROR("Unknown Bprim Type %s", typeId.GetText());
    return nullptr;
}

void
Hd_USTC_CG_RenderDelegate::DestroyBprim(HdBprim* bPrim)
{
    std::string sentence = "Destroy Bprim";
    auto bprim_name = bPrim->GetId().GetString();
    if (!bprim_name.empty())
    {
        sentence += " id=" + bprim_name;
    }
    logging(sentence, USTC_CG::Info);
    delete bPrim;
}

HdInstancer*
Hd_USTC_CG_RenderDelegate::CreateInstancer(
    HdSceneDelegate* delegate,
    const SdfPath& id)
{
    return new HdEmbreeInstancer(delegate, id);
}

void
Hd_USTC_CG_RenderDelegate::DestroyInstancer(HdInstancer* instancer)
{
    TF_CODING_ERROR("Destroy instancer not supported");
}

HdRenderParam*
Hd_USTC_CG_RenderDelegate::GetRenderParam() const
{
    return _renderParam.get();
}

PXR_NAMESPACE_CLOSE_SCOPE
