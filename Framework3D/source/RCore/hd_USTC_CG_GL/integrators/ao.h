#pragma once

#include <random>

#include "integrator.h"
#include "pxr/pxr.h"
#include "pxr/base/gf/ray.h"

USTC_CG_NAMESPACE_OPEN_SCOPE
using namespace pxr;
class AOIntegrator : public SamplingIntegrator
{
public:
    AOIntegrator(
        const Hd_USTC_CG_Camera* camera,
        Hd_USTC_CG_RenderBuffer* render_buffer,
        HdRenderThread* render_thread)
        : SamplingIntegrator(camera, render_buffer, render_thread)
    {
    }

protected:
    VtValue Li(const GfRay& ray, std::default_random_engine& uniform_float)
    override;
};

USTC_CG_NAMESPACE_CLOSE_SCOPE
