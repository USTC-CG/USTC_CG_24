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
#ifndef EXTRAS_IMAGING_EXAMPLES_HD_TINY_RENDERER_PLUGIN_H
#define EXTRAS_IMAGING_EXAMPLES_HD_TINY_RENDERER_PLUGIN_H
#include "USTC_CG.h"

#include "pxr/pxr.h"
#include "api.h"
#include "pxr/imaging/hd/rendererPlugin.h"

PXR_NAMESPACE_OPEN_SCOPE
using namespace pxr;

void HD_USTC_CG_API foo();

///
/// \class Hd_USTC_CG_RendererPlugin
///
/// A registered child of HdRendererPlugin, this is the class that gets
/// loaded when a Hydra application asks to draw with a certain renderer.
/// It supports rendering via creation/destruction of renderer-specific
/// classes. The render delegate is the Hydra-facing entrypoint into the
/// renderer; it's responsible for creating specialized implementations of Hydra
/// prims (which translate scene data into drawable representations) and Hydra
/// renderpasses (which draw the scene to the framebuffer).
///
class HD_USTC_CG_API Hd_USTC_CG_RendererPlugin final : public HdRendererPlugin
{
public:
    Hd_USTC_CG_RendererPlugin() = default;
    virtual ~Hd_USTC_CG_RendererPlugin() = default;

    /// Construct a new render delegate of type Hd_USTC_CG_RenderDelegate.
    virtual HdRenderDelegate *CreateRenderDelegate() override;

    /// Construct a new render delegate of type Hd_USTC_CG_RenderDelegate.
    virtual HdRenderDelegate *CreateRenderDelegate(
        HdRenderSettingsMap const& settingsMap) override;

    /// Destroy a render delegate created by this class's CreateRenderDelegate.
    ///   \param renderDelegate The render delegate to delete.
    virtual void DeleteRenderDelegate(
        HdRenderDelegate *renderDelegate) override;

    /// Checks to see if the plugin is supported on the running system.
    virtual bool IsSupported(bool gpuEnabled = true) const override;

private:
    // This class does not support copying.
    Hd_USTC_CG_RendererPlugin(const Hd_USTC_CG_RendererPlugin&) = delete;
    Hd_USTC_CG_RendererPlugin &operator =(const Hd_USTC_CG_RendererPlugin&) = delete;
};

PXR_NAMESPACE_CLOSE_SCOPE

#endif // EXTRAS_IMAGING_EXAMPLES_HD_TINY_RENDERER_PLUGIN_H
