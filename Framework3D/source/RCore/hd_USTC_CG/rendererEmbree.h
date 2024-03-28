#pragma once
#include "USTC_CG.h"
#include "camera.h"
#include "embree4/rtcore_geometry.h"
#include "pxr/imaging/hd/renderThread.h"
#include "pxr/pxr.h"
#include "renderer.h"

USTC_CG_NAMESPACE_OPEN_SCOPE
using namespace pxr;
class Hd_USTC_CG_Renderer_Embree : public Hd_USTC_CG_Renderer {
   public:
    explicit Hd_USTC_CG_Renderer_Embree(Hd_USTC_CG_RenderParam* render_param);

    void Render(HdRenderThread* renderThread) override;
    void Clear() override;

    void SetScene(RTCScene scene);

   private:
    void _RenderTiles(HdRenderThread* renderThread, size_t tileStart, size_t tileEnd);
    static GfVec4f _GetClearColor(const VtValue& clearValue);
    RTCDevice _rtcDevice;

    RTCScene _rtcScene;

    bool _enableSceneColors;
    std::atomic<int> _completedSamples;

    int _ambientOcclusionSamples = 16;
    // A callback that interprets embree error codes and injects them into
    // the hydra logging system.
    static void HandleRtcError(void* userPtr, RTCError code, const char* msg);
};

USTC_CG_NAMESPACE_CLOSE_SCOPE
