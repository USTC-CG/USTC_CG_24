#pragma once
#include "USTC_CG.h"
#include "integrator.h"
#include "pxr/pxr.h"

USTC_CG_NAMESPACE_OPEN_SCOPE
using namespace pxr;
class DirectLightIntegrator : public SamplingIntegrator {
   public:
    DirectLightIntegrator(
        const Hd_USTC_CG_Camera* camera,
        Hd_USTC_CG_RenderBuffer* render_buffer,
        HdRenderThread* render_thread)
        : SamplingIntegrator(camera, render_buffer, render_thread)
    {
    }

   protected:
    VtValue Li(const GfRay& ray, std::default_random_engine& uniform_float) override;
};

USTC_CG_NAMESPACE_CLOSE_SCOPE
