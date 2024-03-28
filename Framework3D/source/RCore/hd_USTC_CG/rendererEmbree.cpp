#include "rendererEmbree.h"

#include <random>

#include "context.h"
#include "embree4/rtcore.h"
#include "integrator.h"
#include "integrators/ao.h"
#include "pxr/base/gf/matrix3f.h"
#include "pxr/base/gf/vec2f.h"
#include "pxr/base/work/loops.h"
#include "pxr/imaging/hd/meshUtil.h"
#include "pxr/imaging/hd/rprim.h"
#include "pxr/imaging/hd/tokens.h"
#include "renderBuffer.h"
#include "renderParam.h"

USTC_CG_NAMESPACE_OPEN_SCOPE
using namespace pxr;

Hd_USTC_CG_Renderer_Embree::Hd_USTC_CG_Renderer_Embree(Hd_USTC_CG_RenderParam* render_param)
    : Hd_USTC_CG_Renderer(render_param)
{
    _rtcDevice = rtcNewDevice(nullptr);
    rtcSetDeviceErrorFunction(_rtcDevice, HandleRtcError, NULL);

    _rtcScene = rtcNewScene(_rtcDevice);
    rtcSetSceneFlags(_rtcScene, RTC_SCENE_FLAG_DYNAMIC);

    rtcSetSceneBuildQuality(_rtcScene, RTC_BUILD_QUALITY_LOW);

    render_param->_scene = _rtcScene;
    render_param->_device = _rtcDevice;
}

void Hd_USTC_CG_Renderer_Embree::Render(HdRenderThread* renderThread)
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

    auto integrator = std::make_shared<AOIntegrator>(
        camera_, static_cast<Hd_USTC_CG_RenderBuffer*>(_aovBindings[0].renderBuffer), renderThread);

    integrator->_scene = _rtcScene;

    integrator->Render();
}

void Hd_USTC_CG_Renderer_Embree::Clear()
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

void Hd_USTC_CG_Renderer_Embree::SetScene(RTCScene scene)
{
    _rtcScene = scene;
}

/* static */
GfVec4f Hd_USTC_CG_Renderer_Embree::_GetClearColor(const VtValue& clearValue)
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

void Hd_USTC_CG_Renderer_Embree::HandleRtcError(void* userPtr, RTCError code, const char* msg)
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
USTC_CG_NAMESPACE_CLOSE_SCOPE
