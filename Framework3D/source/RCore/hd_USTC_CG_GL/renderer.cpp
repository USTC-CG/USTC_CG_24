#include "renderer.h"

#include "Nodes/node_tree.hpp"
#include "RCore/Backend.hpp"
#include "pxr/imaging/hd/renderBuffer.h"
#include "pxr/imaging/hd/tokens.h"
#include "renderBuffer.h"
#include "renderParam.h"

USTC_CG_NAMESPACE_OPEN_SCOPE
using namespace pxr;

Hd_USTC_CG_Renderer::Hd_USTC_CG_Renderer(Hd_USTC_CG_RenderParam* render_param)
    : _enableSceneColors(false),
      render_param(render_param)
{
}

void Hd_USTC_CG_Renderer::Render(HdRenderThread* renderThread)
{
    

    _completedSamples.store(0);

    // Commit any pending changes to the scene.

    if (!_ValidateAovBindings()) {
        for (size_t i = 0; i < _aovBindings.size(); ++i) {
            auto rb = static_cast<Hd_USTC_CG_RenderBufferGL*>(_aovBindings[i].renderBuffer);
            rb->SetConverged(true);
        }
        // XXX:validation
        TF_WARN("Could not validate Aovs. Render will not complete");
        return;
    }

    // Fill the nodes that requires value from the scene.
    auto& executor = render_param->executor;
    auto& node_tree = render_param->node_tree;
    

    executor->prepare_tree(node_tree);
    

    for (auto&& node : node_tree->nodes) {
        auto try_fill_info = [&node, &executor](const char* id_name, void* data) {
            if (std::string(node->typeinfo->id_name) == id_name) {
                assert(node->outputs.size() == 1);
                auto output_socket = node->outputs[0];
                executor->sync_node_from_external_storage(output_socket, data);
            }
        };
        try_fill_info("render_scene_lights", render_param->lights);
        try_fill_info("render_scene_camera", render_param->cameras);
        try_fill_info("render_scene_meshes", render_param->meshes);
        try_fill_info("render_scene_materials", render_param->materials);
    }
    executor->execute_tree(node_tree);
    

    TextureHandle texture = nullptr;
    for (auto&& node : node_tree->nodes) {
        auto try_fetch_info = [&node, &executor](const char* id_name, void* data) {
            if (std::string(node->typeinfo->id_name) == id_name) {
                assert(node->inputs.size() == 1);
                auto output_socket = node->inputs[0];
                executor->sync_node_to_external_storage(output_socket, data);
            }
        };
        try_fetch_info("render_present", &texture);
        if (texture) {
            break;
        }
    }
    

    if (texture) {
        for (size_t i = 0; i < _aovBindings.size(); ++i) {
            auto rb = static_cast<Hd_USTC_CG_RenderBufferGL*>(_aovBindings[i].renderBuffer);
            rb->Present(texture->texture_id);
    

            rb->SetConverged(true);
        }
    }
    

    executor->finalize(node_tree);
    

}

void Hd_USTC_CG_Renderer::Clear()
{
    if (!_ValidateAovBindings()) {
        return;
    }

    for (size_t i = 0; i < _aovBindings.size(); ++i) {
        if (_aovBindings[i].clearValue.IsEmpty()) {
            continue;
        }

        auto rb = static_cast<Hd_USTC_CG_RenderBufferGL*>(_aovBindings[i].renderBuffer);

        rb->Map();
        if (_aovNames[i].name == HdAovTokens->color) {
            GfVec4f clearColor = _GetClearColor(_aovBindings[i].clearValue);

            rb->Clear(clearColor.data());
        }
        else if (rb->GetFormat() == HdFormatInt32) {
            int32_t clearValue = _aovBindings[i].clearValue.Get<int32_t>();
            rb->Clear(&clearValue);
        }
        else if (rb->GetFormat() == HdFormatFloat32) {
            float clearValue = _aovBindings[i].clearValue.Get<float>();
            rb->Clear(&clearValue);
        }
        else if (rb->GetFormat() == HdFormatFloat32Vec3) {
            auto clearValue = _aovBindings[i].clearValue.Get<GfVec3f>();
            rb->Clear(clearValue.data());
        }  // else, _ValidateAovBindings would have already warned.

        rb->Unmap();
        rb->SetConverged(false);
    }
}

