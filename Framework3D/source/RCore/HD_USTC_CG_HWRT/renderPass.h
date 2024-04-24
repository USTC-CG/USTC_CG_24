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
#ifndef EXTRAS_IMAGING_EXAMPLES_HD_TINY_RENDER_PASS_H
#define EXTRAS_IMAGING_EXAMPLES_HD_TINY_RENDER_PASS_H

#include "renderBuffer.h"
#include "renderer.h"
#include "pxr/pxr.h"
#include "pxr/imaging/hd/renderPass.h"
#include "pxr/imaging/hd/renderThread.h"

USTC_CG_NAMESPACE_OPEN_SCOPE
using namespace pxr;
/// \class Hd_USTC_CG_RenderPass
///
/// HdRenderPass represents a single render iteration, rendering a view of the
/// scene (the HdRprimCollection) for a specific viewer (the camera/viewport
/// parameters in HdRenderPassState) to the current draw target.
///
class Hd_USTC_CG_RenderPass final : public HdRenderPass
{
public:
    /// Renderpass constructor.
    ///   \param index The render index containing scene data to render.
    ///   \param collection The initial rprim collection for this renderpass.
    ///   \param renderThread A handle to the global render thread.
    ///   \param renderer A handle to the global renderer.
    Hd_USTC_CG_RenderPass(
        HdRenderIndex* index,
        const HdRprimCollection& collection,
        HdRenderThread* renderThread,
        Hd_USTC_CG_Renderer* renderer,
        std::atomic<int>* sceneVersion);
    /// Renderpass destructor.
    ~Hd_USTC_CG_RenderPass() override;

protected:
    /// Draw the scene with the bound renderpass state.
    ///   \param renderPassState Input parameters (including viewer parameters)
    ///                          for this renderpass.
    ///   \param renderTags Which rendertags should be drawn this pass.
    void _Execute(
        const HdRenderPassStateSharedPtr& renderPassState,
        const TfTokenVector& renderTags) override;

public:
    bool IsConverged() const override;

protected:
    // The list of aov buffers this renderpass should write to.
    HdRenderPassAovBindingVector _aovBindings;
    // A handle to the render thread.
    HdRenderThread* _renderThread;
    Hd_USTC_CG_Renderer* _renderer;
    std::atomic<int>* _sceneVersion;
    int _lastSceneVersion;
    int _lastSettingsVersion;
    // The view matrix: world space to camera space
    GfMatrix4d _viewMatrix;
    // The projection matrix: camera space to NDC space (with
    // respect to the data window).
    GfMatrix4d _projMatrix;
    GfRect2i _dataWindow;
};

USTC_CG_NAMESPACE_CLOSE_SCOPE

#endif // EXTRAS_IMAGING_EXAMPLES_HD_TINY_RENDER_PASS_H
