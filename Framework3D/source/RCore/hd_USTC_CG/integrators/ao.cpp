#include "ao.h"

#include "context.h"
#include "embree4/rtcore.h"
#include "pxr/base/gf/matrix3f.h"

USTC_CG_NAMESPACE_OPEN_SCOPE
using namespace pxr;
/// Fill in an RTCRay structure from the given parameters.
static void _PopulateRay(
    RTCRay* ray,
    const GfVec3d& origin,
    const GfVec3d& dir,
    float nearest)
{
    ray->org_x = origin[0];
    ray->org_y = origin[1];
    ray->org_z = origin[2];
    ray->tnear = nearest;

    ray->dir_x = dir[0];
    ray->dir_y = dir[1];
    ray->dir_z = dir[2];
    ray->time = 0.0f;

    ray->tfar = std::numeric_limits<float>::infinity();
    ray->mask = -1;
}

/// Fill in an RTCRayHit structure from the given parameters.
// note this containts a Ray and a RayHit
static void _PopulateRayHit(
    RTCRayHit* rayHit,
    const GfVec3d& origin,
    const GfVec3d& dir,
    float nearest)
{
    // Fill in defaults for the ray
    _PopulateRay(&rayHit->ray, origin, dir, nearest);

    // Fill in defaults for the hit
    rayHit->hit.primID = RTC_INVALID_GEOMETRY_ID;
    rayHit->hit.geomID = RTC_INVALID_GEOMETRY_ID;
}

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
    // auto dir = GfVec3f(ray.GetDirection());
    // return VtValue(GfVec4f(abs(dir[0]), abs(dir[1]), abs(dir[2]), 1));

    // Create a uniform random distribution for AO calculations.
    std::uniform_real_distribution<float> uniform_dist(0.0f, 1.0f);
    std::function<float()> uniform_float = std::bind(uniform_dist, random);
    RTCRayHit rayHit; // EMBREE_FIXME: use RTCRay for occlusion rays
    rayHit.ray.flags = 0;
    _PopulateRayHit(&rayHit, ray.GetStartPoint(), ray.GetDirection(), 0.0f);
    {
        rtcIntersect1(_scene, &rayHit);

        rayHit.hit.Ng_x = -rayHit.hit.Ng_x;
        rayHit.hit.Ng_y = -rayHit.hit.Ng_y;
        rayHit.hit.Ng_z = -rayHit.hit.Ng_z;
    }

    if (rayHit.hit.geomID == RTC_INVALID_GEOMETRY_ID) {
        return VtValue(GfVec4f(0.0f));
    }
    // Get the instance and prototype context structures for the hit prim.
    // We don't use embree's multi-level instancing; we
    // flatten everything in hydra. So instID[0] should always be correct.
    const HdEmbreeInstanceContext* instanceContext =
        static_cast<HdEmbreeInstanceContext*>(rtcGetGeometryUserData(
            rtcGetGeometry(_scene, rayHit.hit.instID[0])));

    const HdEmbreePrototypeContext* prototypeContext =
        static_cast<HdEmbreePrototypeContext*>(rtcGetGeometryUserData(
            rtcGetGeometry(instanceContext->rootScene, rayHit.hit.geomID)));

    // Compute the worldspace location of the rayHit hit.
    auto hitPos = GfVec3f(
        rayHit.ray.org_x + rayHit.ray.tfar * rayHit.ray.dir_x,
        rayHit.ray.org_y + rayHit.ray.tfar * rayHit.ray.dir_y,
        rayHit.ray.org_z + rayHit.ray.tfar * rayHit.ray.dir_z);

    // If a normal primvar is present (e.g. from smooth shading), use that
    // for shading; otherwise use the flat face normal.
    GfVec3f normal =
        -GfVec3f(rayHit.hit.Ng_x, rayHit.hit.Ng_y, rayHit.hit.Ng_z);

    // Transform the normal from object space to world space.
    normal = instanceContext->objectToWorldMatrix.TransformDir(normal);

    // Make sure the normal is unit-length.
    normal.Normalize();

    const int _ambientOcclusionSamples = 16;

    // 0 ambient occlusion samples means disable the ambient occlusion term.
    if (_ambientOcclusionSamples < 1) {
        return VtValue(GfVec4f(1.0f));
    }

    float color = 0.0f;

    // For hemisphere sampling we need to choose a coordinate frame at this
    // point. For the purposes of _CosineWeightedDirection, the normal needs
    // to map to (0,0,1), but since the distribution is radially symmetric
    // we don't care about the other axes.
    GfMatrix3f basis(1);
    GfVec3f xAxis;
    if (fabsf(GfDot(normal, GfVec3f(0, 0, 1))) < 0.9f) {
        xAxis = GfCross(normal, GfVec3f(0, 0, 1));
    }
    else {
        xAxis = GfCross(normal, GfVec3f(0, 1, 0));
    }
    GfVec3f yAxis = GfCross(normal, xAxis);
    basis.SetColumn(0, xAxis.GetNormalized());
    basis.SetColumn(1, yAxis.GetNormalized());
    basis.SetColumn(2, normal);

    // Generate random samples, stratified with Latin Hypercube Sampling.
    // https://en.wikipedia.org/wiki/Latin_hypercube_sampling
    // Stratified sampling means we don't get all of our random samples
    // bunched in the far corner of the hemisphere, but instead have some
    // equal spacing guarantees.
    std::vector<GfVec2f> samples;
    samples.resize(_ambientOcclusionSamples);
    for (int i = 0; i < _ambientOcclusionSamples; ++i) {
        samples[i][0] = (float(i) + uniform_float()) / _ambientOcclusionSamples;
    }
    std::shuffle(samples.begin(), samples.end(), random);
    for (int i = 0; i < _ambientOcclusionSamples; ++i) {
        samples[i][1] = (float(i) + uniform_float()) / _ambientOcclusionSamples;
    }

    // Trace ambient occlusion rays. The occlusion factor is the fraction of
    // the hemisphere that's occluded when rays are traced to infinity,
    // computed by random sampling over the hemisphere.
    for (int i = 0; i < _ambientOcclusionSamples; i++) {
        // Sample in the hemisphere centered on the face normal. Use
        // cosine-weighted hemisphere sampling to bias towards samples which
        // will have a bigger effect on the occlusion term.
        GfVec3f shadowDir = basis * _CosineWeightedDirection(samples[i]);

        // Trace shadow ray, using the fast interface (rtcOccluded) since
        // we only care about intersection status, not intersection id.
        RTCRay shadow;
        shadow.flags = 0;
        _PopulateRay(&shadow, hitPos, shadowDir, 0.001f);
        {
            rtcOccluded1(_scene, &shadow);
        }

        // Record this AO ray's contribution to the occlusion factor: a
        // boolean [In shadow/Not in shadow].
        // shadow is occluded when shadow.ray.tfar < 0.0f
        // notice this is reversed since "it's a visibility ray, and
        // the occlusionFactor is really an ambientLightFactor."
        if (shadow.tfar > 0.0f)
            color += GfDot(shadowDir, normal);
    }
    // Compute the average of the occlusion samples.
    color /= _ambientOcclusionSamples;

    return VtValue(GfVec4f(color, color, color, 1));
}

USTC_CG_NAMESPACE_CLOSE_SCOPE