/* static */
GfVec4f Hd_USTC_CG_Renderer::_GetClearColor(const VtValue& clearValue)
{
    HdTupleType type = HdGetValueTupleType(clearValue);
    if (type.count != 1) {
        return GfVec4f(0.0f, 0.0f, 0.0f, 1.0f);
    }

    switch (type.type) {
        case HdTypeFloatVec3: {
            GfVec3f f = *(static_cast<const GfVec3f*>(HdGetValueData(clearValue)));
            return GfVec4f(f[0], f[1], f[2], 1.0f);
        }
        case HdTypeFloatVec4: {
            GfVec4f f = *(static_cast<const GfVec4f*>(HdGetValueData(clearValue)));
            return f;
        }
        case HdTypeDoubleVec3: {
            GfVec3d f = *(static_cast<const GfVec3d*>(HdGetValueData(clearValue)));
            return GfVec4f(f[0], f[1], f[2], 1.0f);
        }
        case HdTypeDoubleVec4: {
            GfVec4d f = *(static_cast<const GfVec4d*>(HdGetValueData(clearValue)));
            return GfVec4f(f);
        }
        default: return GfVec4f(0.0f, 0.0f, 0.0f, 1.0f);
    }
}

void Hd_USTC_CG_Renderer::SetAovBindings(const HdRenderPassAovBindingVector& aovBindings)
{
    _aovBindings = aovBindings;
    _aovNames.resize(_aovBindings.size());
    for (size_t i = 0; i < _aovBindings.size(); ++i) {
        _aovNames[i] = HdParsedAovToken(_aovBindings[i].aovName);
    }

    // Re-validate the attachments.
    _aovBindingsNeedValidation = true;
}

void Hd_USTC_CG_Renderer::MarkAovBuffersUnconverged()
{
    for (size_t i = 0; i < _aovBindings.size(); ++i) {
        auto rb = static_cast<Hd_USTC_CG_RenderBufferGL*>(_aovBindings[i].renderBuffer);
        rb->SetConverged(false);
    }
}

void Hd_USTC_CG_Renderer::renderTimeUpdateCamera(const HdRenderPassStateSharedPtr& renderPassState)
{
    camera_ = static_cast<const Hd_USTC_CG_Camera*>(renderPassState->GetCamera());
    camera_->update(renderPassState);
}

bool Hd_USTC_CG_Renderer::nodetree_modified()
{
    return render_param->node_tree->GetDirty();
}

bool Hd_USTC_CG_Renderer::nodetree_modified(bool new_status)
{
    auto old_status = render_param->node_tree->GetDirty();
    render_param->node_tree->SetDirty(new_status);
    return old_status;
}

