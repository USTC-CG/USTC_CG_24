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
#include "rendererPlugin.h"

#include "pxr/imaging/hd/rendererPluginRegistry.h"
#include "renderDelegate.h"

PXR_NAMESPACE_OPEN_SCOPE
using namespace pxr;

// Register the plugin with the renderer plugin system.
TF_REGISTRY_FUNCTION(TfType)
{
    HdRendererPluginRegistry::Define<Hd_USTC_CG_GL_RendererPlugin>();
}

HdRenderDelegate* Hd_USTC_CG_GL_RendererPlugin::CreateRenderDelegate()
{
    return new USTC_CG::Hd_USTC_CG_RenderDelegate();
}

HdRenderDelegate* Hd_USTC_CG_GL_RendererPlugin::CreateRenderDelegate(
    HdRenderSettingsMap const& settingsMap)
{
    return new USTC_CG::Hd_USTC_CG_RenderDelegate(settingsMap);
}

void Hd_USTC_CG_GL_RendererPlugin::DeleteRenderDelegate(HdRenderDelegate* renderDelegate)
{
    delete renderDelegate;
}

bool Hd_USTC_CG_GL_RendererPlugin::IsSupported(bool /* gpuEnabled */) const
{
    // Nothing more to check for now, we assume if the plugin loads correctly
    // it is supported.
    return true;
}

PXR_NAMESPACE_CLOSE_SCOPE
