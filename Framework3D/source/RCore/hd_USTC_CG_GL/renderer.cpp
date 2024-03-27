#include "renderer.h"

#include "renderBuffer.h"
#include "pxr/imaging/hd/renderBuffer.h"
#include "pxr/imaging/hd/tokens.h"

PXR_NAMESPACE_OPEN_SCOPE
void Hd_USTC_CG_Renderer::SetAovBindings(
    const HdRenderPassAovBindingVector& aovBindings)
{
    _aovBindings = aovBindings;
    _aovNames.resize(_aovBindings.size());
    for (size_t i = 0; i < _aovBindings.size(); ++i)
    {
        _aovNames[i] = HdParsedAovToken(_aovBindings[i].aovName);
    }

    // Re-validate the attachments.
    _aovBindingsNeedValidation = true;
}

void Hd_USTC_CG_Renderer::MarkAovBuffersUnconverged()
{
    for (size_t i = 0; i < _aovBindings.size(); ++i)
    {
        auto rb =
            static_cast<Hd_USTC_CG_RenderBuffer*>(
                _aovBindings[i].renderBuffer);
        rb->SetConverged(false);
    }
}


void Hd_USTC_CG_Renderer::renderTimeUpdateCamera(
    const HdRenderPassStateSharedPtr& renderPassState)
{
    camera_ =
        static_cast<const Hd_USTC_CG_Camera*>(renderPassState->GetCamera());
    camera_->update(renderPassState);
}

void Hd_USTC_CG_Renderer::SetScene(RTCScene scene)
{
    _scene = scene;
}


bool Hd_USTC_CG_Renderer::_ValidateAovBindings()
{
    if (!_aovBindingsNeedValidation)
    {
        return _aovBindingsValid;
    }

    _aovBindingsNeedValidation = false;
    _aovBindingsValid = true;

    for (size_t i = 0; i < _aovBindings.size(); ++i)
    {
        // By the time the attachment gets here, there should be a bound
        // output buffer.
        if (_aovBindings[i].renderBuffer == nullptr)
        {
            TF_WARN(
                "Aov '%s' doesn't have any renderbuffer bound",
                _aovNames[i].name.GetText());
            _aovBindingsValid = false;
            continue;
        }

        if (_aovNames[i].name != HdAovTokens->color &&
            _aovNames[i].name != HdAovTokens->cameraDepth &&
            _aovNames[i].name != HdAovTokens->depth &&
            _aovNames[i].name != HdAovTokens->primId &&
            _aovNames[i].name != HdAovTokens->instanceId &&
            _aovNames[i].name != HdAovTokens->elementId &&
            _aovNames[i].name != HdAovTokens->Neye &&
            _aovNames[i].name != HdAovTokens->normal &&
            !_aovNames[i].isPrimvar)
        {
            TF_WARN(
                "Unsupported attachment with Aov '%s' won't be rendered to",
                _aovNames[i].name.GetText());
        }

        HdFormat format = _aovBindings[i].renderBuffer->GetFormat();

        // depth is only supported for float32 attachments
        if ((_aovNames[i].name == HdAovTokens->cameraDepth ||
             _aovNames[i].name == HdAovTokens->depth) &&
            format != HdFormatFloat32)
        {
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
            format != HdFormatInt32)
        {
            TF_WARN(
                "Aov '%s' has unsupported format '%s'",
                _aovNames[i].name.GetText(),
                TfEnum::GetName(format).c_str());
            _aovBindingsValid = false;
        }

        // Normal is only supported for vec3 attachments of float.
        if ((_aovNames[i].name == HdAovTokens->Neye ||
             _aovNames[i].name == HdAovTokens->normal) &&
            format != HdFormatFloat32Vec3)
        {
            TF_WARN(
                "Aov '%s' has unsupported format '%s'",
                _aovNames[i].name.GetText(),
                TfEnum::GetName(format).c_str());
            _aovBindingsValid = false;
        }

        // Primvars support vec3 output (though some channels may not be
        // used).
        if (_aovNames[i].isPrimvar && format != HdFormatFloat32Vec3)
        {
            TF_WARN(
                "Aov 'primvars:%s' has unsupported format '%s'",
                _aovNames[i].name.GetText(),
                TfEnum::GetName(format).c_str());
            _aovBindingsValid = false;
        }

        // color is only supported for vec3/vec4 attachments of float,
        // unorm, or snorm.
        if (_aovNames[i].name == HdAovTokens->color)
        {
            switch (format)
            {
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
        if (!_aovBindings[i].clearValue.IsEmpty())
        {
            HdTupleType clearType =
                HdGetValueTupleType(_aovBindings[i].clearValue);

            // array-valued clear types aren't supported.
            if (clearType.count != 1)
            {
                TF_WARN(
                    "Aov '%s' clear value type '%s' is an array",
                    _aovNames[i].name.GetText(),
                    _aovBindings[i].clearValue.GetTypeName().c_str());
                _aovBindingsValid = false;
            }

            // color only supports float/double vec3/4
            if (_aovNames[i].name == HdAovTokens->color &&
                clearType.type != HdTypeFloatVec3 &&
                clearType.type != HdTypeFloatVec4 &&
                clearType.type != HdTypeDoubleVec3 &&
                clearType.type != HdTypeDoubleVec4)
            {
                TF_WARN(
                    "Aov '%s' clear value type '%s' isn't compatible",
                    _aovNames[i].name.GetText(),
                    _aovBindings[i].clearValue.GetTypeName().c_str());
                _aovBindingsValid = false;
            }

            // only clear float formats with float, int with int, float3
            // with float3.
            if ((format == HdFormatFloat32 &&
                 clearType.type != HdTypeFloat) ||
                (format == HdFormatInt32 &&
                 clearType.type != HdTypeInt32) ||
                (format == HdFormatFloat32Vec3 &&
                 clearType.type != HdTypeFloatVec3))
            {
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


PXR_NAMESPACE_CLOSE_SCOPE
