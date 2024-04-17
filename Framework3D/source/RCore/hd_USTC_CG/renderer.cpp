#include "renderer.h"

#include "embree4/rtcore_scene.h"
#include "integrators/ao.h"
#include "integrators/direct.h"
#include "pxr/imaging/hd/renderBuffer.h"
#include "pxr/imaging/hd/tokens.h"
#include "renderBuffer.h"
#include "renderParam.h"
#include "integrators/path.h"

USTC_CG_NAMESPACE_OPEN_SCOPE
using namespace pxr;

Hd_USTC_CG_Renderer::Hd_USTC_CG_Renderer(Hd_USTC_CG_RenderParam* render_param)
    : render_param(render_param)
{
    _rtcDevice = rtcNewDevice(nullptr);
    rtcSetDeviceErrorFunction(_rtcDevice, HandleRtcError, NULL);

    _rtcScene = rtcNewScene(_rtcDevice);
    rtcSetSceneFlags(_rtcScene, RTC_SCENE_FLAG_DYNAMIC);

    rtcSetSceneBuildQuality(_rtcScene, RTC_BUILD_QUALITY_LOW);

    render_param->_scene = _rtcScene;
    render_param->_device = _rtcDevice;
}

void Hd_USTC_CG_Renderer::Render(HdRenderThread* renderThread)
{
    _completedSamples.store(0);

    // Commit any pending changes to the scene.
    rtcCommitScene(_rtcScene);

    if (!_ValidateAovBindings()) {
        // We aren't going to render anything. Just mark all AOVs as converged
        // so that we will stop rendering.
        for (size_t i = 0; i < _aovBindings.size(); ++i) {
            auto rb = static_cast<Hd_USTC_CG_RenderBuffer*>(_aovBindings[i].renderBuffer);
            rb->SetConverged(true);
        }
        // XXX:validation
        TF_WARN("Could not validate Aovs. Render will not complete");
        return;
    }

    auto integrator = std::make_shared<DirectLightIntegrator>(
        camera_, static_cast<Hd_USTC_CG_RenderBuffer*>(_aovBindings[0].renderBuffer), renderThread);

    integrator->rtc_scene = _rtcScene;
    integrator->render_param = render_param;

    integrator->Render();
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

        auto rb = static_cast<Hd_USTC_CG_RenderBuffer*>(_aovBindings[i].renderBuffer);

        rb->Map();
        if (_aovNames[i].name == HdAovTokens->color) {
            GfVec4f clearColor = _GetClearColor(_aovBindings[i].clearValue);
            rb->Clear(4, clearColor.data());
        }
        else if (rb->GetFormat() == HdFormatInt32) {
            int32_t clearValue = _aovBindings[i].clearValue.Get<int32_t>();
            rb->Clear(1, &clearValue);
        }
        else if (rb->GetFormat() == HdFormatFloat32) {
            float clearValue = _aovBindings[i].clearValue.Get<float>();
            rb->Clear(1, &clearValue);
        }
        else if (rb->GetFormat() == HdFormatFloat32Vec3) {
            auto clearValue = _aovBindings[i].clearValue.Get<GfVec3f>();
            rb->Clear(3, clearValue.data());
        }  // else, _ValidateAovBindings would have already warned.

        rb->Unmap();
        rb->SetConverged(false);
    }
}

void Hd_USTC_CG_Renderer::SetScene(RTCScene scene)
{
    _rtcScene = scene;
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

void Hd_USTC_CG_Renderer::HandleRtcError(void* userPtr, RTCError code, const char* msg)
{
    // Forward RTC error messages through to hydra logging.
    switch (code) {
        case RTC_ERROR_UNKNOWN: TF_CODING_ERROR("Embree unknown error: %s", msg); break;
        case RTC_ERROR_INVALID_ARGUMENT: TF_CODING_ERROR("Embree invalid argument: %s", msg); break;
        case RTC_ERROR_INVALID_OPERATION:
            TF_CODING_ERROR("Embree invalid operation: %s", msg);
            break;
        case RTC_ERROR_OUT_OF_MEMORY: TF_CODING_ERROR("Embree out of memory: %s", msg); break;
        case RTC_ERROR_UNSUPPORTED_CPU: TF_CODING_ERROR("Embree unsupported CPU: %s", msg); break;
        case RTC_ERROR_CANCELLED: TF_CODING_ERROR("Embree cancelled: %s", msg); break;
        default: TF_CODING_ERROR("Embree invalid error code: %s", msg); break;
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
        auto rb = static_cast<Hd_USTC_CG_RenderBuffer*>(_aovBindings[i].renderBuffer);
        rb->SetConverged(false);
    }
}

void Hd_USTC_CG_Renderer::renderTimeUpdateCamera(const HdRenderPassStateSharedPtr& renderPassState)
{
    camera_ = static_cast<const Hd_USTC_CG_Camera*>(renderPassState->GetCamera());
    camera_->update(renderPassState);
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
