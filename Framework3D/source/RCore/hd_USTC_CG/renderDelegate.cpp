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

#include <iostream>

#include "Utils/Logging/Logging.h"
#include "config.h"
#include "geometries/mesh.h"
#include "instancer.h"
#include "light.h"
#include "material.h"
#include "pxr/imaging/hd/camera.h"
#include "pxr/imaging/hd/extComputation.h"
#include "renderBuffer.h"
#include "renderPass.h"
#include "renderer.h"

USTC_CG_NAMESPACE_OPEN_SCOPE
using namespace pxr;
TF_DEFINE_PUBLIC_TOKENS(Hd_USTC_CG_RenderSettingsTokens, HDEMBREE_RENDER_SETTINGS_TOKENS);

const TfTokenVector Hd_USTC_CG_RenderDelegate::SUPPORTED_RPRIM_TYPES = {
    HdPrimTypeTokens->mesh,
};

const TfTokenVector Hd_USTC_CG_RenderDelegate::SUPPORTED_SPRIM_TYPES = {
    HdPrimTypeTokens->camera,    HdPrimTypeTokens->sphereLight, HdPrimTypeTokens->rectLight,
    HdPrimTypeTokens->domeLight, HdPrimTypeTokens->material,

};

const TfTokenVector Hd_USTC_CG_RenderDelegate::SUPPORTED_BPRIM_TYPES = {
    HdPrimTypeTokens->renderBuffer,
};

Hd_USTC_CG_RenderDelegate::Hd_USTC_CG_RenderDelegate() : HdRenderDelegate()
{
    _Initialize();
}

Hd_USTC_CG_RenderDelegate::Hd_USTC_CG_RenderDelegate(const HdRenderSettingsMap& settingsMap)
    : HdRenderDelegate(settingsMap)
{
    _Initialize();
}

static void _RenderCallback(Hd_USTC_CG_Renderer* renderer, HdRenderThread* renderThread)
{
    renderer->Clear();
    renderer->Render(renderThread);
}

std::mutex Hd_USTC_CG_RenderDelegate::_mutexResourceRegistry;
std::atomic_int Hd_USTC_CG_RenderDelegate::_counterResourceRegistry;
HdResourceRegistrySharedPtr Hd_USTC_CG_RenderDelegate::_resourceRegistry;

void Hd_USTC_CG_RenderDelegate::_Initialize()
{
    // Initialize the settings and settings descriptors.
    _settingDescriptors.resize(5);
    _settingDescriptors[0] = { "Enable Scene Colors",
                               Hd_USTC_CG_RenderSettingsTokens->enableSceneColors,
                               VtValue(Hd_USTC_CG_Config::GetInstance().useFaceColors) };
    _settingDescriptors[1] = { "Enable Ambient Occlusion",
                               Hd_USTC_CG_RenderSettingsTokens->enableAmbientOcclusion,
                               VtValue(
                                   Hd_USTC_CG_Config::GetInstance().ambientOcclusionSamples > 0) };
    _settingDescriptors[2] = { "Ambient Occlusion Samples",
                               Hd_USTC_CG_RenderSettingsTokens->ambientOcclusionSamples,
                               VtValue(static_cast<int>(
                                   Hd_USTC_CG_Config::GetInstance().ambientOcclusionSamples)) };
    _settingDescriptors[3] = { "Samples To Convergence",
                               HdRenderSettingsTokens->convergedSamplesPerPixel,
                               VtValue(static_cast<int>(
                                   Hd_USTC_CG_Config::GetInstance().samplesToConvergence)) };

    _settingDescriptors[4] = { "Render Mode",
                               Hd_USTC_CG_RenderSettingsTokens->renderMode,
                               VtValue(0) };
    _PopulateDefaultSettings(_settingDescriptors);

    _renderParam = std::make_shared<Hd_USTC_CG_RenderParam>(&_renderThread, &_sceneVersion);
    _renderParam->lights = &lights;
    _renderParam->materials = &materials;

    _renderer = std::make_shared<Hd_USTC_CG_Renderer>(_renderParam.get());

    // Set the background render thread's rendering entrypoint to
    // Hd_USTC_CG_Renderer::Render.
    _renderThread.SetRenderCallback(std::bind(_RenderCallback, _renderer.get(), &_renderThread));
    _renderThread.StartThread();

    // Initialize one resource registry for all embree plugins
    std::lock_guard<std::mutex> guard(_mutexResourceRegistry);

    if (_counterResourceRegistry.fetch_add(1) == 0) {
        _resourceRegistry = std::make_shared<HdResourceRegistry>();
    }
    _resourceRegistry = std::make_shared<HdResourceRegistry>();
}

HdAovDescriptor Hd_USTC_CG_RenderDelegate::GetDefaultAovDescriptor(const TfToken& name) const
{
    if (name == HdAovTokens->color) {
        return HdAovDescriptor(HdFormatFloat32Vec4, false, VtValue(GfVec4f(0.0f)));
    }
    if (name == HdAovTokens->normal || name == HdAovTokens->Neye) {
        return HdAovDescriptor(HdFormatFloat32Vec3, false, VtValue(GfVec3f(-1.0f)));
    }
    if (name == HdAovTokens->depth) {
        return HdAovDescriptor(HdFormatFloat32, false, VtValue(1.0f));
    }
    if (name == HdAovTokens->cameraDepth) {
        return HdAovDescriptor(HdFormatFloat32, false, VtValue(0.0f));
    }
    if (name == HdAovTokens->primId || name == HdAovTokens->instanceId ||
        name == HdAovTokens->elementId) {
        return HdAovDescriptor(HdFormatInt32, false, VtValue(-1));
    }
    HdParsedAovToken aovId(name);
    if (aovId.isPrimvar) {
        return HdAovDescriptor(HdFormatFloat32Vec3, false, VtValue(GfVec3f(0.0f)));
    }

    return HdAovDescriptor();
}