bool Hd_USTC_CG_Renderer::_ValidateAovBindings()
{
    if (!_aovBindingsNeedValidation) {
        return _aovBindingsValid;
    }

    _aovBindingsNeedValidation = false;
    _aovBindingsValid = true;

    for (size_t i = 0; i < _aovBindings.size(); ++i) {
        // By the time the attachment gets here, there should be a bound
        // output buffer.
        if (_aovBindings[i].renderBuffer == nullptr) {
            TF_WARN("Aov '%s' doesn't have any renderbuffer bound", _aovNames[i].name.GetText());
            _aovBindingsValid = false;
            continue;
        }

        if (_aovNames[i].name != HdAovTokens->color &&
            _aovNames[i].name != HdAovTokens->cameraDepth &&
            _aovNames[i].name != HdAovTokens->depth && _aovNames[i].name != HdAovTokens->primId &&
            _aovNames[i].name != HdAovTokens->instanceId &&
            _aovNames[i].name != HdAovTokens->elementId && _aovNames[i].name != HdAovTokens->Neye &&
            _aovNames[i].name != HdAovTokens->normal && !_aovNames[i].isPrimvar) {
            TF_WARN(
                "Unsupported attachment with Aov '%s' won't be rendered to",
                _aovNames[i].name.GetText());
        }

        HdFormat format = _aovBindings[i].renderBuffer->GetFormat();

        // depth is only supported for float32 attachments
        if ((_aovNames[i].name == HdAovTokens->cameraDepth ||
             _aovNames[i].name == HdAovTokens->depth) &&
            format != HdFormatFloat32) {
            TF_WARN(
                "Aov '%s' has unsupported format '%s'",
                _aovNames[i].name.GetText(),
                TfEnum::GetName(format).c_str());
            _aovBindingsValid = false;
        }

        // ids are only supported for int32 attachments
        if ((_aovNames[i].name == HdAovTokens->primId ||
             _aovNames[i].name == HdAovTokens->instanceId ||
             _aovNames[i].name == HdAovTokens->elementId) &&
            format != HdFormatInt32) {
            TF_WARN(
                "Aov '%s' has unsupported format '%s'",
                _aovNames[i].name.GetText(),
                TfEnum::GetName(format).c_str());
            _aovBindingsValid = false;
        }

        // Normal is only supported for vec3 attachments of float.
        if ((_aovNames[i].name == HdAovTokens->Neye || _aovNames[i].name == HdAovTokens->normal) &&
            format != HdFormatFloat32Vec3) {
            TF_WARN(
                "Aov '%s' has unsupported format '%s'",
                _aovNames[i].name.GetText(),
                TfEnum::GetName(format).c_str());
            _aovBindingsValid = false;
        }

        // Primvars support vec3 output (though some channels may not be
        // used).
        if (_aovNames[i].isPrimvar && format != HdFormatFloat32Vec3) {
            TF_WARN(
                "Aov 'primvars:%s' has unsupported format '%s'",
                _aovNames[i].name.GetText(),
                TfEnum::GetName(format).c_str());
            _aovBindingsValid = false;
        }

        // color is only supported for vec3/vec4 attachments of float,
        // unorm, or snorm.
        if (_aovNames[i].name == HdAovTokens->color) {
            switch (format) {
                case HdFormatUNorm8Vec4:
                case HdFormatUNorm8Vec3:
                case HdFormatSNorm8Vec4:
                case HdFormatSNorm8Vec3:
                case HdFormatFloat32Vec4:
                case HdFormatFloat32Vec3: break;
                default:
                    TF_WARN(
                        "Aov '%s' has unsupported format '%s'",
                        _aovNames[i].name.GetText(),
                        TfEnum::GetName(format).c_str());
                    _aovBindingsValid = false;
                    break;
            }
        }

        // make sure the clear value is reasonable for the format of the
        // attached buffer.
        if (!_aovBindings[i].clearValue.IsEmpty()) {
            HdTupleType clearType = HdGetValueTupleType(_aovBindings[i].clearValue);

            // array-valued clear types aren't supported.
            if (clearType.count != 1) {
                TF_WARN(
                    "Aov '%s' clear value type '%s' is an array",
                    _aovNames[i].name.GetText(),
                    _aovBindings[i].clearValue.GetTypeName().c_str());
                _aovBindingsValid = false;
            }

            // color only supports float/double vec3/4
            if (_aovNames[i].name == HdAovTokens->color && clearType.type != HdTypeFloatVec3 &&
                clearType.type != HdTypeFloatVec4 && clearType.type != HdTypeDoubleVec3 &&
                clearType.type != HdTypeDoubleVec4) {
                TF_WARN(
                    "Aov '%s' clear value type '%s' isn't compatible",
                    _aovNames[i].name.GetText(),
                    _aovBindings[i].clearValue.GetTypeName().c_str());
                _aovBindingsValid = false;
            }

            // only clear float formats with float, int with int, float3
            // with float3.
            if ((format == HdFormatFloat32 && clearType.type != HdTypeFloat) ||
                (format == HdFormatInt32 && clearType.type != HdTypeInt32) ||
                (format == HdFormatFloat32Vec3 && clearType.type != HdTypeFloatVec3)) {
                TF_WARN(
                    "Aov '%s' clear value type '%s' isn't compatible with"
                    " format %s",
                    _aovNames[i].name.GetText(),
                    _aovBindings[i].clearValue.GetTypeName().c_str(),
                    TfEnum::GetName(format).c_str());
                _aovBindingsValid = false;
            }
        }
    }

    return _aovBindingsValid;
};

USTC_CG_NAMESPACE_CLOSE_SCOPE
