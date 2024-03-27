#pragma once
#include <random>

#include "camera.h"
#include "renderBuffer.h"
#include "pxr/pxr.h"
#include "pxr/base/gf/rect2i.h"
#include "pxr/imaging/hd/renderThread.h"
#include "pxr/imaging/hd/sceneDelegate.h"

PXR_NAMESPACE_OPEN_SCOPE
class Integrator
{
public:
    Integrator(
        const Hd_USTC_CG_GL_Camera* camera,
        Hd_USTC_CG_GL_RenderBuffer* render_buffer,
        HdRenderThread* render_thread)
        : camera_(camera),
          render_thread_(render_thread)
    {
        camera_->attachFilm(render_buffer);
    }

    virtual ~Integrator() = default;
    virtual void Render() = 0;


protected:
    const Hd_USTC_CG_GL_Camera* camera_;
    HdRenderThread* render_thread_;
};


class SamplingIntegrator : public Integrator
{
public:
    SamplingIntegrator(
        const Hd_USTC_CG_GL_Camera* camera,
        Hd_USTC_CG_GL_RenderBuffer* render_buffer,
        HdRenderThread* render_thread)
        : Integrator(camera, render_buffer, render_thread)
    {
    }

protected:
    void _writeBuffer(unsigned x, unsigned y, VtValue color);

    virtual VtValue Li(
        const GfRay& ray,
        std::default_random_engine& uniform_float) = 0;
    void _RenderTiles(
        HdRenderThread* renderThread,
        size_t tileStart,
        size_t tileEnd);

public:
    void Render() override;
};


PXR_NAMESPACE_CLOSE_SCOPE
