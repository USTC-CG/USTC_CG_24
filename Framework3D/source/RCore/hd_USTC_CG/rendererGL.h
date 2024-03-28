#pragma once
#include "USTC_CG.h"
#include "renderer.h"

USTC_CG_NAMESPACE_OPEN_SCOPE
using namespace pxr;
class Hd_USTC_CG_Renderer_GL : public Hd_USTC_CG_Renderer {
public:
    explicit Hd_USTC_CG_Renderer_GL(Hd_USTC_CG_RenderParam* render_param)
        : Hd_USTC_CG_Renderer(render_param)
    {
    }

    void Render(HdRenderThread* render_thread) override;
    void Clear() override;
};

USTC_CG_NAMESPACE_CLOSE_SCOPE