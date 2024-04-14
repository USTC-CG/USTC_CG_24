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

    GfVec3f color;
    for (int i = 0; i < spp; ++i) {
        GfVec3f dir;
        float sample_light;
        auto luminance = SampleLights(si.position, dir, sample_light, random);
    }

    return VtValue(GfVec4f(color[0], color[1], color[2], 1));
    // SampleLights(lights,)
}

USTC_CG_NAMESPACE_CLOSE_SCOPE
