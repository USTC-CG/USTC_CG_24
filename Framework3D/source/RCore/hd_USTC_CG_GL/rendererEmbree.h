#pragma once

#include <random>

#include "camera.h"
#include "renderer.h"
#include "embree4/rtcore_ray.h"
#include "pxr/imaging/hd/aov.h"
#include "pxr/imaging/hd/renderThread.h"
#include "pxr/pxr.h"

PXR_NAMESPACE_OPEN_SCOPE
class Hd_USTC_CG_Renderer_Embree : public Hd_USTC_CG_Renderer
{
public:
    void Render(HdRenderThread* renderThread) override;
    void Clear() override;


private:
    void _RenderTiles(
        HdRenderThread* renderThread,
        size_t tileStart,
        size_t tileEnd);
    static GfVec4f _GetClearColor(const VtValue& clearValue);

    bool _enableSceneColors;
    std::atomic<int> _completedSamples;

    int _ambientOcclusionSamples = 16;
};

PXR_NAMESPACE_CLOSE_SCOPE
