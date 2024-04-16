#include "surfaceInteraction.h"

USTC_CG_NAMESPACE_OPEN_SCOPE

Color SurfaceInteraction::Sample(GfVec3f& dir, float& pdf, const std::function<float()>& function)
    const
{
    GfVec3f sampled_dir;
    auto wo = WorldToTangent(this->wo);
    const auto color = material->Sample(wo, sampled_dir, pdf, texcoord, function);
    dir = TangentToWorld(sampled_dir);
    return color;
}

Color SurfaceInteraction::Eval(GfVec3f wi) const
{
    auto wo = WorldToTangent(this->wo);
    return material->Eval(wi, wo, texcoord);
}

float SurfaceInteraction::Pdf(GfVec3f wi, GfVec3f wo) const
{
    return material->Pdf(wi, wo, texcoord);
}

void SurfaceInteraction::PrepareTransforms()
{
    tangentToWorld = constructONB(shadingNormal);
    worldToTangent = tangentToWorld.GetInverse();
}

GfVec3f SurfaceInteraction::TangentToWorld(const GfVec3f& v_tangent_space) const
{
    return tangentToWorld * v_tangent_space;
}

GfVec3f SurfaceInteraction::WorldToTangent(const GfVec3f& v_world_space) const
{
    return worldToTangent * v_world_space;
}

void SurfaceInteraction::flipNormal()
{
    shadingNormal *= -1;
    geometricNormal *= -1;
}

USTC_CG_NAMESPACE_CLOSE_SCOPE