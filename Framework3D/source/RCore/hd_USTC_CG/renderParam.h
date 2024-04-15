//
// Copyright 2017 Pixar
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
#ifndef PXR_IMAGING_PLUGIN_HD_EMBREE_RENDER_PARAM_H
#define PXR_IMAGING_PLUGIN_HD_EMBREE_RENDER_PARAM_H
#include <embree4/rtcore.h>

#include "USTC_CG.h"
#include "pxr/imaging/hd/renderDelegate.h"
#include "pxr/imaging/hd/renderThread.h"
#include "pxr/pxr.h"

USTC_CG_NAMESPACE_OPEN_SCOPE
class Hd_USTC_CG_Light;
class Hd_USTC_CG_Material;
using namespace pxr;

///
/// \class Hd_USTC_CG_RenderParam
///
/// The render delegate can create an object of type HdRenderParam, to pass
/// to each prim during Sync(). Hd_USTC_CG_ uses this class to pass top-level
/// embree state around.
///
class Hd_USTC_CG_RenderParam final : public HdRenderParam {
   public:
    Hd_USTC_CG_RenderParam(HdRenderThread *renderThread, std::atomic<int> *sceneVersion)
        : _renderThread(renderThread),
          _sceneVersion(sceneVersion)
    {
    }

    /// Accessor for the top-level embree scene.
    RTCScene AcquireSceneForEdit()
    {
        _renderThread->StopRender();
        (*_sceneVersion)++;
        return _scene;
    }
    /// Accessor for the top-level embree device (library handle).
    RTCDevice GetEmbreeDevice()
    {
        return _device;
    }

    friend class Hd_USTC_CG_Renderer;
    pxr::TfHashMap<SdfPath, Hd_USTC_CG_Material *, TfHash> *materials = nullptr;
    pxr::VtArray<Hd_USTC_CG_Light *> *lights = nullptr;

   private:
    /// A handle to the top-level embree scene.
    RTCScene _scene = nullptr;
    /// A handle to the top-level embree device (library handle).
    RTCDevice _device = nullptr;
    /// A handle to the global render thread.
    HdRenderThread *_renderThread = nullptr;
    /// A version counter for edits to _scene.
    std::atomic<int> *_sceneVersion;
};

USTC_CG_NAMESPACE_CLOSE_SCOPE

#endif  // PXR_IMAGING_PLUGIN_HD_EMBREE_RENDER_PARAM_H
