#pragma once
#include "USTC_CG.h"
#include "camera.h"
#include "pxr/imaging/hd/aov.h"
#include "pxr/imaging/hd/renderThread.h"
#include "pxr/pxr.h"

USTC_CG_NAMESPACE_OPEN_SCOPE
class Hd_USTC_CG_RenderParam;
using namespace pxr;
class Hd_USTC_CG_Renderer {
   public:
    explicit Hd_USTC_CG_Renderer(Hd_USTC_CG_RenderParam* render_param) : render_param_(render_param)
    {
    }

    virtual ~Hd_USTC_CG_Renderer() = default;
    void SetAovBindings(const HdRenderPassAovBindingVector& aovBindings);
    virtual void Render(HdRenderThread* render_thread) = 0;
    virtual void Clear() = 0;

    void MarkAovBuffersUnconverged();

    void renderTimeUpdateCamera(const HdRenderPassStateSharedPtr& renderPassState);

   protected:
    Hd_USTC_CG_RenderParam* render_param_;
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
