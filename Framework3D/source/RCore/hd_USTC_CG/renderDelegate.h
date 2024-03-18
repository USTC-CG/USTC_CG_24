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

#include "renderParam.h"
#include "renderer.h"
#include "pxr/pxr.h"
#include "pxr/imaging/hd/renderDelegate.h"
#include "pxr/imaging/hd/resourceRegistry.h"
#include "pxr/base/tf/staticTokens.h"

PXR_NAMESPACE_OPEN_SCOPE
#define HDEMBREE_RENDER_SETTINGS_TOKENS \
    (enableAmbientOcclusion)(enableSceneColors)(ambientOcclusionSamples)
// Also: HdRenderSettingsTokens->convergedSamplesPerPixel

TF_DECLARE_PUBLIC_TOKENS(
    HdEmbreeRenderSettingsTokens,
    HDEMBREE_RENDER_SETTINGS_TOKENS);

///
/// \class Hd_USTC_CG_RenderDelegate
///
/// Render delegates provide renderer-specific functionality to the render
/// index, the main hydra state management structure. The render index uses
/// the render delegate to create and delete scene primitives, which include
/// geometry and also non-drawable objects. The render delegate is also
/// responsible for creating renderpasses, which know how to draw this
/// renderer's scene primitives.
///
class Hd_USTC_CG_RenderDelegate final : public HdRenderDelegate
{
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

    HdInstancer* CreateInstancer(
        HdSceneDelegate* delegate,
        const SdfPath& id) override;
    void DestroyInstancer(HdInstancer* instancer) override;

    HdRprim* CreateRprim(
        const TfToken& typeId,
        const SdfPath& rprimId) override;
    void DestroyRprim(HdRprim* rPrim) override;

    HdSprim* CreateSprim(
        const TfToken& typeId,
        const SdfPath& sprimId) override;
    HdSprim* CreateFallbackSprim(const TfToken& typeId) override;
    void DestroySprim(HdSprim* sprim) override;

    HdBprim* CreateBprim(
        const TfToken& typeId,
        const SdfPath& bprimId) override;
    HdBprim* CreateFallbackBprim(const TfToken& typeId) override;
    void DestroyBprim(HdBprim* bprim) override;

    void CommitResources(HdChangeTracker* tracker) override;

    HdRenderParam* GetRenderParam() const override;

private:
    static const TfTokenVector SUPPORTED_RPRIM_TYPES;
    static const TfTokenVector SUPPORTED_SPRIM_TYPES;
    static const TfTokenVector SUPPORTED_BPRIM_TYPES;

    void _Initialize();

    // Handle for an embree "device", or library state.
    RTCDevice _rtcDevice;

    // Handle for the top-level embree scene, mirroring the Hydra scene.
    RTCScene _rtcScene;

    // A version counter for edits to _scene.
    std::atomic<int> _sceneVersion;

    // A shared HdEmbreeRenderParam object that stores top-level embree state;
    // passed to prims during Sync().
    std::shared_ptr<HdEmbreeRenderParam> _renderParam;

    // A background render thread for running the actual renders in. The
    // render thread object manages synchronization between the scene data
    // and the background-threaded renderer.
    HdRenderThread _renderThread;

    // An renderer object, to perform the actual raytracing.
    std::shared_ptr<Hd_USTC_CG_Renderer> _renderer;

    /// Resource registry used in this render delegate
    static std::mutex _mutexResourceRegistry;
    static std::atomic_int _counterResourceRegistry;
    static HdResourceRegistrySharedPtr _resourceRegistry;

    // This class does not support copying.
    Hd_USTC_CG_RenderDelegate(const Hd_USTC_CG_RenderDelegate&) = delete;
    Hd_USTC_CG_RenderDelegate& operator =(const Hd_USTC_CG_RenderDelegate&)
    = delete;

public:
    HdAovDescriptor
    GetDefaultAovDescriptor(const TfToken& name) const override;

private:
    // A list of render setting exports.
    HdRenderSettingDescriptorList _settingDescriptors;
    // A callback that interprets embree error codes and injects them into
    // the hydra logging system.
    static void HandleRtcError(void* userPtr, RTCError code, const char* msg);
};


PXR_NAMESPACE_CLOSE_SCOPE

#endif // EXTRAS_IMAGING_EXAMPLES_HD_TINY_RENDER_DELEGATE_H
