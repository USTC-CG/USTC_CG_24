#include "direct.h"

#include "Utils/Logging/Logging.h"
#include "pxr/base/gf/vec3f.h"
#include "pxr/pxr.h"
#include "surfaceInteraction.h"
USTC_CG_NAMESPACE_OPEN_SCOPE
using namespace pxr;

VtValue DirectLightIntegrator::Li(const GfRay& ray, std::default_random_engine& random)
{
    std::uniform_real_distribution<float> uniform_dist(
        0.0f, 1.0f - std::numeric_limits<float>::epsilon());
    std::function<float()> uniform_float = std::bind(uniform_dist, random);

    SurfaceInteraction si;
    if (!Intersect(ray, si))
        return VtValue(GfVec3f{ 0, 0, 0 });

    // Flip the normal if opposite
    if (GfDot(si.shadingNormal, ray.GetDirection()) > 0) {
        si.flipNormal();
        si.PrepareTransforms();
    }

    GfVec3f color = EstimateDirectLight(si, uniform_float);

    color[0] = std::clamp(color[0],0.f,50.f);
    color[1] = std::clamp(color[1],0.f,50.f);
    color[2] = std::clamp(color[2],0.f,50.f);

    return VtValue(GfVec3f(color[0], color[1], color[2]));
}

USTC_CG_NAMESPACE_CLOSE_SCOPE
