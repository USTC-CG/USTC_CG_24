#pragma once
#include "USTC_CG.h"
#include "camera.h"
#include "embree4/rtcore_geometry.h"
#include "pxr/imaging/hd/aov.h"
#include "pxr/imaging/hd/renderThread.h"
#include "pxr/pxr.h"
#include "renderer.h"
USTC_CG_NAMESPACE_OPEN_SCOPE
class Hd_USTC_CG_RenderParam;
using namespace pxr;
class Hd_USTC_CG_Renderer {
   public:
    explicit Hd_USTC_CG_Renderer(Hd_USTC_CG_RenderParam* render_param);

    virtual ~Hd_USTC_CG_Renderer() = default;
    void SetAovBindings(const HdRenderPassAovBindingVector& aovBindings);
    virtual void Render(HdRenderThread* render_thread);
    virtual void Clear();
    void SetScene(RTCScene scene);

    void MarkAovBuffersUnconverged();

    void renderTimeUpdateCamera(const HdRenderPassStateSharedPtr& renderPassState);

   protected:
    void _RenderTiles(HdRenderThread* renderThread, size_t tileStart, size_t tileEnd);
    static GfVec4f _GetClearColor(const VtValue& clearValue);
    RTCDevice _rtcDevice;

    RTCScene _rtcScene;

    bool _enableSceneColors;
    std::atomic<int> _completedSamples;

    Hd_USTC_CG_RenderParam* render_param;
    // A callback that interprets embree error codes and injects them into
    // the hydra logging system.
    static void HandleRtcError(void* userPtr, RTCError code, const char* msg);

    // The bound aovs for this renderer.
    HdRenderPassAovBindingVector _aovBindings;
    // Parsed AOV name tokens.
    HdParsedAovTokenVector _aovNames;
    // Do the aov bindings need to be re-validated?
    bool _aovBindingsNeedValidation = true;
    // Are the aov bindings valid?
    bool _aovBindingsValid = false;

    const Hd_USTC_CG_Camera* camera_ = nullptr;

    bool _ValidateAovBindings();
};

USTC_CG_NAMESPACE_CLOSE_SCOPE
