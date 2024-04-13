#include "ao.h"

#include "context.h"
#include "embree4/rtcore.h"
#include "pxr/base/gf/matrix3f.h"
#include "pxr/imaging/hd/rprim.h"
#include "surfaceInteraction.h"

USTC_CG_NAMESPACE_OPEN_SCOPE
using namespace pxr;

static GfVec3f _CosineWeightedDirection(const GfVec2f& uniform_float)
{
    GfVec3f dir;
    float theta = 2.0f * M_PI * uniform_float[0];
    float eta = uniform_float[1];
    float sqrteta = sqrtf(eta);
    dir[0] = cosf(theta) * sqrteta;
    dir[1] = sinf(theta) * sqrteta;
    dir[2] = sqrtf(1.0f - eta);
    return dir;
}

VtValue AOIntegrator::Li(const GfRay& ray, std::default_random_engine& random)
{
    std::uniform_real_distribution<float> uniform_dist(0.0f, 1.0f);
    std::function<float()> uniform_float = std::bind(uniform_dist, random);

    SurfaceInteraction si;
    if (!Intersect(ray, si))
        return VtValue(GfVec4f{ 0, 0, 0, 1 });

    if (GfDot(si.normal, ray.GetDirection()) > 0) {
        si.normal *= -1;
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
        GfVec3f shadowDir = si.TangentToWorld(_CosineWeightedDirection(samples[i]));
        GfRay shadow_ray;
        shadow_ray.SetPointAndDirection(si.position, shadowDir);

        if (VisibilityTest(shadow_ray))
            color += GfDot(shadowDir, si.normal);
    }
    // Compute the average of the occlusion samples.
    color /= spp;

    return VtValue(GfVec4f(color, color, color, 1));
}

USTC_CG_NAMESPACE_CLOSE_SCOPE
