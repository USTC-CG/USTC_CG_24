#include "ao.h"

#include "context.h"
#include "embree4/rtcore.h"
#include "pxr/base/gf/matrix3f.h"
#include "pxr/imaging/hd/rprim.h"
#include "surfaceInteraction.h"

USTC_CG_NAMESPACE_OPEN_SCOPE
using namespace pxr;
/// Fill in an RTCRay structure from the given parameters.
static void _PopulateRay(RTCRay* ray, const GfVec3d& origin, const GfVec3d& dir, float nearest)
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
static void
_PopulateRayHit(RTCRayHit* rayHit, const GfVec3d& origin, const GfVec3d& dir, float nearest)
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

bool AOIntegrator::Intersect(const GfRay& ray, SurfaceInteraction& si)
{
    RTCRayHit rayHit;
    rayHit.ray.flags = 0;
    _PopulateRayHit(&rayHit, ray.GetStartPoint(), ray.GetDirection(), 0.0f);
    {
        rtcIntersect1(_scene, &rayHit);

        rayHit.hit.Ng_x = -rayHit.hit.Ng_x;
        rayHit.hit.Ng_y = -rayHit.hit.Ng_y;
        rayHit.hit.Ng_z = -rayHit.hit.Ng_z;
    }

    if (rayHit.hit.geomID == RTC_INVALID_GEOMETRY_ID) {
        return false;
    }

    const Hd_USTC_CG_InstanceContext* instanceContext = static_cast<Hd_USTC_CG_InstanceContext*>(
        rtcGetGeometryUserData(rtcGetGeometry(_scene, rayHit.hit.instID[0])));

    const Hd_USTC_CG_PrototypeContext* prototypeContext = static_cast<Hd_USTC_CG_PrototypeContext*>(
        rtcGetGeometryUserData(rtcGetGeometry(instanceContext->rootScene, rayHit.hit.geomID)));

    auto hitPos = GfVec3f(
        rayHit.ray.org_x + rayHit.ray.tfar * rayHit.ray.dir_x,
        rayHit.ray.org_y + rayHit.ray.tfar * rayHit.ray.dir_y,
        rayHit.ray.org_z + rayHit.ray.tfar * rayHit.ray.dir_z);

    auto normal = -GfVec3f(rayHit.hit.Ng_x, rayHit.hit.Ng_y, rayHit.hit.Ng_z);

    // Transform the normal from object space to world space.
    normal = instanceContext->objectToWorldMatrix.TransformDir(normal);

    auto it = prototypeContext->primvarMap.find(HdTokens->normals);
    if (it != prototypeContext->primvarMap.end()) {
        assert(it->second->Sample(rayHit.hit.primID, rayHit.hit.u, rayHit.hit.v, &normal));
    }

    normal.Normalize();
    auto materialId = prototypeContext->rprim->GetMaterialId();

    si.normal = normal;
    si.position = hitPos;
    si.uv = { rayHit.hit.u, rayHit.hit.v };
    return true;
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
    if (fabsf(GfDot(si.normal, GfVec3f(0, 0, 1))) < 0.9f) {
        xAxis = GfCross(si.normal, GfVec3f(0, 0, 1));
    }
    else {
        xAxis = GfCross(si.normal, GfVec3f(0, 1, 0));
    }
    GfVec3f yAxis = GfCross(si.normal, xAxis);
    basis.SetColumn(0, xAxis.GetNormalized());
    basis.SetColumn(1, yAxis.GetNormalized());
    basis.SetColumn(2, si.normal);

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
        _PopulateRay(&shadow, si.position, shadowDir, 0.001f);
        {
            rtcOccluded1(_scene, &shadow);
        }

        // Record this AO ray's contribution to the occlusion factor: a
        // boolean [In shadow/Not in shadow].
        // shadow is occluded when shadow.ray.tfar < 0.0f
        // notice this is reversed since "it's a visibility ray, and
        // the occlusionFactor is really an ambientLightFactor."
        if (shadow.tfar > 0.0f)
            color += GfDot(shadowDir, si.normal);
    }
    // Compute the average of the occlusion samples.
    color /= _ambientOcclusionSamples;

    return VtValue(GfVec4f(color, color, color, 1));
}

USTC_CG_NAMESPACE_CLOSE_SCOPE
