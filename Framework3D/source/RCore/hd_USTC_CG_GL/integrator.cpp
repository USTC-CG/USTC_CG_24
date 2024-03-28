#include "integrator.h"

#include <functional>
#include <random>
#include <boost/functional/hash.hpp>
#include "config.h"
#include "context.h"
#include "pxr/pxr.h"
#include "pxr/base/gf/matrix3f.h"
#include "pxr/base/work/loops.h"
USTC_CG_NAMESPACE_OPEN_SCOPE
using namespace pxr;
static unsigned channel(VtValue val)
{
    if (val.CanCast<float>())
    {
        return 1;
    }
    else if (val.CanCast<GfVec4f>())
    {
        return 4;
    }
    else if (val.CanCast<GfVec3f>())
    {
        return 3;
    }

    TF_CODING_ERROR("val must can cast to those types");
    return 0;
}


void SamplingIntegrator::_writeBuffer(unsigned x, unsigned y, VtValue color)
{
    switch (channel(color))
    {
        case 1: camera_->film->Write(
                GfVec3i(x, y, 1),
                1,
                &color.Get<float>());

            break;
        case 3: camera_->film->Write(
                GfVec3i(x, y, 1),
                3,
                color.Get<GfVec3f>().data());
            break;
        case 4: camera_->film->Write(
                GfVec3i(x, y, 1),
                4,
                color.Get<GfVec4f>().data());
            break;
        default: ;
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

    const unsigned int tileSize = HdEmbreeConfig::GetInstance().tileSize;
    const unsigned int numTilesX =
        (camera_->_dataWindow.GetWidth() + tileSize - 1) / tileSize;

    // Initialize the RNG for this tile (each tile creates one as
    // a lazy way to do thread-local RNGs).
    size_t seed = std::chrono::system_clock::now().time_since_epoch().count();
    boost::hash_combine(seed, tileStart);
    std::default_random_engine random(seed);

    // Create a uniform distribution for jitter calculations.
    std::uniform_real_distribution<float> uniform_dist(0.0f, 1.0f);
    std::function<float()> uniform_float = [&uniform_dist, &random]()
    {
        return uniform_dist(random);
    };

    // _RenderTiles gets a range of tiles; iterate through them.
    for (unsigned int tile = tileStart; tile < tileEnd; ++tile)
    {
        // Cancellation point.
        if (renderThread && renderThread->IsStopRequested())
        {
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
        for (unsigned int y = y0; y < y1; ++y)
        {
            for (unsigned int x = x0; x < x1; ++x)
            {
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
    const unsigned int tileSize = HdEmbreeConfig::GetInstance().tileSize;

    const unsigned int numTilesX =
        (camera_->_dataWindow.GetWidth() + tileSize - 1) / tileSize;
    const unsigned int numTilesY =
        (camera_->_dataWindow.GetHeight() + tileSize - 1) / tileSize;

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
