#include "integrator.h"

#include <boost/functional/hash.hpp>
#include <functional>
#include <random>

#include "config.h"
#include "context.h"
#include "pxr/base/gf/matrix3f.h"
#include "pxr/base/work/loops.h"
#include "pxr/imaging/hd/rprim.h"
#include "pxr/pxr.h"
#include "surfaceInteraction.h"
USTC_CG_NAMESPACE_OPEN_SCOPE
using namespace pxr;
static unsigned channel(VtValue val)
{
    if (val.CanCast<float>()) {
        return 1;
    }
    else if (val.CanCast<GfVec4f>()) {
        return 4;
    }
    else if (val.CanCast<GfVec3f>()) {
        return 3;
    }

    TF_CODING_ERROR("val must can cast to those types");
    return 0;
}
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

bool Integrator::Intersect(const GfRay& ray, SurfaceInteraction& si)
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

bool Integrator::VisibilityTest(const GfRay& ray)
{
    RTCRay test_ray;
    _PopulateRay(&test_ray, ray.GetStartPoint(), ray.GetDirection(), 0.0001);

    rtcOccluded1(_scene, &test_ray);

    if (test_ray.tfar > 0.0f) {  // Then this is visible
        return true;
    }
    return false; // Not visible
}

void SamplingIntegrator::_writeBuffer(unsigned x, unsigned y, VtValue color)
{
    switch (channel(color)) {
        case 1: camera_->film->Write(GfVec3i(x, y, 1), 1, &color.Get<float>()); break;
        case 3: camera_->film->Write(GfVec3i(x, y, 1), 3, color.Get<GfVec3f>().data()); break;
        case 4: camera_->film->Write(GfVec3i(x, y, 1), 4, color.Get<GfVec4f>().data()); break;
        default:;
    }
}

void SamplingIntegrator::_RenderTiles(
    HdRenderThread* renderThread,
    size_t tileStart,
    size_t tileEnd)
{
    const unsigned int minX = camera_->_dataWindow.GetMinX();
    unsigned int minY = camera_->_dataWindow.GetMinY();
    const unsigned int maxX = camera_->_dataWindow.GetMaxX() + 1;
    unsigned int maxY = camera_->_dataWindow.GetMaxY() + 1;

    std::swap(minY, maxY);
    auto height = camera_->film->GetHeight();
    minY = height - minY;
    maxY = height - maxY;

    const unsigned int tileSize = Hd_USTC_CG_Config::GetInstance().tileSize;
    const unsigned int numTilesX = (camera_->_dataWindow.GetWidth() + tileSize - 1) / tileSize;

    // Initialize the RNG for this tile (each tile creates one as
    // a lazy way to do thread-local RNGs).
    size_t seed = std::chrono::system_clock::now().time_since_epoch().count();
    boost::hash_combine(seed, tileStart);
    std::default_random_engine random(seed);

    // Create a uniform distribution for jitter calculations.
    std::uniform_real_distribution<float> uniform_dist(0.0f, 1.0f);
    std::function<float()> uniform_float = [&uniform_dist, &random]() {
        return uniform_dist(random);
    };

    // _RenderTiles gets a range of tiles; iterate through them.
    for (unsigned int tile = tileStart; tile < tileEnd; ++tile) {
        // Cancellation point.
        if (renderThread && renderThread->IsStopRequested()) {
            break;
        }

        // Compute the pixel location of tile boundaries.
        const unsigned int tileY = tile / numTilesX;
        const unsigned int tileX = tile - tileY * numTilesX;
        // (Above is equivalent to: tileX = tile % numTilesX)
        const unsigned int x0 = tileX * tileSize + minX;
        const unsigned int y0 = tileY * tileSize + minY;
        // Clamp to data window, in case tileSize doesn't
        // neatly divide its with and height.
        const unsigned int x1 = std::min(x0 + tileSize, maxX);
        const unsigned int y1 = std::min(y0 + tileSize, maxY);
        // Loop over pixels casting rays.
        for (unsigned int y = y0; y < y1; ++y) {
            for (unsigned int x = x0; x < x1; ++x) {
                auto pixel_center_uv = GfVec2f(x, y);
                auto ray = camera_->generateRay(pixel_center_uv, uniform_float);
                auto color = Li(ray, random);
                _writeBuffer(x, y, color);
            }
        }
    }
}

void SamplingIntegrator::Render()
{
    camera_->film->Map();
    const unsigned int tileSize = Hd_USTC_CG_Config::GetInstance().tileSize;

    const unsigned int numTilesX = (camera_->_dataWindow.GetWidth() + tileSize - 1) / tileSize;
    const unsigned int numTilesY = (camera_->_dataWindow.GetHeight() + tileSize - 1) / tileSize;

    // Render by scheduling square tiles of the sample buffer in a parallel
    // for loop.
    // Always pass the renderThread to _RenderTiles to allow the first frame
    // to be interrupted.
    WorkParallelForN(
        numTilesX * numTilesY,
        std::bind(
            &SamplingIntegrator::_RenderTiles,
            this,
            render_thread_,
            std::placeholders::_1,
            std::placeholders::_2));

    camera_->film->Unmap();

    camera_->film->SetConverged(true);
}

USTC_CG_NAMESPACE_CLOSE_SCOPE
