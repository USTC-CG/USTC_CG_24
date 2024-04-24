#include "ao.h"

#include "context.h"
#include "embree4/rtcore.h"
#include "pxr/base/gf/matrix3f.h"
#include "pxr/imaging/hd/rprim.h"
#include "surfaceInteraction.h"
#include "utils/sampling.hpp"

USTC_CG_NAMESPACE_OPEN_SCOPE
using namespace pxr;

VtValue AOIntegrator::Li(const GfRay& ray, std::default_random_engine& random)
{
    std::uniform_real_distribution<float> uniform_dist(0.0f, 1.0f);
    std::function<float()> uniform_float = std::bind(uniform_dist, random);

    SurfaceInteraction si;
    if (!Intersect(ray, si))
        return VtValue(GfVec4f{ 0, 0, 0, 1 });

    // Flip the normal if opposite
    if (GfDot(si.shadingNormal, ray.GetDirection()) > 0) {
        si.flipNormal();
        si.PrepareTransforms();
    }

    float color = 0.0f;

    std::vector<GfVec2f> samples;
    samples.resize(spp);
    for (int i = 0; i < spp; ++i) {
        samples[i][0] = (float(i) + uniform_float()) / spp;
    }
    std::shuffle(samples.begin(), samples.end(), random);
    for (int i = 0; i < spp; ++i) {
        samples[i][1] = (float(i) + uniform_float()) / spp;
    }

    for (int i = 0; i < spp; i++) {
        float pdf;
        GfVec3f shadowDir = si.TangentToWorld(CosineWeightedDirection(samples[i], pdf));
        GfRay shadow_ray;
        shadow_ray.SetPointAndDirection(si.position + 0.00001f * si.geometricNormal, shadowDir);

        if (VisibilityTest(shadow_ray))
            color += GfDot(shadowDir, si.shadingNormal) / pdf;
    }
    color /= spp;

    return VtValue(GfVec4f(color, color, color, 1));
}

USTC_CG_NAMESPACE_CLOSE_SCOPE
