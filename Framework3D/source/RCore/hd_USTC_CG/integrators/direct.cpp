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

    //// Do the multiple importance sampling here.
    //// Sample Lights
    GfVec3f color;
    GfVec3f wi;
    float sample_light_pdf;
    GfVec3f sampled_light_pos;
    auto sample_light_luminance =
        SampleLights(si.position, wi, sampled_light_pos, sample_light_pdf, uniform_float);
    auto brdfVal = si.Eval(wi);
    GfVec3f contribution_by_sample_lights{ 0 };

    if (this->VisibilityTest(sampled_light_pos, si.position + 0.0001f * si.geometricNormal)) {
        contribution_by_sample_lights = GfCompMult(sample_light_luminance, brdfVal) *
                                        abs(GfDot(si.geometricNormal, wi)) / sample_light_pdf;
    }

    // Sample BRDF
    GfVec3f sampled_brdf_dir;
    float sample_brdf_pdf;
    brdfVal = si.Sample(sampled_brdf_dir, sample_brdf_pdf, uniform_float);
    GfRay light_ray{ si.position, sampled_brdf_dir.GetNormalized() };
    GfVec3f intersect_pos;
    auto sample_brdf_luminance = IntersectLights(light_ray, intersect_pos);
    GfVec3f contribution_by_sample_brdf{ 0 };

    if (this->VisibilityTest(si.position + 0.0001 * si.geometricNormal, intersect_pos)) {
        contribution_by_sample_brdf = GfCompMult(sample_brdf_luminance, brdfVal) *
                                      abs(GfDot(si.geometricNormal, sampled_brdf_dir)) /
                                      sample_brdf_pdf;
    }

    float light_sample_weight = PowerHeuristic(sample_light_pdf, sample_brdf_pdf);

    color = light_sample_weight * contribution_by_sample_lights +
            (1 - light_sample_weight) * contribution_by_sample_brdf;

    return VtValue(GfVec3f(color[0], color[1], color[2]));
}

USTC_CG_NAMESPACE_CLOSE_SCOPE
