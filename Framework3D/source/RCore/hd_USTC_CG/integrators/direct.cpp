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
        return VtValue(GfVec4f{ 0, 0, 0, 1 });

    // Flip the normal if opposite
    if (GfDot(si.normal, ray.GetDirection()) > 0) {
        si.normal *= -1;
    }

    // Do the multiple importance sampling here.

    // SampleLights(lights,)
}

USTC_CG_NAMESPACE_CLOSE_SCOPE
