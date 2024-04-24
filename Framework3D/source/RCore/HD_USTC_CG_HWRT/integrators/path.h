#pragma once
#include "USTC_CG.h"
#include "integrator.h"
#include "pxr/pxr.h"

USTC_CG_NAMESPACE_OPEN_SCOPE
using namespace pxr;
class PathIntegrator : public SamplingIntegrator {
   public:
    PathIntegrator(
        const Hd_USTC_CG_Camera* camera,
        Hd_USTC_CG_RenderBuffer* render_buffer,
        HdRenderThread* render_thread)
        : SamplingIntegrator(camera, render_buffer, render_thread)
    {
    }

   protected:
    VtValue Li(const GfRay& ray, std::default_random_engine& uniform_float) override;

    GfVec3f EstimateOutGoingRadiance(
        const GfRay& ray,
        const std::function<float()>& uniform_float,
        int recursion_depth);
};

USTC_CG_NAMESPACE_CLOSE_SCOPE
