#pragma once

#include "camera.h"
#include "pxr/pxr.h"
#include "pxr/base/gf/matrix4d.h"
#include "pxr/base/gf/rect2i.h"
#include "pxr/imaging/hd/aov.h"
#include "pxr/imaging/hd/renderThread.h"

PXR_NAMESPACE_OPEN_SCOPE
class Hd_USTC_CG_GL_Renderer
{
public:
    virtual ~Hd_USTC_CG_GL_Renderer() = default;
    void SetAovBindings(const HdRenderPassAovBindingVector& aovBindings);
    virtual void Render(HdRenderThread* render_thread) = 0;
    virtual void Clear() = 0;

    void MarkAovBuffersUnconverged();


    void renderTimeUpdateCamera(
        const HdRenderPassStateSharedPtr&
        renderPassState);

protected:
    // The bound aovs for this renderer.
    HdRenderPassAovBindingVector _aovBindings;
    // Parsed AOV name tokens.
    HdParsedAovTokenVector _aovNames;
    // Do the aov bindings need to be re-validated?
    bool _aovBindingsNeedValidation;
    // Are the aov bindings valid?
    bool _aovBindingsValid;


    const Hd_USTC_CG_GL_Camera* camera_;


    bool _ValidateAovBindings();
};


PXR_NAMESPACE_CLOSE_SCOPE
