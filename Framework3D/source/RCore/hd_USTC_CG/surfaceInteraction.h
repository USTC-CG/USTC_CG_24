#pragma once
#include "USTC_CG.h"
#include "color.h"
#include "material.h"

USTC_CG_NAMESPACE_OPEN_SCOPE
class Hd_USTC_CG_Material;

class SurfaceInteraction {
   public:
    GfVec3f position;
    GfVec3f normal;
    GfVec3f tangent;

    GfVec2f uv;

    Color Sample(GfVec3f& dir, float& pdf) const;
    Color Eval(GfVec3f wi, GfVec3f wo) const;
    float Pdf(GfVec3f wi, GfVec3f wo) const;

    Hd_USTC_CG_Material* material;
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

USTC_CG_NAMESPACE_CLOSE_SCOPE