Hd_USTC_CG_RenderDelegate::~Hd_USTC_CG_RenderDelegate()
{
    _resourceRegistry.reset();
    std::cout << "Destroying RenderDelegate" << std::endl;
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

HdResourceRegistrySharedPtr Hd_USTC_CG_RenderDelegate::GetResourceRegistry() const
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
    std::cout << "Create RenderPass with Collection=" << collection.GetName() << std::endl;

    return std::make_shared<Hd_USTC_CG_RenderPass>(
        index, collection, &_renderThread, _renderer.get(), &_sceneVersion);
}

HdRprim* Hd_USTC_CG_RenderDelegate::CreateRprim(const TfToken& typeId, const SdfPath& rprimId)
{
    std::cout << "Create Rprim type=" << typeId.GetText() << " id=" << rprimId << std::endl;

    if (typeId == HdPrimTypeTokens->mesh) {
        return new Hd_USTC_CG_Mesh(rprimId);
    }
    TF_CODING_ERROR("Unknown Rprim type=%s id=%s", typeId.GetText(), rprimId.GetText());
    return nullptr;
}

void Hd_USTC_CG_RenderDelegate::DestroyRprim(HdRprim* rPrim)
{
    logging("Destroy Rprim id=" + rPrim->GetId().GetString(), USTC_CG::Info);
    delete rPrim;
}

HdSprim* Hd_USTC_CG_RenderDelegate::CreateSprim(const TfToken& typeId, const SdfPath& sprimId)
{
    if (typeId == HdPrimTypeTokens->camera) {
        return new Hd_USTC_CG_Camera(sprimId);
    }
    else if (typeId == HdPrimTypeTokens->extComputation) {
        return new HdExtComputation(sprimId);
    }
    else if (typeId == HdPrimTypeTokens->material) {
        auto material = new Hd_USTC_CG_Material(sprimId);
        materials[sprimId] = material;

        return material;
    }
    else if (typeId == HdPrimTypeTokens->sphereLight) {
        auto light = new Hd_USTC_CG_Sphere_Light(sprimId, typeId);
        lights.push_back(light);
        return light;
    }

    else if (typeId == HdPrimTypeTokens->rectLight) {
        auto light = new Hd_USTC_CG_Rect_Light(sprimId, typeId);
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
        return new HdCamera(SdfPath::EmptyPath());
    }
    else if (typeId == HdPrimTypeTokens->extComputation) {
        return new HdExtComputation(SdfPath::EmptyPath());
    }
    else if (typeId == HdPrimTypeTokens->material) {
        auto material = new Hd_USTC_CG_Material(SdfPath::EmptyPath());
        materials[SdfPath::EmptyPath()] = material;
        return material;
    }
    else if (typeId == HdPrimTypeTokens->sphereLight) {
        return new Hd_USTC_CG_Sphere_Light(SdfPath::EmptyPath(), typeId);
    }

    else if (typeId == HdPrimTypeTokens->rectLight) {
        return new Hd_USTC_CG_Rect_Light(SdfPath::EmptyPath(), typeId);
    }
    else if (typeId == HdPrimTypeTokens->domeLight) {
        return new Hd_USTC_CG_Dome_Light(SdfPath::EmptyPath(), typeId);
    }
    else {
        TF_CODING_ERROR("Unknown Sprim Type %s", typeId.GetText());
    }

    return nullptr;
}

void Hd_USTC_CG_RenderDelegate::DestroySprim(HdSprim* sPrim)
{
    logging(sPrim->GetId().GetAsString() + " destroyed", USTC_CG::Info);
    lights.erase(std::remove(lights.begin(), lights.end(), sPrim), lights.end());
    materials.erase(sPrim->GetId());
    delete sPrim;
}

HdBprim* Hd_USTC_CG_RenderDelegate::CreateBprim(const TfToken& typeId, const SdfPath& bprimId)
{
    if (typeId == HdPrimTypeTokens->renderBuffer) {
        logging(
            "Create bprim: type id=" + typeId.GetString() + ",prim id = " + bprimId.GetString(),
            USTC_CG::Info);

        return new Hd_USTC_CG_RenderBuffer(bprimId);
    }
    TF_CODING_ERROR("Unknown Bprim Type %s", typeId.GetText());
    return nullptr;
}

HdBprim* Hd_USTC_CG_RenderDelegate::CreateFallbackBprim(const TfToken& typeId)
{
    if (typeId == HdPrimTypeTokens->renderBuffer) {
        return new Hd_USTC_CG_RenderBuffer(SdfPath::EmptyPath());
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
    return new Hd_USTC_CG_Instancer(delegate, id);
}

void Hd_USTC_CG_RenderDelegate::DestroyInstancer(HdInstancer* instancer)
{
    TF_CODING_ERROR("Destroy instancer not supported");
}

HdRenderParam* Hd_USTC_CG_RenderDelegate::GetRenderParam() const
{
    return _renderParam.get();
}

USTC_CG_NAMESPACE_CLOSE_SCOPE
