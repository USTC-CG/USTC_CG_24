#pragma once
#include <random>

#include "camera.h"
#include "color.h"
#include "embree4/rtcore_geometry.h"
#include "pxr/base/gf/rect2i.h"
#include "pxr/imaging/hd/renderThread.h"
#include "pxr/imaging/hd/sceneDelegate.h"
#include "pxr/pxr.h"
#include "renderBuffer.h"

USTC_CG_NAMESPACE_OPEN_SCOPE
class Hd_USTC_CG_RenderParam;
class SurfaceInteraction;
using namespace pxr;
class Integrator {
   public:
    Integrator(
        const Hd_USTC_CG_Camera* camera,
        Hd_USTC_CG_RenderBuffer* render_buffer,
        HdRenderThread* render_thread)
        : camera_(camera),
          render_thread_(render_thread)
    {
        camera_->attachFilm(render_buffer);
    }

    virtual ~Integrator() = default;
    virtual void Render() = 0;

    RTCScene rtc_scene;
    Hd_USTC_CG_RenderParam* render_param;

   protected:
    // All the following utility functions, it is not best practice for all of them to be here.
    // However, for educational purpose I shall just keep it simple and put them together.

    /**
     * \brief Sample light in scene
     * \param pos position on an object. Used to calculate pdf.
     * \param dir sampled direction
     * \param pdf returning the pdf of sampling such a direction. could be 0, which stands for delta
     * lights. \return
     */
    Color SampleLights(
        const GfVec3f& pos,
        GfVec3f& dir,
        GfVec3f& sampled_light_pos,
        float& pdf,
        const std::function<float()>& function);

    /**
     * \brief for now, we only use very limited count of lights, thus we don't use any BVH on lights
     * \param ray the brdf sampled ray
     * \return
     */
    Color IntersectLights(const GfRay& ray, GfVec3f& intersectPos);
    Color IntersectDomeLight(const GfRay& ray);


    bool Intersect(const GfRay& ray, SurfaceInteraction& si);
    bool VisibilityTest(const GfRay& ray);
    bool VisibilityTest(const GfVec3f& begin, const GfVec3f& end);

    Color EstimateDirectLight(SurfaceInteraction& si, const std::function<float()>& uniform_float);

    const Hd_USTC_CG_Camera* camera_;
    HdRenderThread* render_thread_;
};

class SamplingIntegrator : public Integrator {
   public:
    SamplingIntegrator(
        const Hd_USTC_CG_Camera* camera,
        Hd_USTC_CG_RenderBuffer* render_buffer,
        HdRenderThread* render_thread)
        : Integrator(camera, render_buffer, render_thread)
    {
    }

   protected:
    unsigned spp = 256;

    void _writeBuffer(unsigned x, unsigned y, VtValue color);

    virtual VtValue Li(const GfRay& ray, std::default_random_engine& uniform_float) = 0;
    void accumulate_color(VtValue& color, const VtValue& vt_value);
    VtValue average_samples(const VtValue& color, unsigned spp);
    void _RenderTiles(HdRenderThread* renderThread, size_t tileStart, size_t tileEnd);

   public:
    void Render() override;
};

USTC_CG_NAMESPACE_CLOSE_SCOPE
