#include "path.h"

#include <random>

#include "Utils/Logging/Logging.h"
#include "pxr/pxr.h"
#include "surfaceInteraction.h"
USTC_CG_NAMESPACE_OPEN_SCOPE
using namespace pxr;

VtValue PathIntegrator::Li(const GfRay& ray, std::default_random_engine& random)
{
    std::uniform_real_distribution<float> uniform_dist(
        0.0f, 1.0f - std::numeric_limits<float>::epsilon());
    std::function<float()> uniform_float = std::bind(uniform_dist, random);

    SurfaceInteraction si;
    if (!Intersect(ray, si))
        return VtValue(GfVec3f{ 0, 0, 0 });

    // Flip the normal if opposite
    if (GfDot(si.geometricNormal, ray.GetDirection()) > 0) {
        si.geometricNormal *= -1;
        si.PrepareTransforms();
    }

    GfVec3f color = EstimateDirectLight(si, uniform_float);

    return VtValue(GfVec3f(color[0], color[1], color[2]));
}

USTC_CG_NAMESPACE_CLOSE_SCOPE
