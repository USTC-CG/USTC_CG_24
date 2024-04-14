#pragma once
#include "USTC_CG.h"
#include "color.h"
#include "material.h"
#include "pxr/base/gf/matrix3f.h"
#include "utils/math.hpp"

USTC_CG_NAMESPACE_OPEN_SCOPE

class SurfaceInteraction {
   public:
    GfVec3f position;
    GfVec3f normal;
    GfVec3f tangent;

    GfVec2f uv;

    Color Sample(GfVec3f& dir, float& pdf) const;
    Color Eval(GfVec3f wi, GfVec3f wo) const;
    float Pdf(GfVec3f wi, GfVec3f wo) const;

    void PrepareTransforms();
    // This is for transforming vector! It would be different for transforming points.
    GfVec3f TangentToWorld(const GfVec3f& v_tangent_space);

    Hd_USTC_CG_Material* material;

   protected:
    GfMatrix3f basis;
};

inline Color SurfaceInteraction::Sample(GfVec3f& wi, float& pdf) const
{
    return material->Sample(wi, pdf, uv);
}

inline Color SurfaceInteraction::Eval(GfVec3f wi, GfVec3f wo) const
{
    return material->Eval(wi, wo, uv);
}

inline float SurfaceInteraction::Pdf(GfVec3f wi, GfVec3f wo) const
{
    return material->Pdf(wi, wo, uv);
}

inline void SurfaceInteraction::PrepareTransforms()
{
    basis = constructONB(normal);
}

inline GfVec3f SurfaceInteraction::TangentToWorld(const GfVec3f& v_tangent_space)
{
    return basis * v_tangent_space;
}

USTC_CG_NAMESPACE_CLOSE_SCOPE
