#include "direct.h"

#include "Utils/Logging/Logging.h"
#include "pxr/pxr.h"
#include "surfaceInteraction.h"
USTC_CG_NAMESPACE_OPEN_SCOPE
using namespace pxr;

inline float PowerHeuristic(float f, float g)
{
    return f * f / (f * f + g * g);
}

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
        si.PrepareTransforms();
    }

    //// Do the multiple importance sampling here.

    //// Sample Lights
    GfVec3f color;
    GfVec3f wi;
    float sample_light_pdf;
    auto sample_light_luminance = SampleLights(si.position, wi, sample_light_pdf, random);
    auto wo = -GfVec3f(ray.GetDirection());
    auto brdfVal = si.Eval(wo, wi);
    GfVec3f contribution_by_sample_lights{ 0 };

    if (this->VisibilityTest(GfRay(si.position, wi))) {
        contribution_by_sample_lights = GfCompMult(sample_light_luminance, brdfVal) *
                                        abs(GfDot(si.normal, wi)) / sample_light_pdf;
    }

    // Sample BRDF

    GfVec3f sampled_brdf_dir;
    float sample_brdf_pdf;
    brdfVal = si.Sample(sampled_brdf_dir, sample_brdf_pdf, uniform_float);
    GfRay light_ray{ si.position, sampled_brdf_dir.GetNormalized() };
    auto sample_brdf_luminance = IntersectLights(light_ray);
    GfVec3f contribution_by_sample_brdf{ 0 };

    if (this->VisibilityTest(GfRay(si.position, sampled_brdf_dir))) {
        contribution_by_sample_brdf = GfCompMult(sample_brdf_luminance, brdfVal) *
                                      abs(GfDot(si.normal, sampled_brdf_dir)) / sample_brdf_pdf;
    }

    float light_sample_weight = PowerHeuristic(sample_light_pdf, sample_brdf_pdf);

    color = light_sample_weight * contribution_by_sample_lights +
            (1 - light_sample_weight) * contribution_by_sample_brdf;

    color = brdfVal;

    return VtValue(GfVec3f(color[0], color[1], color[2]));
}

USTC_CG_NAMESPACE_CLOSE_SCOPE
