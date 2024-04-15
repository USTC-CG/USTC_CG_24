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
    GfVec3f wo;
    GfVec3f geometricNormal;
    GfVec3f tangent;

    GfVec2f uv;

    Color Sample(GfVec3f& dir, float& pdf, const std::function<float()>& function) const;
    Color Eval(GfVec3f wi) const;
    float Pdf(GfVec3f wi, GfVec3f wo) const;

    void PrepareTransforms();
    // This is for transforming vector! It would be different for transforming points.
    GfVec3f TangentToWorld(const GfVec3f& v_tangent_space) const;
    GfVec3f WorldToTangent(const GfVec3f& v_world_space) const;

    Hd_USTC_CG_Material* material;

   protected:
    GfMatrix3f tangentToWorld;
    GfMatrix3f worldToTangent;
};

inline Color
SurfaceInteraction::Sample(GfVec3f& dir, float& pdf, const std::function<float()>& function) const
{
    GfVec3f sampled_dir;
    auto wo = WorldToTangent(this->wo);
    const auto color = material->Sample(wo, sampled_dir, pdf, uv, function);
    dir = TangentToWorld(sampled_dir);
    return color;
}

inline Color SurfaceInteraction::Eval(GfVec3f wi) const
{
    auto wo = WorldToTangent(this->wo);
    return material->Eval(wi, wo, uv);
}

inline float SurfaceInteraction::Pdf(GfVec3f wi, GfVec3f wo) const
{
    return material->Pdf(wi, wo, uv);
}

inline void SurfaceInteraction::PrepareTransforms()
{
    tangentToWorld = constructONB(geometricNormal);
    worldToTangent = tangentToWorld.GetInverse();
}

inline GfVec3f SurfaceInteraction::TangentToWorld(const GfVec3f& v_tangent_space) const
{
    return tangentToWorld * v_tangent_space;
}

inline GfVec3f SurfaceInteraction::WorldToTangent(const GfVec3f& v_world_space) const
{
    return worldToTangent * v_world_space;
}

USTC_CG_NAMESPACE_CLOSE_SCOPE
