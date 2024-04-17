#pragma once
#include <complex.h>

#include "USTC_CG.h"
#include "color.h"
#include "pxr/imaging/hd/light.h"
#include "pxr/imaging/hio/image.h"
#include "pxr/pxr.h"
#include "pxr/usd/sdf/assetPath.h"
#include "texture.h"

USTC_CG_NAMESPACE_OPEN_SCOPE
using namespace pxr;
class Hd_USTC_CG_Light : public HdLight {
   public:
    explicit Hd_USTC_CG_Light(const SdfPath& id, const TfToken& lightType)
        : HdLight(id),
          _lightType(lightType)
    {
    }

    void Sync(HdSceneDelegate* sceneDelegate, HdRenderParam* renderParam, HdDirtyBits* dirtyBits)
        override;
    HdDirtyBits GetInitialDirtyBitsMask() const override;
    virtual Color Sample(
        const GfVec3f& pos,
        GfVec3f& dir,
        GfVec3f& sampled_light_pos,
        float& sample_light_pdf,
        const std::function<float()>& uniform_float) = 0;
    virtual Color Intersect(const GfRay& ray, float& depth) = 0;

    bool IsDomeLight();

    void Finalize(HdRenderParam* renderParam) override;

protected:
    VtValue Get(TfToken const& token) const;
    // Stores the internal light type of this light. Of course, we can use polymorphism to do this.
    // But let's just keep it simple here.
    TfToken _lightType;
    // Cached states.
    TfHashMap<TfToken, VtValue, TfToken::HashFunctor> _params;
};

class Hd_USTC_CG_Sphere_Light : public Hd_USTC_CG_Light {
   public:
    Hd_USTC_CG_Sphere_Light(const SdfPath& id, const TfToken& lightType)
        : Hd_USTC_CG_Light(id, lightType)
    {
    }

    Color Sample(
        const GfVec3f& pos,
        GfVec3f& dir,
        GfVec3f& sampled_light_pos,
        float& sample_light_pdf,
        const std::function<float()>& uniform_float) override;
    Color Intersect(const GfRay& ray, float& depth) override;
    void Sync(HdSceneDelegate* sceneDelegate, HdRenderParam* renderParam, HdDirtyBits* dirtyBits)
        override;
    float radius;
    GfVec3f power;
    GfVec3f position;
    float area;
    GfVec3f irradiance;
};

class Hd_USTC_CG_Dome_Light : public Hd_USTC_CG_Light {
   public:
    Hd_USTC_CG_Dome_Light(const SdfPath& id, const TfToken& lightType)
        : Hd_USTC_CG_Light(id, lightType)
    {
    }

    Color Sample(
        const GfVec3f& pos,
        GfVec3f& dir,
        GfVec3f& sampled_light_pos,
        float& sample_light_pdf,
        const std::function<float()>& uniform_float) override;
    Color Intersect(const GfRay& ray, float& depth) override;
    void _PrepareDomeLight(SdfPath const& id, HdSceneDelegate* scene_delegate);
    void Sync(HdSceneDelegate* sceneDelegate, HdRenderParam* renderParam, HdDirtyBits* dirtyBits)
        override;

    Color Le(const GfVec3f& dir);
    void Finalize(HdRenderParam* renderParam) override;

   private:
    SdfAssetPath textureFileName;
    GfVec3f radiance;
    std::unique_ptr<Texture2D> texture = nullptr;
};

class Hd_USTC_CG_Distant_Light : public Hd_USTC_CG_Light {
   public:
    Hd_USTC_CG_Distant_Light(const SdfPath& id, const TfToken& lightType)
        : Hd_USTC_CG_Light(id, lightType)
    {
    }

    void Sync(HdSceneDelegate* sceneDelegate, HdRenderParam* renderParam, HdDirtyBits* dirtyBits)
        override;
    Color Sample(
        const GfVec3f& pos,
        GfVec3f& dir,
        GfVec3f& sampled_light_pos,
        float& sample_light_pdf,
        const std::function<float()>& uniform_float) override;
    Color Intersect(const GfRay& ray, float& depth) override;

   private:
    float angle;
    GfVec3f direction;
    GfVec3f radiance;
};

class Hd_USTC_CG_Rect_Light : public Hd_USTC_CG_Light {
   public:
    Hd_USTC_CG_Rect_Light(const SdfPath& id, const TfToken& lightType)
        : Hd_USTC_CG_Light(id, lightType)
    {
    }

    Color Sample(
        const GfVec3f& pos,
        GfVec3f& dir,
        GfVec3f& sampled_light_pos,
        float& sample_light_pdf,
        const std::function<float()>& uniform_float) override;
    Color Intersect(const GfRay& ray, float& depth) override;
    void Sync(HdSceneDelegate* sceneDelegate, HdRenderParam* renderParam, HdDirtyBits* dirtyBits)
        override;

   private:
    GfVec3f corner0;
    GfVec3f corner1;
    GfVec3f corner2;
    GfVec3f corner3;
    float width;
    float height;
    GfVec3f power;
};

USTC_CG_NAMESPACE_CLOSE_SCOPE
