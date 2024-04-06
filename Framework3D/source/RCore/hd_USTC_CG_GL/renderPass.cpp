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
#include "renderPass.h"

#include <iostream>

#include "pxr/imaging/hd/renderBuffer.h"
#include "pxr/imaging/hd/renderDelegate.h"
#include "pxr/imaging/hd/renderPass.h"
#include "pxr/imaging/hd/renderPassState.h"
#include "renderBuffer.h"

USTC_CG_NAMESPACE_OPEN_SCOPE
using namespace pxr;
Hd_USTC_CG_RenderPass::Hd_USTC_CG_RenderPass(
    HdRenderIndex* index,
    const HdRprimCollection& collection,
    HdRenderThread* renderThread,
    Hd_USTC_CG_Renderer* renderer,
    std::atomic<int>* sceneVersion)
    : HdRenderPass(index, collection),
      _renderThread(renderThread),
      _renderer(renderer),
      _sceneVersion(sceneVersion),
      _lastSceneVersion(0),
      _lastSettingsVersion(0)
{
}

Hd_USTC_CG_RenderPass::~Hd_USTC_CG_RenderPass()
{
    std::cout << "Destroying renderPass" << std::endl;
}

static GfRect2i _GetDataWindow(const HdRenderPassStateSharedPtr& renderPassState)
{
    const CameraUtilFraming& framing = renderPassState->GetFraming();
    if (framing.IsValid()) {
        return framing.dataWindow;
    }
    else {
        const GfVec4f vp = renderPassState->GetViewport();
        return GfRect2i(GfVec2i(0), int(vp[2]), int(vp[3]));
    }
}

void Hd_USTC_CG_RenderPass::_Execute(
    const HdRenderPassStateSharedPtr& renderPassState,
    const TfTokenVector& renderTags)
{
    int currentSceneVersion = _sceneVersion->load();
    if (_lastSceneVersion != currentSceneVersion) {
        needStartRender = true;
        _lastSceneVersion = currentSceneVersion;
    }

    // Likewise the render settings.
    HdRenderDelegate* renderDelegate = GetRenderIndex()->GetRenderDelegate();
    int currentSettingsVersion = renderDelegate->GetRenderSettingsVersion();
    if (_lastSettingsVersion != currentSettingsVersion) {
        _renderThread->StopRender();
        _lastSettingsVersion = currentSettingsVersion;

        needStartRender = true;
    }

    // Determine whether we need to update the renderer camera.

    const GfMatrix4d view = renderPassState->GetWorldToViewMatrix();
    const GfMatrix4d proj = renderPassState->GetProjectionMatrix();
    const GfRect2i dataWindow = _GetDataWindow(renderPassState);

    if (_viewMatrix != view || _projMatrix != proj || _dataWindow != dataWindow) {
        _viewMatrix = view;
        _projMatrix = proj;
        _dataWindow = dataWindow;

        _renderThread->StopRender();
        _renderer->renderTimeUpdateCamera(renderPassState);

        needStartRender = true;
    }

    // Determine whether we need to update the renderer AOV bindings.

    HdRenderPassAovBindingVector aovBindings = renderPassState->GetAovBindings();
    if (_aovBindings != aovBindings) {
        _aovBindings = aovBindings;
        _renderThread->StopRender();
        _renderer->SetAovBindings(aovBindings);
        // In general, the render thread clears aov bindings, but make sure
        // they are cleared initially on this thread.
        _renderer->Clear();
        needStartRender = true;
    }

    if (_renderer->nodetree_modified(false)) {
        _renderer->Clear();

        needStartRender = true;
    }

    TF_VERIFY(!_aovBindings.empty(), "No aov bindings to render into");
    // Only start a new render if something in the scene has changed.
    if (needStartRender) {
        _renderer->MarkAovBuffersUnconverged();
        _renderer->Clear();
        _renderer->Render(nullptr);
        //needStartRender = false;
        //_renderThread->StartRender();
    }
    
}

bool Hd_USTC_CG_RenderPass::IsConverged() const
{
    // Otherwise, check the convergence of all attachments.
    for (size_t i = 0; i < _aovBindings.size(); ++i) {
        if (_aovBindings[i].renderBuffer && !_aovBindings[i].renderBuffer->IsConverged()) {
            return false;
        }
    }
    return true;
}

USTC_CG_NAMESPACE_CLOSE_SCOPE
