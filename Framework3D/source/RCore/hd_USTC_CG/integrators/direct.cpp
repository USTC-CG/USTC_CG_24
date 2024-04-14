#include "direct.h"

#include "pxr/pxr.h"
#include "surfaceInteraction.h"
USTC_CG_NAMESPACE_OPEN_SCOPE
using namespace pxr;

VtValue DirectLightIntegrator::Li(const GfRay& ray, std::default_random_engine& random)
{
    std::uniform_real_distribution<float> uniform_dist(0.0f, 1.0f);
    std::function<float()> uniform_float = std::bind(uniform_dist, random);

    SurfaceInteraction si;
    if (!Intersect(ray, si))
        return VtValue(GfVec3f{ 0, 0, 0 });

    // Flip the normal if opposite
    if (GfDot(si.normal, ray.GetDirection()) > 0) {
        si.normal *= -1;
    }

    // Do the multiple importance sampling here.

    GfVec3f color;
    GfVec3f wi;
    float sample_light_pdf;
    auto luminance = SampleLights(si.position, wi, sample_light_pdf, random);
    auto wo = -GfVec3f(ray.GetDirection());

    auto brdfVal = si.Eval(wo, wi);

    if (this->VisibilityTest(GfRay(si.position, wi))) {
        color = GfCompMult(luminance, brdfVal) * abs(GfDot(si.normal, wi)) / sample_light_pdf;
    }
    else {
        color = Color{ 0 };
    }

    // color = {0.3,0.0,0.5};
    // color = luminance;
    return VtValue(GfVec3f(color[0], color[1], color[2]));
    // SampleLights(lights,)
}

USTC_CG_NAMESPACE_CLOSE_SCOPE
